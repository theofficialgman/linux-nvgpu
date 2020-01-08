/*
 * Copyright (c) 2018-2019, NVIDIA CORPORATION.  All rights reserved.
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

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <unit/io.h>
#include <unit/unit.h>

#include <nvgpu/channel.h>
#include <nvgpu/error_notifier.h>
#include <nvgpu/tsg.h>
#include <nvgpu/gk20a.h>
#include <nvgpu/fifo/userd.h>
#include <nvgpu/runlist.h>
#include <nvgpu/fuse.h>
#include <nvgpu/dma.h>
#include <nvgpu/gr/ctx.h>

#include "common/gr/ctx_priv.h"
#include <nvgpu/posix/posix-fault-injection.h>
#include <nvgpu/posix/posix-channel.h>

#include "hal/fifo/tsg_gk20a.h"

#include "hal/init/hal_gv11b.h"

#include "../nvgpu-fifo-common.h"
#include "nvgpu-tsg.h"

#ifdef TSG_UNIT_DEBUG
#undef unit_verbose
#define unit_verbose	unit_info
#else
#define unit_verbose(unit, msg, ...) \
	do { \
		if (0) { \
			unit_info(unit, msg, ##__VA_ARGS__); \
		} \
	} while (0)
#endif

#define assert(cond)	unit_assert(cond, goto done)

struct tsg_unit_ctx {
	u32 branches;
};

static struct tsg_unit_ctx unit_ctx;

#define MAX_STUB	4

struct stub_ctx {
	const char *name;
	u32 count;
	u32 chid;
	u32 tsgid;
};

struct stub_ctx stub[MAX_STUB];

static void subtest_setup(u32 branches)
{
	u32 i;

	unit_ctx.branches = branches;

	memset(stub, 0, sizeof(stub));
	for (i = 0; i < MAX_STUB; i++) {
		stub[i].name = "";
		stub[i].count = 0;
		stub[i].chid = NVGPU_INVALID_CHANNEL_ID;
		stub[i].tsgid = NVGPU_INVALID_TSG_ID;
	}
}

#define pruned	test_fifo_subtest_pruned
#define branches_str test_fifo_flags_str

#define F_TSG_OPEN_ACQUIRE_CH_FAIL		BIT(0)
#define F_TSG_OPEN_SM_FAIL			BIT(1)
#define F_TSG_OPEN_ALLOC_SM_FAIL		BIT(2)
#define F_TSG_OPEN_ALLOC_SM_KZALLOC_FAIL	BIT(3)
#define F_TSG_OPEN_ALLOC_GR_FAIL		BIT(4)
#define F_TSG_OPEN_NO_INIT_BUF			BIT(5)
#define F_TSG_OPEN_INIT_BUF_FAIL		BIT(6)
#define F_TSG_OPEN_NO_OPEN_HAL			BIT(7)
#define F_TSG_OPEN_OPEN_HAL_FAIL		BIT(8)
#define F_TSG_OPEN_LAST				BIT(9)

static int stub_tsg_init_eng_method_buffers(struct gk20a *g,
	struct nvgpu_tsg *tsg)
{
	if (unit_ctx.branches & F_TSG_OPEN_INIT_BUF_FAIL) {
		return -ENOMEM;
	}
	return 0;
}


static int stub_tsg_open(struct nvgpu_tsg *tsg)
{
	if (unit_ctx.branches & F_TSG_OPEN_OPEN_HAL_FAIL) {
		return -EINVAL;
	}
	return 0;
}

static u32 stub_gr_init_get_no_of_sm_0(struct gk20a *g)
{
	return 0;
}

int test_tsg_open(struct unit_module *m,
		struct gk20a *g, void *args)
{
	struct nvgpu_fifo *f = &g->fifo;
	struct gpu_ops gops = g->ops;
	u32 num_channels = f->num_channels;
	struct nvgpu_tsg *tsg = NULL;
	struct nvgpu_tsg *next_tsg = NULL;
	struct nvgpu_posix_fault_inj *kmem_fi;
	u32 branches = 0U;
	int ret = UNIT_FAIL;
	u32 fail = F_TSG_OPEN_ACQUIRE_CH_FAIL |
		   F_TSG_OPEN_SM_FAIL |
		   F_TSG_OPEN_ALLOC_SM_FAIL |
		   F_TSG_OPEN_ALLOC_SM_KZALLOC_FAIL |
		   F_TSG_OPEN_ALLOC_GR_FAIL |
		   F_TSG_OPEN_INIT_BUF_FAIL |
		   F_TSG_OPEN_OPEN_HAL_FAIL;
	u32 prune = fail;
	u32 tsgid;
	const char *labels[] = {
		"acquire_ch_fail",
		"sm_fail",
		"alloc_sm_fail",
		"alloc_sm_kzalloc_fail",
		"alloc_gr_fail",
		"no_init_buf",
		"init_buf_fail",
		"no_open_hal",
		"open_hal_fail"
	};

	kmem_fi = nvgpu_kmem_get_fault_injection();

	for (branches = 0U; branches < F_TSG_OPEN_LAST; branches++) {

		if (pruned(branches, prune)) {
			unit_verbose(m, "%s branches=%s (pruned)\n", __func__,
				branches_str(branches, labels));
			continue;
		}
		unit_verbose(m, "%s branches=%s\n", __func__,
			branches_str(branches, labels));
		subtest_setup(branches);

		/* find next tsg (if acquire succeeds) */
		next_tsg = NULL;
		for (tsgid = 0U; tsgid < f->num_channels; tsgid++) {
			if (!f->tsg[tsgid].in_use) {
				next_tsg = &f->tsg[tsgid];
				break;
			}
		}
		assert(next_tsg != NULL);

		f->num_channels =
			branches & F_TSG_OPEN_ACQUIRE_CH_FAIL ?
			0U : num_channels;

		g->ops.gr.init.get_no_of_sm =
			branches & F_TSG_OPEN_SM_FAIL ?
			stub_gr_init_get_no_of_sm_0 :
			gops.gr.init.get_no_of_sm;

		next_tsg->sm_error_states =
			branches & F_TSG_OPEN_ALLOC_SM_FAIL ?
			(void*)1 : NULL;

		g->ops.tsg.init_eng_method_buffers =
			branches & F_TSG_OPEN_NO_INIT_BUF ?
			NULL : stub_tsg_init_eng_method_buffers;

		g->ops.tsg.open =
			branches & F_TSG_OPEN_NO_OPEN_HAL ?
			NULL : stub_tsg_open;

		nvgpu_posix_enable_fault_injection(kmem_fi, false, 0);

		if (branches & F_TSG_OPEN_ALLOC_SM_KZALLOC_FAIL) {
			nvgpu_posix_enable_fault_injection(kmem_fi, true, 0);
		}

		if (branches & F_TSG_OPEN_ALLOC_GR_FAIL) {
			nvgpu_posix_enable_fault_injection(kmem_fi, true, 1);
		}

		tsg = nvgpu_tsg_open(g, getpid());

		f->tsg[tsgid].sm_error_states = NULL;

		if (branches & fail) {
			f->num_channels = num_channels;
			assert(tsg == NULL);
		} else {
			assert(tsg != NULL);
			nvgpu_ref_put(&tsg->refcount, nvgpu_tsg_release);
			tsg = NULL;
		}
	}
	ret = UNIT_SUCCESS;

