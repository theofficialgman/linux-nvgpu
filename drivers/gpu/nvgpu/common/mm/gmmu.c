/*
 * Copyright (c) 2017, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <nvgpu/log.h>
#include <nvgpu/list.h>
#include <nvgpu/dma.h>
#include <nvgpu/gmmu.h>
#include <nvgpu/nvgpu_mem.h>
#include <nvgpu/enabled.h>
#include <nvgpu/page_allocator.h>

#include "gk20a/gk20a.h"
#include "gk20a/mm_gk20a.h"

#define __gmmu_dbg(g, attrs, fmt, args...)				\
	do {								\
		if (attrs->debug)					\
			nvgpu_info(g, fmt, ##args);			\
		else							\
			nvgpu_log(g, gpu_dbg_map, fmt, ##args);		\
	} while (0)

#define __gmmu_dbg_v(g, attrs, fmt, args...)				\
	do {								\
		if (attrs->debug)					\
			nvgpu_info(g, fmt, ##args);			\
		else							\
			nvgpu_log(g, gpu_dbg_map_v, fmt, ##args);	\
	} while (0)

static int pd_allocate(struct vm_gk20a *vm,
		       struct nvgpu_gmmu_pd *pd,
		       const struct gk20a_mmu_level *l,
		       struct nvgpu_gmmu_attrs *attrs);
static u32 pd_size(const struct gk20a_mmu_level *l,
		   struct nvgpu_gmmu_attrs *attrs);
/*
 * Core GMMU map function for the kernel to use. If @addr is 0 then the GPU
 * VA will be allocated for you. If addr is non-zero then the buffer will be
 * mapped at @addr.
 */
static u64 __nvgpu_gmmu_map(struct vm_gk20a *vm,
			    struct nvgpu_mem *mem,
			    u64 addr,
			    u64 size,
			    u32 flags,
			    int rw_flag,
			    bool priv,
			    enum nvgpu_aperture aperture)
{
	struct gk20a *g = gk20a_from_vm(vm);
	u64 vaddr;

	struct sg_table *sgt = mem->priv.sgt;

	nvgpu_mutex_acquire(&vm->update_gmmu_lock);
	vaddr = g->ops.mm.gmmu_map(vm, addr,
				   sgt,    /* sg table */
				   0,      /* sg offset */
				   size,
				   gmmu_page_size_kernel,
				   0,      /* kind */
				   0,      /* ctag_offset */
				   flags, rw_flag,
				   false,  /* clear_ctags */
				   false,  /* sparse */
				   priv,   /* priv */
				   NULL,   /* mapping_batch handle */
				   aperture);
	nvgpu_mutex_release(&vm->update_gmmu_lock);
	if (!vaddr) {
		nvgpu_err(g, "failed to allocate va space");
		return 0;
	}

	return vaddr;
}

/*
 * Convenience wrapper over __nvgpu_gmmu_map() for non-fixed mappings.
 */
u64 nvgpu_gmmu_map(struct vm_gk20a *vm,
		   struct nvgpu_mem *mem,
		   u64 size,
		   u32 flags,
		   int rw_flag,
		   bool priv,
		   enum nvgpu_aperture aperture)
{
	return __nvgpu_gmmu_map(vm, mem, 0, size, flags, rw_flag, priv,
			aperture);
}

/*
 * Like nvgpu_gmmu_map() except it can work on a fixed address instead.
 */
u64 nvgpu_gmmu_map_fixed(struct vm_gk20a *vm,
			 struct nvgpu_mem *mem,
			 u64 addr,
			 u64 size,
			 u32 flags,
			 int rw_flag,
			 bool priv,
			 enum nvgpu_aperture aperture)
{
	return __nvgpu_gmmu_map(vm, mem, addr, size, flags, rw_flag, priv,
			aperture);
}

