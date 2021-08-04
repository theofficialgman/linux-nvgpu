/*
 * GA10B CBC
 *
 * Copyright (c) 2020-2021, NVIDIA CORPORATION.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <nvgpu/gk20a.h>
#include <nvgpu/io.h>
#include <nvgpu/sizes.h>
#include <nvgpu/ltc.h>
#include <nvgpu/cbc.h>
#include <nvgpu/comptags.h>

#include "cbc_ga10b.h"

#include <nvgpu/hw/ga10b/hw_ltc_ga10b.h>

u64 ga10b_cbc_get_base_divisor(struct gk20a *g)
{
	/*
	 * For Tegra, the addressing works differently. Unlike DGPU, all
	 * partitions talk to the same memory.
	 */
	u64 ltc_count = 1ULL;

	return ltc_count << ltc_ltcs_ltss_cbc_base_alignment_shift_v();
}

int ga10b_cbc_alloc_comptags(struct gk20a *g, struct nvgpu_cbc *cbc)
{
	/*
	 * - Compbit backing store is a memory buffer to store compressed data
	 * corresponding to total compressible memory.
	 * - In GA10B, 1 ROP tile = 256B data is compressed to 1B compression
	 * bits. i.e. 1 GOB = 512B data is compressed to 2B compbits.
	 * - A comptagline is a collection of compbits corresponding to a
	 * compressible page size. In GA10B, compressible page size is 64KB.
	 *
	 * - GA10B has 2 LTCs with 4 slices each. A 64KB page is distributed
	 *   into 8 slices having 8KB (16 GOBs) data each.
	 * - Thus, each comptagline per slice contains compression status bits
	 * corresponding to 16 GOBs.
	 */
	u32 compbit_backing_size;

	/* max memory size (MB) to cover */
	u32 max_size = g->max_comptag_mem;

	/*
	 * one tag line covers 64KB
	 * So, number of comptag lines = (max_size * SZ_1M) / SZ_64K
	 */
	u32 max_comptag_lines = max_size << 4U;

	u32 hw_max_comptag_lines =
		ltc_ltcs_ltss_cbc_ctrl3_clear_upper_bound_init_v();

	u32 gobs_per_comptagline_per_slice =
		ltc_ltcs_ltss_cbc_param2_gobs_per_comptagline_per_slice_v(
				nvgpu_readl(g, ltc_ltcs_ltss_cbc_param2_r()));

	u32 compstatus_per_gob = 2U;

	u32 comptags_size =
		ltc_ltcs_ltss_cbc_param_bytes_per_comptagline_per_slice_v(
			nvgpu_readl(g, ltc_ltcs_ltss_cbc_param_r()));

	/*
	 * For Tegra, the addressing works differently. Unlike DGPU, all
	 * partitions talk to the same memory.
	 */
	u32 ltc_count = 1U;

	/* check if vidmem is present */
	bool alloc_vidmem = g->ops.fb.get_vidmem_size != NULL ? true : false;
	int err;

	nvgpu_log_fn(g, " ");

	if (max_comptag_lines == 0U) {
		return 0;
	}

	/* Already initialized */
	if (cbc->max_comptag_lines != 0U) {
		return 0;
	}

	if (max_comptag_lines > hw_max_comptag_lines) {
		max_comptag_lines = hw_max_comptag_lines;
	}

	/* Memory required for comptag lines in all slices of all ltcs */
	compbit_backing_size =  nvgpu_safe_mult_u32(
		nvgpu_safe_mult_u32(max_comptag_lines,
			nvgpu_ltc_get_slices_per_ltc(g)), ltc_count);

	/* Total memory required for compstatus */
	compbit_backing_size =  nvgpu_safe_mult_u32(
		nvgpu_safe_mult_u32(compbit_backing_size,
			gobs_per_comptagline_per_slice), compstatus_per_gob);

	/* aligned to 2KB * ltc_count */
	compbit_backing_size += nvgpu_safe_cast_u64_to_u32(
		g->ops.cbc.get_base_divisor(g));

	/* must be a multiple of 64KB */
	compbit_backing_size = round_up(compbit_backing_size, SZ_64K);

	err = nvgpu_cbc_alloc(g, compbit_backing_size, alloc_vidmem);
	if (err != 0) {
		return err;
	}

	err = gk20a_comptag_allocator_init(g, &cbc->comp_tags,
						max_comptag_lines);
	if (err != 0) {
		return err;
	}

	cbc->max_comptag_lines = max_comptag_lines;
	cbc->comptags_per_cacheline =
		nvgpu_ltc_get_cacheline_size(g) / comptags_size;
	cbc->gobs_per_comptagline_per_slice = gobs_per_comptagline_per_slice;
	cbc->compbit_backing_size = compbit_backing_size;

	nvgpu_log(g, gpu_dbg_pte, "supported LTCs: 0x%x",
		nvgpu_ltc_get_ltc_count(g));
	nvgpu_log(g, gpu_dbg_pte, "ltc_count used for calculations: 0x%x",
		ltc_count);
	nvgpu_log(g, gpu_dbg_pte, "compbit backing store size : 0x%x",
		compbit_backing_size);
	nvgpu_log(g, gpu_dbg_pte, "max comptag lines: %d",
		max_comptag_lines);
	nvgpu_log(g, gpu_dbg_pte, "gobs_per_comptagline_per_slice: %d",
		cbc->gobs_per_comptagline_per_slice);

	return 0;
}

void ga10b_cbc_init(struct gk20a *g, struct nvgpu_cbc *cbc)
{
	g->ops.fb.cbc_configure(g, cbc);
	g->ops.cbc.ctrl(g, nvgpu_cbc_op_clear, 0U, cbc->max_comptag_lines - 1U);
}