done:
	if (ret != UNIT_SUCCESS) {
		unit_err(m, "%s branches=%s\n", __func__,
			branches_str(branches, labels));
	}
	nvgpu_posix_enable_fault_injection(kmem_fi, false, 0);
	if (tsg != NULL) {
		nvgpu_ref_put(&tsg->refcount, nvgpu_tsg_release);
	}
	g->ops = gops;
	f->num_channels = num_channels;
	return ret;
}

#define F_TSG_BIND_CHANNEL_CH_BOUND		BIT(0)
#define F_TSG_BIND_CHANNEL_RL_MISMATCH		BIT(1)
#define F_TSG_BIND_CHANNEL_ACTIVE		BIT(2)
#define F_TSG_BIND_CHANNEL_BIND_HAL		BIT(3)
#define F_TSG_BIND_CHANNEL_BIND_HAL_ERR		BIT(4)
#define F_TSG_BIND_CHANNEL_ENG_METHOD_BUFFER	BIT(5)
#define F_TSG_BIND_CHANNEL_LAST			BIT(6)

static const char *f_tsg_bind[] = {
	"ch_bound",
	"rl_mismatch",
	"active",
	"bind_hal",
	"eng_method_buffer",
};

static int stub_tsg_bind_channel(struct nvgpu_tsg *tsg,
			struct nvgpu_channel *ch)
{
	if (unit_ctx.branches & F_TSG_BIND_CHANNEL_BIND_HAL_ERR) {
		return -EINVAL;
	}

	return 0;
}

int test_tsg_bind_channel(struct unit_module *m,
		struct gk20a *g, void *args)
{
	struct nvgpu_fifo *f = &g->fifo;
	struct gpu_ops gops = g->ops;
	struct nvgpu_tsg *tsg = NULL;
	struct nvgpu_tsg tsg_save;
	struct nvgpu_channel *chA = NULL;
	struct nvgpu_channel *chB = NULL;
	struct nvgpu_channel *ch = NULL;
	struct nvgpu_runlist_info *runlist = NULL;
	u32 branches = 0U;
	int ret = UNIT_FAIL;
	int err;
	u32 fail = F_TSG_BIND_CHANNEL_CH_BOUND |
		    F_TSG_BIND_CHANNEL_RL_MISMATCH |
		    F_TSG_BIND_CHANNEL_ACTIVE |
		    F_TSG_BIND_CHANNEL_BIND_HAL_ERR;
	u32 prune = fail;

	tsg = nvgpu_tsg_open(g, getpid());
	assert(tsg != NULL);

	chA = nvgpu_channel_open_new(g, ~0U, false, getpid(), getpid());
	assert(chA != NULL);

	chB = nvgpu_channel_open_new(g, ~0U, false, getpid(), getpid());
	assert(chB != NULL);

	err = nvgpu_tsg_bind_channel(tsg, chA);
	assert(err == 0);

	tsg_save = *tsg;

	for (branches = 0U; branches < F_TSG_BIND_CHANNEL_LAST; branches++) {

		if (pruned(branches, prune)) {
			unit_verbose(m, "%s branches=%s (pruned)\n", __func__,
				branches_str(branches, f_tsg_bind));
			continue;
		}
		subtest_setup(branches);
		ch = chB;

		/* ch already bound */
		if (branches & F_TSG_BIND_CHANNEL_CH_BOUND) {
			ch = chA;
		}

		/* runlist id mismatch */
		tsg->runlist_id =
			branches & F_TSG_BIND_CHANNEL_RL_MISMATCH ?
			ch->runlist_id + 1 : tsg_save.runlist_id;

		/* ch already already active */
		runlist = &f->active_runlist_info[tsg->runlist_id];
		if (branches & F_TSG_BIND_CHANNEL_ACTIVE) {
			nvgpu_set_bit(ch->chid, runlist->active_channels);
		} else {
			nvgpu_clear_bit(ch->chid, runlist->active_channels);
		}

		if ((branches & F_TSG_BIND_CHANNEL_BIND_HAL) ||
		    (branches & F_TSG_BIND_CHANNEL_BIND_HAL_ERR)) {
			g->ops.tsg.bind_channel = stub_tsg_bind_channel;
		} else {
			g->ops.tsg.bind_channel = NULL;
		}

		g->ops.tsg.bind_channel_eng_method_buffers =
			branches & F_TSG_BIND_CHANNEL_ENG_METHOD_BUFFER ?
			gops.tsg.bind_channel_eng_method_buffers : NULL;

		unit_verbose(m, "%s branches=%s\n", __func__,
			branches_str(branches, f_tsg_bind));

		err = nvgpu_tsg_bind_channel(tsg, ch);

		if (branches & fail) {
			assert(err != 0);
		} else {
			assert(err == 0);
			assert(!nvgpu_list_empty(&tsg->ch_list));

			err = nvgpu_tsg_unbind_channel(tsg, ch);
			assert(err == 0);
			assert(ch->tsgid == NVGPU_INVALID_TSG_ID);
		}
	}

	ret = UNIT_SUCCESS;

done:
	if (ret != UNIT_SUCCESS) {
		unit_err(m, "%s branches=%s\n", __func__,
			branches_str(branches, f_tsg_bind));
	}
	if (chA != NULL) {
		nvgpu_channel_close(chA);
	}
	if (chB != NULL) {
		nvgpu_channel_close(chB);
	}
	if (tsg != NULL) {
		nvgpu_ref_put(&tsg->refcount, nvgpu_tsg_release);
	}
	g->ops = gops;
	return ret;
}

