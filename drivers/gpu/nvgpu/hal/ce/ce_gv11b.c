/*
 * Volta GPU series Copy Engine.
 *
 * Copyright (c) 2016-2019, NVIDIA CORPORATION.  All rights reserved.
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

#include <nvgpu/io.h>
#include "nvgpu/log.h"
#include "nvgpu/bitops.h"
#include <nvgpu/gk20a.h>
#include <nvgpu/nvgpu_err.h>

#include "hal/ce/ce_gp10b.h"
#include "hal/ce/ce_gv11b.h"

#include <nvgpu/hw/gv11b/hw_ce_gv11b.h>
#include <nvgpu/hw/gv11b/hw_top_gv11b.h>

void nvgpu_report_ce_error(struct gk20a *g, u32 inst,
		u32 err_type, u32 status)
{
	int ret = 0;

	if (g->ops.ce.err_ops.report_ce_err == NULL) {
		return;
	}
	ret = g->ops.ce.err_ops.report_ce_err(g,
			NVGPU_ERR_MODULE_CE, inst, err_type, status);
	if (ret != 0) {
		nvgpu_err(g,
			"report_ce_err failed inst=%u err_type=%u status=%u",
			inst, err_type, status);
	}
}

u32 gv11b_ce_get_num_pce(struct gk20a *g)
{
	/*
	 * register contains a bitmask indicating which physical copy
	 * engines are present (and not floorswept).
	 */
	u32 num_pce;
	u32 ce_pce_map = nvgpu_readl(g, ce_pce_map_r());

	num_pce = U32(hweight32(ce_pce_map));
	nvgpu_log_info(g, "num PCE: %d", num_pce);
	return num_pce;
}

void gv11b_ce_stall_isr(struct gk20a *g, u32 inst_id, u32 pri_base)
{
	u32 ce_intr = nvgpu_readl(g, ce_intr_status_r(inst_id));
	u32 clear_intr = 0U;

	nvgpu_log(g, gpu_dbg_intr, "ce isr 0x%08x 0x%08x", ce_intr, inst_id);

	/*
	 * An INVALID_CONFIG interrupt will be generated if a floorswept
	 * PCE is assigned to a valid LCE in the NV_CE_PCE2LCE_CONFIG
	 * registers. This is a fatal error and the LCE will have to be
	 * reset to get back to a working state.
	 */
	if ((ce_intr & ce_intr_status_invalid_config_pending_f()) != 0U) {
		nvgpu_report_ce_error(g, inst_id,
				GPU_CE_INVALID_CONFIG, ce_intr);
		nvgpu_log(g, gpu_dbg_intr,
			"ce: inst %d: invalid config", inst_id);
		clear_intr |= ce_intr_status_invalid_config_reset_f();
	}

	/*
	 * A MTHD_BUFFER_FAULT interrupt will be triggered if any access
	 * to a method buffer during context load or save encounters a fault.
	 * This is a fatal interrupt and will require at least the LCE to be
	 * reset before operations can start again, if not the entire GPU.
	 */
	if ((ce_intr & ce_intr_status_mthd_buffer_fault_pending_f()) != 0U) {
		nvgpu_report_ce_error(g, inst_id,
				GPU_CE_METHOD_BUFFER_FAULT, ce_intr);
		nvgpu_log(g, gpu_dbg_intr,
			"ce: inst %d: mthd buffer fault", inst_id);
		clear_intr |= ce_intr_status_mthd_buffer_fault_reset_f();
	}

	nvgpu_writel(g, ce_intr_status_r(inst_id), clear_intr);

	gp10b_ce_stall_isr(g, inst_id, pri_base);
}

void gv11b_ce_mthd_buffer_fault_in_bar2_fault(struct gk20a *g)
{
	u32 reg_val, num_lce, lce, clear_intr;

	num_lce = g->ops.top.get_num_lce(g);

	for (lce = 0U; lce < num_lce; lce++) {
		reg_val = nvgpu_readl(g, ce_intr_status_r(lce));
		if ((reg_val &
			ce_intr_status_mthd_buffer_fault_pending_f()) != 0U) {
			nvgpu_log(g, gpu_dbg_intr,
			"ce: lce %d: mthd buffer fault", lce);
			clear_intr = ce_intr_status_mthd_buffer_fault_reset_f();
			nvgpu_writel(g, ce_intr_status_r(lce), clear_intr);
		}
	}
}