void nvgpu_gmmu_unmap(struct vm_gk20a *vm, struct nvgpu_mem *mem, u64 gpu_va)
{
	struct gk20a *g = gk20a_from_vm(vm);

	nvgpu_mutex_acquire(&vm->update_gmmu_lock);
	g->ops.mm.gmmu_unmap(vm,
			     gpu_va,
			     mem->size,
			     gmmu_page_size_kernel,
			     true, /*va_allocated */
			     gk20a_mem_flag_none,
			     false,
			     NULL);

	nvgpu_mutex_release(&vm->update_gmmu_lock);
}

int nvgpu_gmmu_init_page_table(struct vm_gk20a *vm)
{
	u32 pdb_size;
	int err;

	/*
	 * Need this just for page size. Everything else can be ignored. Also
	 * note that we can just use pgsz 0 (i.e small pages) since the number
	 * of bits present in the top level PDE are the same for small/large
	 * page VMs.
	 */
	struct nvgpu_gmmu_attrs attrs = {
		.pgsz = 0,
	};

	/*
	 * PDB size here must be one page so that its address is page size
	 * aligned. Although lower PDE tables can be aligned at 256B boundaries
	 * the main PDB must be page aligned.
	 */
	pdb_size = ALIGN(pd_size(&vm->mmu_levels[0], &attrs), PAGE_SIZE);

	err = __nvgpu_pd_cache_alloc_direct(vm->mm->g, &vm->pdb, pdb_size);
	if (WARN_ON(err))
		return err;

	/*
	 * One mb() is done after all mapping operations. Don't need individual
	 * barriers for each PD write.
	 */
	vm->pdb.mem->skip_wmb = true;

	return 0;
}

/*
 * Ensure that there's a CPU mapping for the page directory memory. This won't
 * always be the case for 32 bit systems since we may need to save kernel
 * virtual memory.
 */
static int map_gmmu_pages(struct gk20a *g, struct nvgpu_gmmu_pd *pd)
{
	return nvgpu_mem_begin(g, pd->mem);
}

/*
 * Handle any necessary CPU unmap semantics for a page directories DMA memory.
 * For 64 bit platforms this is a noop.
 */
static void unmap_gmmu_pages(struct gk20a *g, struct nvgpu_gmmu_pd *pd)
{
	nvgpu_mem_end(g, pd->mem);
}

/*
 * Return the _physical_ address of a page directory.
 */
u64 nvgpu_pde_phys_addr(struct gk20a *g, struct nvgpu_gmmu_pd *pd)
{
	u64 page_addr;

	if (g->mm.has_physical_mode)
		page_addr = sg_phys(pd->mem->priv.sgt->sgl);
	else
		page_addr = nvgpu_mem_get_base_addr(g, pd->mem, 0);

	return page_addr + pd->mem_offs;
}

/*
 * Return the aligned length based on the page size in attrs.
 */
static u64 nvgpu_align_map_length(struct vm_gk20a *vm, u64 length,
				  struct nvgpu_gmmu_attrs *attrs)
{
	u64 page_size = vm->gmmu_page_sizes[attrs->pgsz];

	return ALIGN(length, page_size);
}

static u32 pd_entries(const struct gk20a_mmu_level *l,
		      struct nvgpu_gmmu_attrs *attrs)
{
	/*
	 * Number of entries in a PD is easy to compute from the number of bits
	 * used to index the page directory. That is simply 2 raised to the
	 * number of bits.
	 */
	return 1UL << (l->hi_bit[attrs->pgsz] - l->lo_bit[attrs->pgsz] + 1UL);
}

/*
 * Computes the size of a PD table.
 */
static u32 pd_size(const struct gk20a_mmu_level *l,
		   struct nvgpu_gmmu_attrs *attrs)
{
	return pd_entries(l, attrs) * l->entry_size;
}

/*
 * Allocate a physically contiguous region big enough for a gmmu page table
 * of the specified level and page size. The whole range is zeroed so that any
 * accesses will fault until proper values are programmed.
 */