#define F_TSG_UNBIND_CHANNEL_ABORT_RUNLIST_UPDATE_FAIL	BIT(0)
#define F_TSG_UNBIND_CHANNEL_UNSERVICEABLE		BIT(1)
#define F_TSG_UNBIND_CHANNEL_PREEMPT_TSG_FAIL		BIT(2)
#define F_TSG_UNBIND_CHANNEL_CHECK_HW_STATE		BIT(3)
#define F_TSG_UNBIND_CHANNEL_CHECK_HW_STATE_FAIL	BIT(4)
#define F_TSG_UNBIND_CHANNEL_RUNLIST_UPDATE_FAIL	BIT(5)
#define F_TSG_UNBIND_CHANNEL_UNBIND_HAL			BIT(6)
#define F_TSG_UNBIND_CHANNEL_UNBIND_HAL_FAIL		BIT(7)
#define F_TSG_UNBIND_CHANNEL_ABORT_CLEAN_UP_NULL	BIT(8)
#define F_TSG_UNBIND_CHANNEL_LAST			BIT(9)

static int stub_fifo_preempt_tsg_EINVAL(
		struct gk20a *g, struct nvgpu_tsg *tsg)
{
	return -EINVAL;
}

static int stub_tsg_unbind_channel_check_hw_state(
		struct nvgpu_tsg *tsg, struct nvgpu_channel *ch)
{
	if (unit_ctx.branches & F_TSG_UNBIND_CHANNEL_CHECK_HW_STATE_FAIL) {
		return -EINVAL;
	}

	return 0;
}

static int stub_tsg_unbind_channel(struct nvgpu_tsg *tsg,
		struct nvgpu_channel *ch)
{
	if (unit_ctx.branches & F_TSG_UNBIND_CHANNEL_UNBIND_HAL_FAIL) {
		return -EINVAL;
	}

	return 0;
}

static int stub_runlist_update_for_channel_EINVAL(
		struct gk20a *g, u32 runlist_id,
		struct nvgpu_channel *ch, bool add, bool wait_for_finish)
{
	stub[0].count++;
	if (stub[0].count == 1 && (unit_ctx.branches &
			F_TSG_UNBIND_CHANNEL_RUNLIST_UPDATE_FAIL)) {
		return -EINVAL;
	}
	if (stub[0].count == 2 && (unit_ctx.branches &
			F_TSG_UNBIND_CHANNEL_ABORT_RUNLIST_UPDATE_FAIL)) {
		return -EINVAL;
	}
	return 0;
}

int test_tsg_unbind_channel(struct unit_module *m,
		struct gk20a *g, void *args)
{
	struct gpu_ops gops = g->ops;
	struct nvgpu_tsg *tsg = NULL;
	struct nvgpu_channel *chA = NULL;
	struct nvgpu_channel *chB = NULL;
	u32 branches = 0U;
	int ret = UNIT_FAIL;
	int err;
	const char *labels[] = {
		"abort_runlist_update_fail",
		"unserviceable",
		"preempt_tsg_fail",
		"check_hw_state",
		"check_hw_state_fail",
		"runlist_update_fail",
		"unbind_hal",
		"unbind_hal_fail",
		"abort_cleanup_null"
	};
	u32 fail =
		F_TSG_UNBIND_CHANNEL_PREEMPT_TSG_FAIL |
		F_TSG_UNBIND_CHANNEL_CHECK_HW_STATE_FAIL |
		F_TSG_UNBIND_CHANNEL_RUNLIST_UPDATE_FAIL |
		F_TSG_UNBIND_CHANNEL_UNBIND_HAL_FAIL;

	/*
	 * do not prune F_TSG_UNBIND_CHANNEL_UNBIND_HAL_FAIL, to
	 * exercise following abort path.
	 */
	u32 prune =
		F_TSG_UNBIND_CHANNEL_PREEMPT_TSG_FAIL |
		F_TSG_UNBIND_CHANNEL_CHECK_HW_STATE_FAIL |
		F_TSG_UNBIND_CHANNEL_RUNLIST_UPDATE_FAIL;

	for (branches = 0U; branches < F_TSG_UNBIND_CHANNEL_LAST; branches++) {
		if (pruned(branches, prune)) {
			unit_verbose(m, "%s branches=%s (pruned)\n", __func__,
				branches_str(branches, labels));
			continue;
		}

		subtest_setup(branches);
		unit_verbose(m, "%s branches=%s\n", __func__,
			branches_str(branches, labels));

		/*
		 * tsg unbind tears down TSG in case of failure:
		 * we need to create tsg + bind channel for each test
		 */
		tsg = nvgpu_tsg_open(g, getpid());
		assert(tsg != NULL);

		chA = nvgpu_channel_open_new(g, ~0U, false, getpid(), getpid());
		assert(chA != NULL);

		chB = nvgpu_channel_open_new(g, ~0U, false, getpid(), getpid());
		assert(chB != NULL);

		err = nvgpu_tsg_bind_channel(tsg, chA);
		assert(err == 0);

		err = nvgpu_tsg_bind_channel(tsg, chB);
		assert(err == 0);

		chA->unserviceable =
			branches & F_TSG_UNBIND_CHANNEL_UNSERVICEABLE ?
			true : false;

		g->ops.fifo.preempt_tsg =
			branches & F_TSG_UNBIND_CHANNEL_PREEMPT_TSG_FAIL ?
			stub_fifo_preempt_tsg_EINVAL :
			gops.fifo.preempt_tsg;

		if ((branches & F_TSG_UNBIND_CHANNEL_CHECK_HW_STATE) ||
		    (branches & F_TSG_UNBIND_CHANNEL_CHECK_HW_STATE_FAIL)) {
			g->ops.tsg.unbind_channel_check_hw_state =
				stub_tsg_unbind_channel_check_hw_state;
		} else {
			g->ops.tsg.unbind_channel_check_hw_state = NULL;
		}

		g->ops.runlist.update_for_channel =
			branches & F_TSG_UNBIND_CHANNEL_RUNLIST_UPDATE_FAIL ?
			stub_runlist_update_for_channel_EINVAL :
			gops.runlist.update_for_channel;

		if (branches & F_TSG_UNBIND_CHANNEL_RUNLIST_UPDATE_FAIL ||
		    branches & F_TSG_UNBIND_CHANNEL_ABORT_RUNLIST_UPDATE_FAIL) {
			g->ops.runlist.update_for_channel =
				stub_runlist_update_for_channel_EINVAL;
		}

		if ((branches & F_TSG_UNBIND_CHANNEL_UNBIND_HAL) ||
		    (branches & F_TSG_UNBIND_CHANNEL_UNBIND_HAL_FAIL)) {
			g->ops.tsg.unbind_channel = stub_tsg_unbind_channel;
		} else {
			g->ops.tsg.unbind_channel = NULL;
		}

		g->ops.channel.abort_clean_up =
			branches & F_TSG_UNBIND_CHANNEL_ABORT_CLEAN_UP_NULL ?
			NULL : gops.channel.abort_clean_up;

		err = nvgpu_tsg_unbind_channel(tsg, chA);

		if (branches & fail) {
			/* check that TSG has been torn down */
			assert(err != 0);
			assert(chA->unserviceable);
			assert(chB->unserviceable);
			assert(chA->tsgid == NVGPU_INVALID_TSG_ID);
		} else {

			if (branches & F_TSG_UNBIND_CHANNEL_ABORT_CLEAN_UP_NULL) {
				gops.channel.abort_clean_up(chA);
			}

			assert(chA->tsgid == NVGPU_INVALID_TSG_ID);
			assert(nvgpu_list_empty(&chA->ch_entry));
			/* check that TSG has not been torn down */
			assert(!chB->unserviceable);
			assert(!nvgpu_list_empty(&chB->ch_entry));
			assert(!nvgpu_list_empty(&tsg->ch_list));
		}

		nvgpu_channel_close(chA);
		nvgpu_channel_close(chB);
		nvgpu_ref_put(&tsg->refcount, nvgpu_tsg_release);
		chA = NULL;
		chB = NULL;
		tsg = NULL;
	}

	ret = UNIT_SUCCESS;

done:
	if (ret == UNIT_FAIL) {
		unit_err(m, "%s branches=%s\n", __func__,
			branches_str(branches, labels));
	}
	if (chA != NULL) {
		nvgpu_channel_close(chA);
	}
	if (chB != NULL) {
		nvgpu_channel_close(chB);
	}
	if (tsg != NULL) {
		nvgpu_ref_put(&tsg->refcount, nvgpu_tsg_release);
	}
	g->ops = gops;
	return ret;
}

#define F_TSG_RELEASE_NO_RELEASE_HAL	BIT(0)
#define F_TSG_RELEASE_GR_CTX		BIT(1)
#define F_TSG_RELEASE_MEM		BIT(2)
#define F_TSG_RELEASE_VM		BIT(3)
#define F_TSG_RELEASE_ENG_BUFS		BIT(4)
#define F_TSG_RELEASE_SM_ERR_STATES	BIT(5)
#define F_TSG_RELEASE_LAST		BIT(6)


static void stub_tsg_release(struct nvgpu_tsg *tsg)
{
}

static void stub_tsg_deinit_eng_method_buffers(struct gk20a *g,
		struct nvgpu_tsg *tsg)
{
	stub[0].name = __func__;
	stub[0].tsgid = tsg->tsgid;
}

static void stub_gr_setup_free_gr_ctx(struct gk20a *g,
		struct vm_gk20a *vm, struct nvgpu_gr_ctx *gr_ctx)
{
	stub[1].name = __func__;
	stub[1].count++;
}


int test_tsg_release(struct unit_module *m,
		struct gk20a *g, void *args)
{
	struct nvgpu_fifo *f = &g->fifo;
	struct gpu_ops gops = g->ops;
	struct nvgpu_tsg *tsg = NULL;
	struct vm_gk20a vm;
	u32 branches = 0U;
	int ret = UNIT_FAIL;
	struct nvgpu_mem mem;
	u32 free_gr_ctx_mask =
		F_TSG_RELEASE_GR_CTX|F_TSG_RELEASE_MEM|F_TSG_RELEASE_VM;
	const char *labels[] = {
		"no_release_hal",
		"gr_ctx",
		"mem",
		"vm",
		"eng_bufs",
		"sm_err_states"
	};

	for (branches = 0U; branches < F_TSG_RELEASE_LAST; branches++) {

		if (!(branches & F_TSG_RELEASE_GR_CTX) &&
				(branches & F_TSG_RELEASE_MEM)) {
			unit_verbose(m, "%s branches=%s (pruned)\n", __func__,
				branches_str(branches, labels));
			continue;
		}
		subtest_setup(branches);
		unit_verbose(m, "%s branches=%s\n", __func__,
			branches_str(branches, labels));

		tsg = nvgpu_tsg_open(g, getpid());
		assert(tsg != NULL);
		assert(tsg->gr_ctx != NULL);
		assert(tsg->gr_ctx->mem.aperture == APERTURE_INVALID);

		g->ops.tsg.release =
			branches & F_TSG_RELEASE_NO_RELEASE_HAL ?
			NULL : stub_tsg_release;

		if (!(branches & F_TSG_RELEASE_GR_CTX)) {
			nvgpu_free_gr_ctx_struct(g, tsg->gr_ctx);
			tsg->gr_ctx = NULL;
		}

		if (branches & F_TSG_RELEASE_MEM) {
			nvgpu_dma_alloc(g, PAGE_SIZE, &mem);
			tsg->gr_ctx->mem = mem;
		}

		if (branches & F_TSG_RELEASE_VM) {
			tsg->vm = &vm;
			/* prevent nvgpu_vm_remove */
			nvgpu_ref_init(&vm.ref);
			nvgpu_ref_get(&vm.ref);
		} else {
			tsg->vm = NULL;
		}

		if ((branches & free_gr_ctx_mask) == free_gr_ctx_mask) {
			g->ops.gr.setup.free_gr_ctx =
				stub_gr_setup_free_gr_ctx;
		}

		g->ops.tsg.deinit_eng_method_buffers =
			branches & F_TSG_RELEASE_ENG_BUFS ?
			stub_tsg_deinit_eng_method_buffers : NULL;

		if (branches & F_TSG_RELEASE_SM_ERR_STATES) {
			assert(tsg->sm_error_states != NULL);
		} else {
			nvgpu_kfree(g, tsg->sm_error_states);
			tsg->sm_error_states = NULL;
		}

		nvgpu_ref_put(&tsg->refcount, nvgpu_tsg_release);

		if ((branches & free_gr_ctx_mask) == free_gr_ctx_mask) {
			assert(tsg->gr_ctx == NULL);
		} else {
			g->ops.gr.setup.free_gr_ctx =
				gops.gr.setup.free_gr_ctx;

			if (branches & F_TSG_RELEASE_MEM) {
				nvgpu_dma_free(g, &mem);
			}

			if (tsg->gr_ctx != NULL) {
				nvgpu_free_gr_ctx_struct(g, tsg->gr_ctx);
				tsg->gr_ctx = NULL;
			}
			assert(stub[1].count == 0);
		}

		if (branches & F_TSG_RELEASE_ENG_BUFS) {
			assert(stub[0].tsgid == tsg->tsgid);
		}

		assert(!f->tsg[tsg->tsgid].in_use);
		assert(tsg->gr_ctx == NULL);
		assert(tsg->vm == NULL);
		assert(tsg->sm_error_states == NULL);
	}
	ret = UNIT_SUCCESS;

done:
	if (ret != UNIT_SUCCESS) {
		unit_err(m, "%s branches=%s\n", __func__,
			branches_str(branches, labels));
	}
	g->ops = gops;
	return ret;
}