static int pd_allocate(struct vm_gk20a *vm,
		       struct nvgpu_gmmu_pd *pd,
		       const struct gk20a_mmu_level *l,
		       struct nvgpu_gmmu_attrs *attrs)
{
	int err;

	if (pd->mem)
		return 0;

	err = __nvgpu_pd_alloc(vm, pd, pd_size(l, attrs));
	if (err) {
		nvgpu_info(vm->mm->g, "error allocating page directory!");
		return err;
	}

	/*
	 * One mb() is done after all mapping operations. Don't need individual
	 * barriers for each PD write.
	 */
	pd->mem->skip_wmb = true;

	return 0;
}

/*
 * Compute what page directory index at the passed level the passed virtual
 * address corresponds to. @attrs is necessary for determining the page size
 * which is used to pick the right bit offsets for the GMMU level.
 */
static u32 pd_index(const struct gk20a_mmu_level *l, u64 virt,
		    struct nvgpu_gmmu_attrs *attrs)
{
	u64 pd_mask = (1ULL << ((u64)l->hi_bit[attrs->pgsz] + 1)) - 1ULL;
	u32 pd_shift = (u64)l->lo_bit[attrs->pgsz];

	/*
	 * For convenience we don't bother computing the lower bound of the
	 * mask; it's easier to just shift it off.
	 */
	return (virt & pd_mask) >> pd_shift;
}

static int pd_allocate_children(struct vm_gk20a *vm,
				const struct gk20a_mmu_level *l,
				struct nvgpu_gmmu_pd *pd,
				struct nvgpu_gmmu_attrs *attrs)
{
	struct gk20a *g = gk20a_from_vm(vm);

	if (pd->entries)
		return 0;

	pd->num_entries = pd_entries(l, attrs);
	pd->entries = nvgpu_vzalloc(g, sizeof(struct nvgpu_gmmu_pd) *
				    pd->num_entries);
	if (!pd->entries)
		return -ENOMEM;

	return 0;
}

/*
 * This function programs the GMMU based on two ranges: a physical range and a
 * GPU virtual range. The virtual is mapped to the physical. Physical in this
 * case can mean either a real physical sysmem address or a IO virtual address
 * (for instance when a system has an IOMMU running).
 *
 * The rest of the parameters are for describing the actual mapping itself.
 *
 * This function recursively calls itself for handling PDEs. At the final level
 * a PTE handler is called. The phys and virt ranges are adjusted for each
 * recursion so that each invocation of this function need only worry about the
 * range it is passed.
 *
 * phys_addr will always point to a contiguous range - the discontiguous nature
 * of DMA buffers is taken care of at the layer above this.
 */
static int __set_pd_level(struct vm_gk20a *vm,
			  struct nvgpu_gmmu_pd *pd,
			  int lvl,
			  u64 phys_addr,
			  u64 virt_addr, u64 length,
			  struct nvgpu_gmmu_attrs *attrs)
{
	int err = 0;
	u64 pde_range;
	struct gk20a *g = gk20a_from_vm(vm);
	struct nvgpu_gmmu_pd *next_pd = NULL;
	const struct gk20a_mmu_level *l      = &vm->mmu_levels[lvl];
	const struct gk20a_mmu_level *next_l = &vm->mmu_levels[lvl + 1];

	/*
	 * 5 levels for Pascal+. For pre-pascal we only have 2. This puts
	 * offsets into the page table debugging code which makes it easier to
	 * see what level prints are from.
	 */
	static const char *__lvl_debug[] = {
		"",          /* L=0 */
		"  ",        /* L=1 */
		"    ",      /* L=2 */
		"      ",    /* L=3 */
		"        ",  /* L=4 */
	};

	pde_range = 1ULL << (u64)l->lo_bit[attrs->pgsz];

	__gmmu_dbg_v(g, attrs,
		     "L=%d   %sGPU virt %#-12llx +%#-9llx -> phys %#-12llx",
		     lvl,
		     __lvl_debug[lvl],
		     virt_addr,
		     length,
		     phys_addr);

	/*
	 * Iterate across the mapping in chunks the size of this level's PDE.
	 * For each of those chunks program our level's PDE and then, if there's
	 * a next level, program the next level's PDEs/PTEs.
	 */
	while (length) {
		u32 pd_idx = pd_index(l, virt_addr, attrs);
		u64 chunk_size;
		u64 target_addr;

		/*
		 * Truncate the pde_range when the virtual address does not
		 * start at a PDE boundary.
		 */
		chunk_size = min(length,
				 pde_range - (virt_addr & (pde_range - 1)));

		/*
		 * If the next level has an update_entry function then we know
		 * that _this_ level points to PDEs (not PTEs). Thus we need to
		 * have a bunch of children PDs.
		 */
		if (next_l->update_entry) {
			if (pd_allocate_children(vm, l, pd, attrs))
				return -ENOMEM;

			/*
			 * Get the next PD so that we know what to put in this
			 * current PD. If the next level is actually PTEs then
			 * we don't need this - we will just use the real
			 * physical target.
			 */
			next_pd = &pd->entries[pd_idx];

			/*
			 * Allocate the backing memory for next_pd.
			 */
			if (pd_allocate(vm, next_pd, next_l, attrs))
				return -ENOMEM;
		}

		/*
		 * This is the address we want to program into the actual PDE/
		 * PTE. When the next level is PDEs we need the target address
		 * to be the table of PDEs. When the next level is PTEs the
		 * target addr is the real physical address we are aiming for.
		 */
		target_addr = next_pd ?
			nvgpu_pde_phys_addr(g, next_pd) :
			g->ops.mm.gpu_phys_addr(g, attrs, phys_addr);

		l->update_entry(vm, l,
				pd, pd_idx,
				virt_addr,
				target_addr,
				attrs);

		if (next_l->update_entry) {
			err = map_gmmu_pages(g, next_pd);
			if (err) {
				nvgpu_err(g,
					  "couldn't map ptes for update as=%d",
					  vm_aspace_id(vm));
				return err;
			}

			err = __set_pd_level(vm, next_pd,
					     lvl + 1,
					     phys_addr,
					     virt_addr,
					     chunk_size,
					     attrs);
			unmap_gmmu_pages(g, next_pd);

			if (err)
				return err;
		}

		virt_addr += chunk_size;

		/*
		 * Only add to phys_addr if it's non-zero. A zero value implies
		 * we are unmapping as as a result we don't want to place
		 * non-zero phys addresses in the PTEs. A non-zero phys-addr
		 * would also confuse the lower level PTE programming code.
		 */
		if (phys_addr)
			phys_addr += chunk_size;
		length -= chunk_size;
	}

	__gmmu_dbg_v(g, attrs, "L=%d   %s%s", lvl, __lvl_debug[lvl], "ret!");

	return 0;
}

/*
 * VIDMEM version of the update_ptes logic.
 */
static int __nvgpu_gmmu_update_page_table_vidmem(struct vm_gk20a *vm,
						 struct sg_table *sgt,
						 u64 space_to_skip,
						 u64 virt_addr,
						 u64 length,
						 struct nvgpu_gmmu_attrs *attrs)
{
	struct nvgpu_page_alloc *alloc = NULL;
	struct page_alloc_chunk *chunk = NULL;
	u64 phys_addr, chunk_length;
	int err = 0;

	if (!sgt) {
		/*
		 * This is considered an unmap. Just pass in 0 as the physical
		 * address for the entire GPU range.
		 */
		err = __set_pd_level(vm, &vm->pdb,
				     0,
				     0,
				     virt_addr, length,
				     attrs);
		return err;
	}

	alloc = get_vidmem_page_alloc(sgt->sgl);