#define F_TSG_UNBIND_CHANNEL_CHECK_HW_NEXT		BIT(0)
#define F_TSG_UNBIND_CHANNEL_CHECK_HW_CTX_RELOAD	BIT(1)
#define F_TSG_UNBIND_CHANNEL_CHECK_HW_ENG_FAULTED	BIT(2)
#define F_TSG_UNBIND_CHANNEL_CHECK_HW_LAST		BIT(3)

static const char *f_tsg_unbind_channel_check_hw[] = {
	"next",
	"ctx_reload",
	"eng_faulted",
};

static void stub_channel_read_state_NEXT(struct gk20a *g,
		struct nvgpu_channel *ch, struct nvgpu_channel_hw_state *state)
{
	state->next = true;
}

int test_tsg_unbind_channel_check_hw_state(struct unit_module *m,
		struct gk20a *g, void *args)
{
	struct gpu_ops gops = g->ops;
	struct nvgpu_channel *ch = NULL;
	struct nvgpu_tsg *tsg = NULL;
	u32 branches = 0U;
	int ret = UNIT_FAIL;
	int err;
	u32 prune = F_TSG_UNBIND_CHANNEL_CHECK_HW_NEXT;

	tsg = nvgpu_tsg_open(g, getpid());
	assert(tsg != NULL);

	ch = nvgpu_channel_open_new(g, ~0U, false, getpid(), getpid());
	assert(ch != NULL);

	err = nvgpu_tsg_bind_channel(tsg, ch);
	assert(err == 0);

	for (branches = 0; branches < F_TSG_UNBIND_CHANNEL_CHECK_HW_LAST;
			branches++) {

		if (pruned(branches, prune)) {
			unit_verbose(m, "%s branches=%s (pruned)\n", __func__,
				branches_str(branches,
					f_tsg_unbind_channel_check_hw));
			continue;
		}
		subtest_setup(branches);
		unit_verbose(m, "%s branches=%s\n", __func__,
			branches_str(branches, f_tsg_unbind_channel_check_hw));

		g->ops.channel.read_state =
			branches & F_TSG_UNBIND_CHANNEL_CHECK_HW_NEXT ?
			stub_channel_read_state_NEXT : gops.channel.read_state;

		g->ops.tsg.unbind_channel_check_ctx_reload =
			branches & F_TSG_UNBIND_CHANNEL_CHECK_HW_CTX_RELOAD ?
			gops.tsg.unbind_channel_check_ctx_reload : NULL;

		g->ops.tsg.unbind_channel_check_eng_faulted =
			branches & F_TSG_UNBIND_CHANNEL_CHECK_HW_ENG_FAULTED ?
			gops.tsg.unbind_channel_check_eng_faulted : NULL;

		err = nvgpu_tsg_unbind_channel_check_hw_state(tsg, ch);

		if (branches & F_TSG_UNBIND_CHANNEL_CHECK_HW_NEXT) {
			assert(err != 0);
		} else {
			assert(err == 0);
		}
	}
	ret = UNIT_SUCCESS;

done:
	if (ret == UNIT_FAIL) {
		unit_err(m, "%s branches=%s\n", __func__,
			branches_str(branches, f_tsg_unbind_channel_check_hw));
	}
	if (ch != NULL) {
		nvgpu_channel_close(ch);
	}
	if (tsg != NULL) {
		nvgpu_ref_put(&tsg->refcount, nvgpu_tsg_release);
	}
	g->ops = gops;
	return ret;
}

#define F_UNBIND_CHANNEL_CHECK_CTX_RELOAD_SET		BIT(0)
#define F_UNBIND_CHANNEL_CHECK_CTX_RELOAD_CHID_MATCH	BIT(1)
#define F_UNBIND_CHANNEL_CHECK_CTX_RELOAD_LAST		BIT(2)

static const char *f_unbind_channel_check_ctx_reload[] = {
	"reload_set",
	"chid_match",
};

static void stub_channel_force_ctx_reload(struct nvgpu_channel *ch)
{
	stub[0].name = __func__;
	stub[0].chid = ch->chid;
}

int test_tsg_unbind_channel_check_ctx_reload(struct unit_module *m,
		struct gk20a *g, void *args)
{
	struct gpu_ops gops = g->ops;
	u32 branches = 0U;
	int ret = UNIT_FAIL;
	struct nvgpu_channel_hw_state hw_state;
	struct nvgpu_tsg *tsg = NULL;
	struct nvgpu_channel *chA = NULL;
	struct nvgpu_channel *chB = NULL;
	int err;

	tsg = nvgpu_tsg_open(g, getpid());
	assert(tsg != NULL);

	chA = nvgpu_channel_open_new(g, ~0U, false, getpid(), getpid());
	assert(chA != NULL);

	chB = nvgpu_channel_open_new(g, ~0U, false, getpid(), getpid());
	assert(chB != NULL);

	err = nvgpu_tsg_bind_channel(tsg, chA);
	assert(err == 0);

	g->ops.channel.force_ctx_reload = stub_channel_force_ctx_reload;

	for (branches = 0; branches < F_UNBIND_CHANNEL_CHECK_CTX_RELOAD_LAST;
			branches++) {

		subtest_setup(branches);
		unit_verbose(m, "%s branches=%s\n", __func__,
			branches_str(branches,
				f_unbind_channel_check_ctx_reload));

		hw_state.ctx_reload =
			branches & F_UNBIND_CHANNEL_CHECK_CTX_RELOAD_SET ?
			true : false;

		if ((branches & F_UNBIND_CHANNEL_CHECK_CTX_RELOAD_SET) &&
		    (branches & F_UNBIND_CHANNEL_CHECK_CTX_RELOAD_CHID_MATCH)) {
			assert(nvgpu_tsg_bind_channel(tsg, chB) == 0);
		}

		nvgpu_tsg_unbind_channel_check_ctx_reload(tsg, chA, &hw_state);

		if ((branches & F_UNBIND_CHANNEL_CHECK_CTX_RELOAD_SET) &&
		    (branches & F_UNBIND_CHANNEL_CHECK_CTX_RELOAD_CHID_MATCH)) {
			nvgpu_tsg_unbind_channel(tsg, chB);
			assert(stub[0].chid == chB->chid);
		}
	}
	ret = UNIT_SUCCESS;

done:
	if (ret != UNIT_SUCCESS) {
		unit_err(m, "%s branches=%s\n", __func__,
			branches_str(branches,
				f_unbind_channel_check_ctx_reload));
	}
	if (chA != NULL) {
		nvgpu_channel_close(chA);
	}
	if (chB != NULL) {
		nvgpu_channel_close(chB);
	}
	if (tsg != NULL) {
		nvgpu_ref_put(&tsg->refcount, nvgpu_tsg_release);
	}
	g->ops = gops;
	return ret;
}