	/*
	 * Otherwise iterate across all the chunks in this allocation and
	 * map them.
	 */
	nvgpu_list_for_each_entry(chunk, &alloc->alloc_chunks,
				  page_alloc_chunk, list_entry) {
		if (space_to_skip &&
		    space_to_skip >= chunk->length) {
			space_to_skip -= chunk->length;
			continue;
		}

		phys_addr = chunk->base + space_to_skip;
		chunk_length = min(length, (chunk->length - space_to_skip));

		err = __set_pd_level(vm, &vm->pdb,
				     0,
				     phys_addr,
				     virt_addr, length,
				     attrs);
		if (err)
			break;

		/* Space has been skipped so zero this for future chunks. */
		space_to_skip = 0;

		/*
		 * Update the map pointer and the remaining length.
		 */
		virt_addr += chunk_length;
		length    -= chunk_length;

		if (length == 0)
			break;
	}

	return err;
}

static int __nvgpu_gmmu_update_page_table_sysmem(struct vm_gk20a *vm,
						 struct sg_table *sgt,
						 u64 space_to_skip,
						 u64 virt_addr,
						 u64 length,
						 struct nvgpu_gmmu_attrs *attrs)
{
	int err;
	struct scatterlist *sgl;
	struct gk20a *g = gk20a_from_vm(vm);

	if (!sgt) {
		/*
		 * This is considered an unmap. Just pass in 0 as the physical
		 * address for the entire GPU range.
		 */
		err = __set_pd_level(vm, &vm->pdb,
				     0,
				     0,
				     virt_addr, length,
				     attrs);
		return err;
	}

	/*
	 * At this point we have a Linux scatter-gather list pointing to some
	 * number of discontiguous chunks of memory. Iterate over that list and
	 * generate a GMMU map call for each chunk. There are two possibilities:
	 * either the IOMMU is enabled or not. When the IOMMU is enabled the
	 * mapping is simple since the "physical" address is actually a virtual
	 * IO address and will be contiguous. The no-IOMMU case is more
	 * complicated. We will have to iterate over the SGT and do a separate
	 * map for each chunk of the SGT.
	 */
	sgl = sgt->sgl;

	if (!g->mm.bypass_smmu) {
		u64 io_addr = g->ops.mm.get_iova_addr(g, sgl, 0);

		io_addr += space_to_skip;

		err = __set_pd_level(vm, &vm->pdb,
				     0,
				     io_addr,
				     virt_addr,
				     length,
				     attrs);

		return err;
	}

	/*
	 * Finally: last possible case: do the no-IOMMU mapping. In this case we
	 * really are mapping physical pages directly.
	 */
	while (sgl) {
		u64 phys_addr;
		u64 chunk_length;

		/*
		 * Cut out sgl ents for space_to_skip.
		 */
		if (space_to_skip && space_to_skip >= sgl->length) {
			space_to_skip -= sgl->length;
			sgl = sg_next(sgl);
			continue;
		}

		phys_addr = sg_phys(sgl) + space_to_skip;
		chunk_length = min(length, sgl->length - space_to_skip);

		err = __set_pd_level(vm, &vm->pdb,
				     0,
				     phys_addr,
				     virt_addr,
				     chunk_length,
				     attrs);
		if (err)
			return err;

		space_to_skip = 0;
		virt_addr += chunk_length;
		length    -= chunk_length;
		sgl        = sg_next(sgl);

		if (length == 0)
			break;
	}

	return 0;
}

/*
 * This is the true top level GMMU mapping logic. This breaks down the incoming
 * scatter gather table and does actual programming of GPU virtual address to
 * physical* address.
 *
 * The update of each level of the page tables is farmed out to chip specific
 * implementations. But the logic around that is generic to all chips. Every
 * chip has some number of PDE levels and then a PTE level.
 *
 * Each chunk of the incoming SGT is sent to the chip specific implementation
 * of page table update.
 *
 * [*] Note: the "physical" address may actually be an IO virtual address in the
 *     case of SMMU usage.
 */
static int __nvgpu_gmmu_update_page_table(struct vm_gk20a *vm,
					  struct sg_table *sgt,
					  u64 space_to_skip,
					  u64 virt_addr,
					  u64 length,
					  struct nvgpu_gmmu_attrs *attrs)
{
	struct gk20a *g = gk20a_from_vm(vm);
	u32 page_size;
	int err;

	/* note: here we need to map kernel to small, since the
	 * low-level mmu code assumes 0 is small and 1 is big pages */
	if (attrs->pgsz == gmmu_page_size_kernel)
		attrs->pgsz = gmmu_page_size_small;

	page_size = vm->gmmu_page_sizes[attrs->pgsz];

	if (space_to_skip & (page_size - 1))
		return -EINVAL;

	/*
	 * Update length to be aligned to the passed page size.
	 */
	length = nvgpu_align_map_length(vm, length, attrs);

	err = map_gmmu_pages(g, &vm->pdb);
	if (err) {
		nvgpu_err(g, "couldn't map ptes for update as=%d",
			  vm_aspace_id(vm));
		return err;
	}

	__gmmu_dbg(g, attrs,
		   "vm=%s "
		   "%-5s GPU virt %#-12llx +%#-9llx    phys %#-12llx "
		   "phys offset: %#-4llx;  pgsz: %3dkb perm=%-2s | "
		   "kind=%#02x APT=%-6s %c%c%c%c%c",
		   vm->name,
		   sgt ? "MAP" : "UNMAP",
		   virt_addr,
		   length,
		   sgt ? g->ops.mm.get_iova_addr(g, sgt->sgl, 0) : 0ULL,
		   space_to_skip,
		   page_size >> 10,
		   nvgpu_gmmu_perm_str(attrs->rw_flag),
		   attrs->kind_v,
		   nvgpu_aperture_str(attrs->aperture),
		   attrs->cacheable ? 'C' : 'c', /* C = cached, V = volatile. */
		   attrs->sparse    ? 'S' : '-',
		   attrs->priv      ? 'P' : '-',
		   attrs->coherent  ? 'c' : '-',
		   attrs->valid     ? 'V' : '-');

	/*
	 * Handle VIDMEM progamming. Currently uses a different scatter list
	 * format.
	 */
	if (attrs->aperture == APERTURE_VIDMEM)
		err = __nvgpu_gmmu_update_page_table_vidmem(vm,
							    sgt,
							    space_to_skip,
							    virt_addr,
							    length,
							    attrs);
	else
		err = __nvgpu_gmmu_update_page_table_sysmem(vm,
							    sgt,
							    space_to_skip,
							    virt_addr,
							    length,
							    attrs);

	unmap_gmmu_pages(g, &vm->pdb);
	mb();

	__gmmu_dbg(g, attrs, "%-5s Done!", sgt ? "MAP" : "UNMAP");

	return err;
}

/**
 * gk20a_locked_gmmu_map - Map a buffer into the GMMU
 *
 * This is for non-vGPU chips. It's part of the HAL at the moment but really
 * should not be. Chip specific stuff is handled at the PTE/PDE programming
 * layer. The rest of the logic is essentially generic for all chips.
 *
 * To call this function you must have locked the VM lock: vm->update_gmmu_lock.
 * However, note: this function is not called directly. It's used through the
 * mm.gmmu_lock() HAL. So before calling the mm.gmmu_lock() HAL make sure you
 * have the update_gmmu_lock aquired.
 */