#define F_TSG_ENABLE_CH			BIT(0)
#define F_TSG_ENABLE_STUB		BIT(1)
#define F_TSG_ENABLE_LAST		BIT(2)

static const char *f_tsg_enable[] = {
	"ch",
	"stub"
};

static void stub_channel_enable(struct nvgpu_channel *ch)
{
	stub[0].name = __func__;
	stub[0].chid = ch->chid;
	stub[0].count++;
}

static void stub_usermode_ring_doorbell(struct nvgpu_channel *ch)
{
	stub[1].name = __func__;
	stub[1].chid = ch->chid;
	stub[1].count++;
}

static void stub_channel_disable(struct nvgpu_channel *ch)
{
	stub[2].name = __func__;
	stub[2].chid = ch->chid;
	stub[2].count++;
}

int test_tsg_enable(struct unit_module *m,
		struct gk20a *g, void *args)
{
	struct gpu_ops gops = g->ops;
	struct nvgpu_tsg *tsgA = NULL;
	struct nvgpu_tsg *tsgB = NULL;
	struct nvgpu_tsg *tsg = NULL;
	struct nvgpu_channel *chA = NULL;
	u32 branches = 0U;
	int ret = UNIT_FAIL;
	int err;

	tsgA = nvgpu_tsg_open(g, getpid());
	assert(tsgA != NULL);

	tsgB = nvgpu_tsg_open(g, getpid());
	assert(tsgB != NULL);

	chA = nvgpu_channel_open_new(g, ~0U, false, getpid(), getpid());
	assert(chA != NULL);

	err = nvgpu_tsg_bind_channel(tsgA, chA);
	assert(err == 0);

	g->ops.channel.disable = stub_channel_disable;

	for (branches = 0U; branches < F_TSG_ENABLE_LAST; branches++) {

		subtest_setup(branches);
		unit_verbose(m, "%s branches=%s\n", __func__,
			branches_str(branches, f_tsg_enable));

		tsg = branches & F_TSG_ENABLE_CH ?
			tsgA : tsgB;

		g->ops.channel.enable =
			branches & F_TSG_ENABLE_STUB ?
			stub_channel_enable : gops.channel.enable;

		g->ops.usermode.ring_doorbell =
			branches & F_TSG_ENABLE_STUB ?
			stub_usermode_ring_doorbell :
			gops.usermode.ring_doorbell;

		g->ops.tsg.enable(tsg);

		if (branches & F_TSG_ENABLE_STUB) {
			if (tsg == tsgB) {
				assert(stub[0].count == 0);
				assert(stub[1].count == 0);
			}

			if (tsg == tsgA) {
				assert(stub[0].chid == chA->chid);
				assert(stub[1].count > 0);
			}
		}

		g->ops.channel.disable =
			branches & F_TSG_ENABLE_STUB ?
			stub_channel_disable : gops.channel.disable;

		g->ops.tsg.disable(tsg);

		if (branches & F_TSG_ENABLE_STUB) {
			if (tsg == tsgB) {
				assert(stub[2].count == 0);
			}

			if (tsg == tsgA) {
				assert(stub[2].chid == chA->chid);
			}
		}
	}

	ret = UNIT_SUCCESS;
done:
	if (ret != UNIT_SUCCESS) {
		unit_err(m, "%s branches=%s\n", __func__,
			branches_str(branches, f_tsg_enable));
	}
	if (chA != NULL) {
		nvgpu_channel_close(chA);
	}
	if (tsgA != NULL) {
		nvgpu_ref_put(&tsgA->refcount, nvgpu_tsg_release);
	}
	if (tsgB != NULL) {
		nvgpu_ref_put(&tsgB->refcount, nvgpu_tsg_release);
	}
	g->ops = gops;
	return ret;
}

int test_tsg_check_and_get_from_id(struct unit_module *m,
		struct gk20a *g, void *args)
{
	struct nvgpu_tsg *tsg;
	int ret = UNIT_FAIL;

	tsg = nvgpu_tsg_check_and_get_from_id(g, NVGPU_INVALID_TSG_ID);
	assert(tsg == NULL);

	tsg = nvgpu_tsg_open(g, getpid());
	assert(tsg != NULL);

	assert(nvgpu_tsg_check_and_get_from_id(g, tsg->tsgid) == tsg);
	nvgpu_ref_put(&tsg->refcount, nvgpu_tsg_release);

	ret = UNIT_SUCCESS;
done:
	return ret;
}

#define F_TSG_ABORT_CH_ABORT_CLEANUP_NULL	BIT(0)
#define F_TSG_ABORT_PREEMPT			BIT(1)
#define F_TSG_ABORT_CH				BIT(2)
#define F_TSG_ABORT_NON_ABORTABLE		BIT(3)
#define F_TSG_ABORT_CH_NON_REFERENCABLE		BIT(4)
#define F_TSG_ABORT_LAST			BIT(5)

static const char *f_tsg_abort[] = {
	"preempt",
	"ch",
	"ch_abort_cleanup_null",
	"non_abortable",
	"non_referenceable"
};

static int stub_fifo_preempt_tsg(struct gk20a *g, struct nvgpu_tsg *tsg)
{
	stub[0].tsgid = tsg->tsgid;
	return 0;
}

static void stub_channel_abort_clean_up(struct nvgpu_channel *ch)
{
	stub[1].chid = ch->chid;
}