u64 gk20a_locked_gmmu_map(struct vm_gk20a *vm,
			  u64 vaddr,
			  struct sg_table *sgt,
			  u64 buffer_offset,
			  u64 size,
			  int pgsz_idx,
			  u8 kind_v,
			  u32 ctag_offset,
			  u32 flags,
			  int rw_flag,
			  bool clear_ctags,
			  bool sparse,
			  bool priv,
			  struct vm_gk20a_mapping_batch *batch,
			  enum nvgpu_aperture aperture)
{
	struct gk20a *g = gk20a_from_vm(vm);
	int err = 0;
	bool allocated = false;
	int ctag_granularity = g->ops.fb.compression_page_size(g);
	struct nvgpu_gmmu_attrs attrs = {
		.pgsz      = pgsz_idx,
		.kind_v    = kind_v,
		.ctag      = (u64)ctag_offset * (u64)ctag_granularity,
		.cacheable = flags & NVGPU_MAP_BUFFER_FLAGS_CACHEABLE_TRUE,
		.rw_flag   = rw_flag,
		.sparse    = sparse,
		.priv      = priv,
		.coherent  = flags & NVGPU_AS_MAP_BUFFER_FLAGS_IO_COHERENT,
		.valid     = !(flags & NVGPU_AS_MAP_BUFFER_FLAGS_UNMAPPED_PTE),
		.aperture  = aperture
	};

#ifdef CONFIG_TEGRA_19x_GPU
	nvgpu_gmmu_add_t19x_attrs(&attrs, flags);
#endif

	/*
	 * Only allocate a new GPU VA range if we haven't already been passed a
	 * GPU VA range. This facilitates fixed mappings.
	 */
	if (!vaddr) {
		vaddr = __nvgpu_vm_alloc_va(vm, size, pgsz_idx);
		if (!vaddr) {
			nvgpu_err(g, "failed to allocate va space");
			err = -ENOMEM;
			goto fail_alloc;
		}
		allocated = true;
	}

	err = __nvgpu_gmmu_update_page_table(vm, sgt, buffer_offset,
					     vaddr, size, &attrs);
	if (err) {
		nvgpu_err(g, "failed to update ptes on map");
		goto fail_validate;
	}

	if (!batch)
		g->ops.fb.tlb_invalidate(g, vm->pdb.mem);
	else
		batch->need_tlb_invalidate = true;

	return vaddr;
fail_validate:
	if (allocated)
		__nvgpu_vm_free_va(vm, vaddr, pgsz_idx);
fail_alloc:
	nvgpu_err(g, "%s: failed with err=%d", __func__, err);
	return 0;
}

void gk20a_locked_gmmu_unmap(struct vm_gk20a *vm,
			     u64 vaddr,
			     u64 size,
			     int pgsz_idx,
			     bool va_allocated,
			     int rw_flag,
			     bool sparse,
			     struct vm_gk20a_mapping_batch *batch)
{
	int err = 0;
	struct gk20a *g = gk20a_from_vm(vm);
	struct nvgpu_gmmu_attrs attrs = {
		.pgsz      = pgsz_idx,
		.kind_v    = 0,
		.ctag      = 0,
		.cacheable = 0,
		.rw_flag   = rw_flag,
		.sparse    = sparse,
		.priv      = 0,
		.coherent  = 0,
		.valid     = 0,
		.aperture  = APERTURE_INVALID,
	};

	if (va_allocated) {
		err = __nvgpu_vm_free_va(vm, vaddr, pgsz_idx);
		if (err) {
			nvgpu_err(g, "failed to free va");
			return;
		}
	}

	/* unmap here needs to know the page size we assigned at mapping */
	err = __nvgpu_gmmu_update_page_table(vm, NULL, 0,
					     vaddr, size, &attrs);
	if (err)
		nvgpu_err(g, "failed to update gmmu ptes on unmap");

	if (!batch) {
		gk20a_mm_l2_flush(g, true);
		g->ops.fb.tlb_invalidate(g, vm->pdb.mem);
	} else {
		if (!batch->gpu_l2_flushed) {
			gk20a_mm_l2_flush(g, true);
			batch->gpu_l2_flushed = true;
		}
		batch->need_tlb_invalidate = true;
	}
}