int test_tsg_abort(struct unit_module *m, struct gk20a *g, void *args)
{
	struct gpu_ops gops = g->ops;
	struct nvgpu_tsg *tsgA = NULL;
	struct nvgpu_tsg *tsgB = NULL;
	struct nvgpu_tsg *tsg = NULL;
	struct nvgpu_channel *chA = NULL;
	bool preempt = false;
	u32 branches = 0U;
	int ret = UNIT_FAIL;
	u32 prune = F_TSG_ABORT_NON_ABORTABLE;
	int err;

	tsgA = nvgpu_tsg_open(g, getpid());
	assert(tsgA != NULL);

	tsgB = nvgpu_tsg_open(g, getpid());
	assert(tsgB != NULL);

	chA = nvgpu_channel_open_new(g, ~0U, false, getpid(), getpid());
	assert(chA != NULL);

	for (branches = 0U; branches < F_TSG_ABORT_LAST; branches++) {

		if (pruned(branches, prune)) {
			unit_verbose(m, "%s branches=%s (pruned)\n", __func__,
				branches_str(branches, f_tsg_abort));
			continue;
		}
		subtest_setup(branches);
		unit_verbose(m, "%s branches=%s\n", __func__,
			branches_str(branches, f_tsg_abort));

		g->ops.channel.abort_clean_up =
			branches & F_TSG_ABORT_CH_ABORT_CLEANUP_NULL ?
			NULL : stub_channel_abort_clean_up;

		g->ops.fifo.preempt_tsg = stub_fifo_preempt_tsg;

		tsg = branches & F_TSG_ABORT_CH ? tsgA : tsgB;

		tsg->abortable =
			branches & F_TSG_ABORT_NON_ABORTABLE ? false : true;

		preempt = branches & F_TSG_ABORT_PREEMPT ? true : false;

		if (branches & F_TSG_ABORT_CH_ABORT_CLEANUP_NULL) {
			g->ops.channel.abort_clean_up = NULL;
		}

		if (branches & F_TSG_ABORT_CH_NON_REFERENCABLE) {
			chA->referenceable = false;
		}

		if (chA->tsgid == NVGPU_INVALID_TSG_ID) {
			err = nvgpu_tsg_bind_channel(tsgA, chA);
			assert(err == 0);
		}

		nvgpu_tsg_abort(g, tsg, preempt);

		assert(preempt == (stub[0].tsgid == tsg->tsgid));

		assert(chA->unserviceable ==
			((tsg == tsgA) && (chA->referenceable)));

		if (!((branches & F_TSG_ABORT_CH_ABORT_CLEANUP_NULL) ||
		      (branches & F_TSG_ABORT_CH_NON_REFERENCABLE))) {
			assert((stub[1].chid == chA->chid) == (tsg == tsgA) );
			assert((stub[1].chid == NVGPU_INVALID_CHANNEL_ID) ==
				(tsg == tsgB));
		}

		tsg->abortable = true;
		chA->unserviceable = false;
		chA->referenceable = true;
	}

	ret = UNIT_SUCCESS;

done:
	if (ret == UNIT_FAIL) {
		unit_err(m, "%s branches=%s\n", __func__,
			branches_str(branches, f_tsg_abort));
	}
	if (chA != NULL) {
		nvgpu_channel_close(chA);
	}
	if (tsgA != NULL) {
		nvgpu_ref_put(&tsgA->refcount, nvgpu_tsg_release);
	}
	if (tsgB != NULL) {
		nvgpu_ref_put(&tsgB->refcount, nvgpu_tsg_release);
	}
	g->ops = gops;
	return ret;
}

#define F_TSG_SETUP_SW_VZALLOC_FAIL		BIT(0)
#define F_TSG_SETUP_SW_LAST			BIT(1)

static const char *f_tsg_setup_sw[] = {
	"vzalloc_fail",
};

int test_tsg_setup_sw(struct unit_module *m,
		struct gk20a *g, void *args)
{
	struct gpu_ops gops = g->ops;
	struct nvgpu_posix_fault_inj *kmem_fi;
	u32 branches = 0U;
	int ret = UNIT_FAIL;
	int err;
	u32 fail = F_TSG_SETUP_SW_VZALLOC_FAIL;
	u32 prune = fail;

	kmem_fi = nvgpu_kmem_get_fault_injection();

	for (branches = 0U; branches < F_TSG_SETUP_SW_LAST; branches++) {

		if (pruned(branches, prune)) {
			unit_verbose(m, "%s branches=%s (pruned)\n", __func__,
				branches_str(branches, f_tsg_setup_sw));
			continue;
		}
		subtest_setup(branches);
		unit_verbose(m, "%s branches=%s\n", __func__,
			branches_str(branches, f_tsg_setup_sw));

		nvgpu_posix_enable_fault_injection(kmem_fi,
			branches & F_TSG_SETUP_SW_VZALLOC_FAIL ?
			true : false, 0);

		err = nvgpu_tsg_setup_sw(g);

		if (branches & fail) {
			assert(err != 0);
		} else {
			assert(err == 0);
			nvgpu_tsg_cleanup_sw(g);
		}
	}

	ret = UNIT_SUCCESS;
done:
	nvgpu_posix_enable_fault_injection(kmem_fi, false, 0);
	if (ret != UNIT_SUCCESS) {
		unit_err(m, "%s branches=%s\n", __func__,
			branches_str(branches, f_tsg_setup_sw));
	}
	g->ops = gops;
	return ret;
}

#define F_TSG_MARK_ERROR_NO_CHANNEL		BIT(0)
#define F_TSG_MARK_ERROR_NON_REFERENCABLE	BIT(1)
#define F_TSG_MARK_ERROR_VERBOSE		BIT(2)
#define F_TSG_MARK_ERROR_LAST			BIT(3)

int test_tsg_mark_error(struct unit_module *m,
		struct gk20a *g, void *args)
{
	struct gpu_ops gops = g->ops;
	struct nvgpu_tsg *tsg = NULL;
	struct nvgpu_channel *ch = NULL;
	int ret = UNIT_FAIL;
	bool verbose;
	int err;
	u32 branches;
	static const char *labels[] = {
		"no_channel",
		"non_referencable",
		"verbose",
	};
	u32 prune =
		F_TSG_MARK_ERROR_NO_CHANNEL |
		F_TSG_MARK_ERROR_NON_REFERENCABLE;
	struct nvgpu_posix_channel ch_priv;

	for (branches = 0U; branches < F_TSG_MARK_ERROR_LAST; branches++) {

		if (pruned(branches, prune)) {
			unit_verbose(m, "%s branches=%s (pruned)\n", __func__,
				branches_str(branches, labels));
			continue;
		}
		subtest_setup(branches);
		unit_verbose(m, "%s branches=%s\n", __func__,
			branches_str(branches, labels));

		tsg = nvgpu_tsg_open(g, getpid());
		assert(tsg != NULL);

		ch = nvgpu_channel_open_new(g, ~0U, false, getpid(), getpid());
		assert(ch != NULL);

		ch->os_priv = &ch_priv;
		ch_priv.err_notifier.error = U32_MAX;
		ch_priv.err_notifier.status = 0;

		if ((branches & F_TSG_MARK_ERROR_NO_CHANNEL) == 0) {
			err = nvgpu_tsg_bind_channel(tsg, ch);
			assert(err == 0);
		}

		if (branches & F_TSG_MARK_ERROR_NON_REFERENCABLE) {
			ch->referenceable = false;
		}

		ch->ctxsw_timeout_debug_dump =
			branches & F_TSG_MARK_ERROR_VERBOSE ? true : false;

		nvgpu_tsg_set_error_notifier(g, tsg,
			NVGPU_ERR_NOTIFIER_FIFO_ERROR_IDLE_TIMEOUT);

		verbose = nvgpu_tsg_mark_error(g, tsg);

		if ((branches & F_TSG_MARK_ERROR_NO_CHANNEL) ||
		    (branches & F_TSG_MARK_ERROR_NON_REFERENCABLE)) {
			assert(!verbose);
		}

		if (branches & F_TSG_MARK_ERROR_VERBOSE) {
			assert(verbose);
		} else {
			assert(!verbose);
		}

		nvgpu_channel_close(ch);
		ch = NULL;
		nvgpu_ref_put(&tsg->refcount, nvgpu_tsg_release);
		tsg = NULL;
	}

	ret = UNIT_SUCCESS;
done:
	if (ret != UNIT_SUCCESS) {
		unit_err(m, "%s branches=%s\n", __func__,
			branches_str(branches, labels));
	}
	if (ch != NULL) {
		nvgpu_channel_close(ch);
	}
	if (tsg != NULL) {
		nvgpu_ref_put(&tsg->refcount, nvgpu_tsg_release);
	}
	g->ops = gops;
	return ret;
}

int test_tsg_set_ctx_mmu_error(struct unit_module *m,
		struct gk20a *g, void *args)
{
	struct nvgpu_tsg *tsg = NULL;
	struct nvgpu_channel *ch = NULL;
	int ret = UNIT_FAIL;
	struct nvgpu_posix_channel ch_priv;
	int err;

	tsg = nvgpu_tsg_open(g, getpid());
	assert(tsg != NULL);

	ch = nvgpu_channel_open_new(g, ~0U, false, getpid(), getpid());
	assert(ch != NULL);

	err = nvgpu_tsg_bind_channel(tsg, ch);
	assert(err == 0);

	ch->os_priv = &ch_priv;
	ch_priv.err_notifier.error = U32_MAX;
	ch_priv.err_notifier.status = 0;

	nvgpu_tsg_set_ctx_mmu_error(g, tsg);

	assert(ch_priv.err_notifier.error == NVGPU_ERR_NOTIFIER_FIFO_ERROR_MMU_ERR_FLT);
	assert(ch_priv.err_notifier.status != 0);

	ret = UNIT_SUCCESS;
done:
	if (ch != NULL) {
		nvgpu_channel_close(ch);
	}
	if (tsg != NULL) {
		nvgpu_ref_put(&tsg->refcount, nvgpu_tsg_release);
	}
	return ret;
}

#define F_TSG_RESET_FAULTED_NO_RESET_HAL		BIT(0)
#define F_TSG_RESET_FAULTED_TSG_NULL			BIT(1)
#define F_TSG_RESET_FAULTED_LAST			BIT(2)

static void stub_channel_reset_faulted(struct gk20a *g, struct nvgpu_channel *ch,
		bool eng, bool pbdma)
{
	stub[0].name = __func__;
	stub[0].chid = ch->chid;
}

int test_tsg_reset_faulted_eng_pbdma(struct unit_module *m,
		struct gk20a *g, void *args)
{
	struct nvgpu_tsg *tsg = NULL;
	struct nvgpu_channel *ch = NULL;
	int ret = UNIT_FAIL;
	struct gpu_ops gops = g->ops;
	int err;
	u32 branches;
	static const char *labels[] = {
		"no_reset_hal",
		"tsg_null"
	};
	u32 fail =
		F_TSG_RESET_FAULTED_NO_RESET_HAL |
		F_TSG_RESET_FAULTED_TSG_NULL;
	u32 prune = fail;

	tsg = nvgpu_tsg_open(g, getpid());
	assert(tsg != NULL);

	ch = nvgpu_channel_open_new(g, ~0U, false, getpid(), getpid());
	assert(ch != NULL);

	err = nvgpu_tsg_bind_channel(tsg, ch);
	assert(err == 0);

	for (branches = 0U; branches < F_TSG_MARK_ERROR_LAST; branches++) {

		if (pruned(branches, prune)) {
			unit_verbose(m, "%s branches=%s (pruned)\n", __func__,
				branches_str(branches, labels));
			continue;
		}
		subtest_setup(branches);
		unit_verbose(m, "%s branches=%s\n", __func__,
			branches_str(branches, labels));

		g->ops.channel.reset_faulted =
			branches & F_TSG_RESET_FAULTED_NO_RESET_HAL ?
			NULL : stub_channel_reset_faulted;

		if (branches & F_TSG_RESET_FAULTED_TSG_NULL) {
			nvgpu_tsg_reset_faulted_eng_pbdma(g, NULL, true, true);
		} else {
			nvgpu_tsg_reset_faulted_eng_pbdma(g, tsg, true, true);
		}

		if (branches & fail) {
			assert(stub[0].chid != ch->chid);
		} else {
			assert(stub[0].chid == ch->chid);
		}
	}

	ret = UNIT_SUCCESS;
done:
	if (ch != NULL) {
		nvgpu_channel_close(ch);
	}
	if (tsg != NULL) {
		nvgpu_ref_put(&tsg->refcount, nvgpu_tsg_release);
	}
	g->ops = gops;
	return ret;
}



struct unit_module_test nvgpu_tsg_tests[] = {
	UNIT_TEST(setup_sw, test_tsg_setup_sw, &unit_ctx, 0),
	UNIT_TEST(init_support, test_fifo_init_support, &unit_ctx, 0),
	UNIT_TEST(open, test_tsg_open, &unit_ctx, 0),
	UNIT_TEST(release, test_tsg_release, &unit_ctx, 0),
	UNIT_TEST(get_from_id, test_tsg_check_and_get_from_id, &unit_ctx, 0),
	UNIT_TEST(bind_channel, test_tsg_bind_channel, &unit_ctx, 0),
	UNIT_TEST(unbind_channel, test_tsg_unbind_channel, &unit_ctx, 0),
	UNIT_TEST(unbind_channel_check_hw_state,
		test_tsg_unbind_channel_check_hw_state, &unit_ctx, 0),
	UNIT_TEST(unbind_channel_check_ctx_reload,
		test_tsg_unbind_channel_check_ctx_reload, &unit_ctx, 0),
	UNIT_TEST(enable_disable, test_tsg_enable, &unit_ctx, 0),
	UNIT_TEST(abort, test_tsg_abort, &unit_ctx, 0),
	UNIT_TEST(mark_error, test_tsg_mark_error, &unit_ctx, 0),
	UNIT_TEST(set_ctx_mmu_error, test_tsg_set_ctx_mmu_error, &unit_ctx, 0),
	UNIT_TEST(reset_faulted_eng_pbdma, test_tsg_reset_faulted_eng_pbdma, &unit_ctx, 0),
	UNIT_TEST(remove_support, test_fifo_remove_support, &unit_ctx, 0),
};

UNIT_MODULE(nvgpu_tsg, nvgpu_tsg_tests, UNIT_PRIO_NVGPU_TEST);
