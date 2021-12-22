/*
 * Copyright (c) 2014-2022, NVIDIA CORPORATION.  All rights reserved.
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
 *
 * This file is autogenerated.  Do not edit.
 */

#include <nvgpu/types.h>
#include <nvgpu/io.h>
#include <nvgpu/enabled.h>
#include <nvgpu/utils.h>
#include <nvgpu/static_analysis.h>

#include <nvgpu/gk20a.h>
#include <nvgpu/fifo.h>
#include <nvgpu/runlist.h>

#include "hal/power_features/cg/gating_reglist.h"
#include "ga10b_gating_reglist.h"

#define GATING_DESC_SIZE (u32)(sizeof(struct gating_desc))

/* slcg bus */
static const struct gating_desc ga10b_slcg_bus[] = {
	{.addr = 0x00001c04U, .prod = 0x00000000U, .disable = 0x000003feU},
};

/* slcg ce2 */
static const struct gating_desc ga10b_slcg_ce2[] = {
	{.addr = 0x00104204U, .prod = 0x00000000U, .disable = 0x000007feU},
};

/* slcg chiplet */
static const struct gating_desc ga10b_slcg_chiplet[] = {
	{.addr = 0x0010e07cU, .prod = 0x00000000U, .disable = 0x00000001U},
	{.addr = 0x0010e17cU, .prod = 0x00000000U, .disable = 0x00000001U},
};

/* slcg fb */
static const struct gating_desc ga10b_slcg_fb[] = {
	{.addr = 0x00100d14U, .prod = 0x00010c20U, .disable = 0xfffffffeU},
	{.addr = 0x00100c9cU, .prod = 0x00000000U, .disable = 0x000001feU},
};

/* slcg runlist*/
static const struct gating_desc ga10b_slcg_runlist[] = {
	{.addr = 0x00000054U, .prod = 0x00000000U, .disable = 0x0001fffeU},
};

/* slcg gr */
static const struct gating_desc ga10b_slcg_gr[] = {
	{.addr = 0x004041f4U, .prod = 0x00200000U, .disable = 0x0ffffffeU},
	{.addr = 0x00409134U, .prod = 0x00000140U, .disable = 0x0003fffeU},
	{.addr = 0x00409894U, .prod = 0x00000000U, .disable = 0x0000fffeU},
	{.addr = 0x00406004U, .prod = 0x00000000U, .disable = 0x0001fffeU},
	{.addr = 0x00405864U, .prod = 0x00000000U, .disable = 0x000001feU},
	{.addr = 0x00405bf4U, .prod = 0x00000000U, .disable = 0x00000002U},
	{.addr = 0x004078c4U, .prod = 0x00000000U, .disable = 0x000001feU},
	{.addr = 0x00405910U, .prod = 0xfffffff0U, .disable = 0xfffffffeU},
	{.addr = 0x00408044U, .prod = 0x00000000U, .disable = 0x00000ffeU},
	{.addr = 0x00407004U, .prod = 0x00000002U, .disable = 0x000001feU},
	{.addr = 0x0041a134U, .prod = 0x00000140U, .disable = 0x0003fffeU},
	{.addr = 0x0041a894U, .prod = 0x00000000U, .disable = 0x0000fffeU},
	{.addr = 0x00418974U, .prod = 0x00000000U, .disable = 0x0001fffeU},
	{.addr = 0x00418e10U, .prod = 0xfffffffeU, .disable = 0xfffffffeU},
	{.addr = 0x0041889cU, .prod = 0x00000000U, .disable = 0x000001feU},
	{.addr = 0x0041949cU, .prod = 0x00000000U, .disable = 0x000001feU},
	{.addr = 0x00418504U, .prod = 0x00000000U, .disable = 0x01fffffeU},
	{.addr = 0x0041860cU, .prod = 0x00000000U, .disable = 0x000001feU},
	{.addr = 0x0041868cU, .prod = 0x00000000U, .disable = 0x0000001eU},
	{.addr = 0x0041871cU, .prod = 0x00000000U, .disable = 0x000003feU},
	{.addr = 0x00418388U, .prod = 0x00000000U, .disable = 0x00000001U},
	{.addr = 0x0041882cU, .prod = 0x00000000U, .disable = 0x0001fffeU},
	{.addr = 0x00418bc0U, .prod = 0x00000000U, .disable = 0x000001feU},
	{.addr = 0x00418c74U, .prod = 0xffffff80U, .disable = 0xfffffffeU},
	{.addr = 0x00418cf4U, .prod = 0xfffffff0U, .disable = 0xfffffffeU},
	{.addr = 0x00418d74U, .prod = 0xffffffe0U, .disable = 0xfffffffeU},
	{.addr = 0x00418f10U, .prod = 0xffffffe0U, .disable = 0xfffffffeU},
	{.addr = 0x00419024U, .prod = 0x000001feU, .disable = 0x000001feU},
	{.addr = 0x00419d24U, .prod = 0x00000000U, .disable = 0x000000ffU},
	{.addr = 0x0041986cU, .prod = 0x00000104U, .disable = 0x00fffffeU},
	{.addr = 0x00419c84U, .prod = 0x0003fffcU, .disable = 0x0003fffeU},
	{.addr = 0x00419c8cU, .prod = 0xff71f384U, .disable = 0xfffffffeU},
	{.addr = 0x00419c94U, .prod = 0x38f00340U, .disable = 0x3ffffffeU},
	{.addr = 0x00419ca4U, .prod = 0x00003ffcU, .disable = 0x00003ffeU},
	{.addr = 0x00419cacU, .prod = 0x0001f800U, .disable = 0x0001fffeU},
	{.addr = 0x00419cb4U, .prod = 0x00001d00U, .disable = 0x00001ffeU},
	{.addr = 0x00419a44U, .prod = 0x00000008U, .disable = 0x0000000eU},
	{.addr = 0x00419a4cU, .prod = 0x000001f8U, .disable = 0x000001feU},
	{.addr = 0x00419a54U, .prod = 0x0000003cU, .disable = 0x0000003eU},
	{.addr = 0x00419a5cU, .prod = 0x0000000cU, .disable = 0x0000000eU},
	{.addr = 0x00419a64U, .prod = 0x00000182U, .disable = 0x000001feU},
	{.addr = 0x00419a7cU, .prod = 0x0000003cU, .disable = 0x0000003eU},
	{.addr = 0x00419a84U, .prod = 0x0000000cU, .disable = 0x0000000eU},
	{.addr = 0x00419c74U, .prod = 0x0000001eU, .disable = 0x0000001eU},
	{.addr = 0x0041be2cU, .prod = 0x04115fc0U, .disable = 0xfffffffeU},
	{.addr = 0x0041bfecU, .prod = 0xfffffff0U, .disable = 0xfffffffeU},
	{.addr = 0x0041bed4U, .prod = 0xfffffff8U, .disable = 0xfffffffeU},
	{.addr = 0x0041b81cU, .prod = 0x00000000U, .disable = 0x0001fffeU},
	{.addr = 0x0041b878U, .prod = 0x00000000U, .disable = 0x0001fffeU},
	{.addr = 0x0041b89cU, .prod = 0x00000000U, .disable = 0x0001fffeU},
	{.addr = 0x0041bc1cU, .prod = 0x00000000U, .disable = 0x0001fffeU},
	{.addr = 0x0041bc78U, .prod = 0x00000000U, .disable = 0x0001fffeU},
	{.addr = 0x0041bc9cU, .prod = 0x00000000U, .disable = 0x0001fffeU},
};

/* slcg ltc */
static const struct gating_desc ga10b_slcg_ltc[] = {
	{.addr = 0x0017e050U, .prod = 0x00000000U, .disable = 0xfffffffeU},
	{.addr = 0x0017e35cU, .prod = 0x00000000U, .disable = 0xfffffffeU},
};

/* slcg perf */
static const struct gating_desc ga10b_slcg_perf[] = {
	{.addr = 0x00248018U, .prod = 0x00000000U, .disable = 0x00000003U},
	{.addr = 0x00251a18U, .prod = 0x00000000U, .disable = 0x00000003U},
	{.addr = 0x00251818U, .prod = 0x00000000U, .disable = 0x00000003U},
	{.addr = 0x0024a044U, .prod = 0x00000000U, .disable = 0x00000001U},
};

/* slcg PriRing */
static const struct gating_desc ga10b_slcg_priring[] = {
	{.addr = 0x001200a8U, .prod = 0x00000000U, .disable = 0x00000001U},
};

/* slcg pmu */
static const struct gating_desc ga10b_slcg_pmu[] = {
	{.addr = 0x0010a134U, .prod = 0x00040140U, .disable = 0x0003fffeU},
	{.addr = 0x0010aa74U, .prod = 0x00000400U, .disable = 0x000007feU},
	{.addr = 0x0010ae74U, .prod = 0x00000000U, .disable = 0x0000000fU},
};

/* therm gr */
static const struct gating_desc ga10b_slcg_therm[] = {
	{.addr = 0x000206b8U, .prod = 0x00000000U, .disable = 0x00000007U},
};

/* slcg Xbar */
static const struct gating_desc ga10b_slcg_xbar[] = {
	{.addr = 0x0013c824U, .prod = 0x00000000U, .disable = 0x7ffffffeU},
	{.addr = 0x0013dc08U, .prod = 0x00000000U, .disable = 0xfffffffeU},
	{.addr = 0x0013c924U, .prod = 0x00000000U, .disable = 0x7ffffffeU},
	{.addr = 0x0013cb84U, .prod = 0x00000000U, .disable = 0x1ffffffeU},
	{.addr = 0x0013cb8cU, .prod = 0x00000000U, .disable = 0x1ffffffeU},
};

/* slcg Hshub */
static const struct gating_desc ga10b_slcg_hshub[] = {
	{.addr = 0x000043f4U, .prod = 0x00000000U, .disable = 0xfffffffeU},
	{.addr = 0x000047f4U, .prod = 0x00000000U, .disable = 0xfffffffeU},
	{.addr = 0x00004bf4U, .prod = 0x00000000U, .disable = 0xfffffffeU},
};

/* slcg timer */
static const struct gating_desc ga10b_slcg_timer[] = {
	{.addr = 0x00009600U, .prod = 0x00000000U, .disable = 0x00000002U},
};

/* slcg rs_ctrl_fbp */
static const struct gating_desc ga10b_slcg_rs_ctrl_fbp[] = {
	{.addr = 0x00128048U, .prod = 0x00000000U, .disable = 0x00000003U},
};

/* slcg rs_ctrl_gpc */
static const struct gating_desc ga10b_slcg_rs_ctrl_gpc[] = {
	{.addr = 0x00124048U, .prod = 0x00000000U, .disable = 0x00000003U},
};

/* slcg rs_ctrl_sys */
static const struct gating_desc ga10b_slcg_rs_ctrl_sys[] = {
	{.addr = 0x00122048U, .prod = 0x00000000U, .disable = 0x00000003U},
};

/* slcg rs_fbp */
static const struct gating_desc ga10b_slcg_rs_fbp[] = {
	{.addr = 0x00128250U, .prod = 0x00000000U, .disable = 0x000007ffU},
	{.addr = 0x00121250U, .prod = 0x00000000U, .disable = 0x000007ffU},
};

/* slcg rs_gpc */
static const struct gating_desc ga10b_slcg_rs_gpc[] = {
	{.addr = 0x00124250U, .prod = 0x00000000U, .disable = 0x000007ffU},
	{.addr = 0x00120a50U, .prod = 0x00000000U, .disable = 0x000007ffU},
};

/* slcg rs_sys */
static const struct gating_desc ga10b_slcg_rs_sys[] = {
	{.addr = 0x00122250U, .prod = 0x00000000U, .disable = 0x000007ffU},
};

/* blcg bus */
static const struct gating_desc ga10b_blcg_bus[] = {
	{.addr = 0x00001c00U, .prod = 0x00000042U, .disable = 0x00000000U},
};

/* blcg ce */
static const struct gating_desc ga10b_blcg_ce[] = {
	{.addr = 0x00104200U, .prod = 0x0000c244U, .disable = 0x00000000U},
};

/* blcg fb */
static const struct gating_desc ga10b_blcg_fb[] = {
	{.addr = 0x00100d10U, .prod = 0x0000c270U, .disable = 0x00000000U},
	{.addr = 0x00100d30U, .prod = 0x0000c270U, .disable = 0x00000000U},
	{.addr = 0x00100c98U, .prod = 0x00004242U, .disable = 0x00000000U},
};

/* blcg runlist */
static const struct gating_desc ga10b_blcg_runlist[] = {
	{.addr = 0x00000050U, .prod = 0x00004042U, .disable = 0x00000000U},
};

/* blcg gr */
static const struct gating_desc ga10b_blcg_gr[] = {
	{.addr = 0x004041f0U, .prod = 0x0000c646U, .disable = 0x00000000U},
	{.addr = 0x00409890U, .prod = 0x0000007fU, .disable = 0x00000000U},
	{.addr = 0x004098b0U, .prod = 0x0000007fU, .disable = 0x00000000U},
	{.addr = 0x00406000U, .prod = 0x0000c444U, .disable = 0x00000000U},
	{.addr = 0x00405860U, .prod = 0x0000c242U, .disable = 0x00000000U},
	{.addr = 0x00405bf0U, .prod = 0x0000c444U, .disable = 0x00000000U},
	{.addr = 0x004078c0U, .prod = 0x00004242U, .disable = 0x00000000U},
	{.addr = 0x0040590cU, .prod = 0x0000c444U, .disable = 0x00000000U},
	{.addr = 0x00408040U, .prod = 0x0000c444U, .disable = 0x00000000U},
	{.addr = 0x00407000U, .prod = 0x4000c242U, .disable = 0x00000000U},
	{.addr = 0x0041a890U, .prod = 0x0000427fU, .disable = 0x00000000U},
	{.addr = 0x0041a8b0U, .prod = 0x0000007fU, .disable = 0x00000000U},
	{.addr = 0x00418970U, .prod = 0x0000c242U, .disable = 0x00000000U},
	{.addr = 0x00418e0cU, .prod = 0x0000c444U, .disable = 0x00000000U},
	{.addr = 0x00418898U, .prod = 0x00004242U, .disable = 0x00000000U},
	{.addr = 0x00419498U, .prod = 0x00004242U, .disable = 0x00000000U},
	{.addr = 0x00418500U, .prod = 0x0000c244U, .disable = 0x00000000U},
	{.addr = 0x00418608U, .prod = 0x0000c242U, .disable = 0x00000000U},
	{.addr = 0x00418688U, .prod = 0x0000c242U, .disable = 0x00000000U},
	{.addr = 0x00418718U, .prod = 0x00000042U, .disable = 0x00000000U},
	{.addr = 0x00418828U, .prod = 0x00008444U, .disable = 0x00000000U},
	{.addr = 0x00418bbcU, .prod = 0x0000c242U, .disable = 0x00000000U},
	{.addr = 0x00418c70U, .prod = 0x0000c444U, .disable = 0x00000000U},
	{.addr = 0x00418cf0U, .prod = 0x0000c444U, .disable = 0x00000000U},
	{.addr = 0x00418d70U, .prod = 0x0000c444U, .disable = 0x00000000U},
	{.addr = 0x00418f0cU, .prod = 0x0000c444U, .disable = 0x00000000U},
	{.addr = 0x00419020U, .prod = 0x0000c242U, .disable = 0x00000000U},
	{.addr = 0x00419868U, .prod = 0x00008242U, .disable = 0x00000000U},
	{.addr = 0x00419c80U, .prod = 0x00004045U, .disable = 0x00000000U},
	{.addr = 0x00419c88U, .prod = 0x00004047U, .disable = 0x00000000U},
	{.addr = 0x00419c90U, .prod = 0x00004046U, .disable = 0x00000000U},
	{.addr = 0x00419c98U, .prod = 0x00000042U, .disable = 0x00000000U},
	{.addr = 0x00419ca0U, .prod = 0x00004047U, .disable = 0x00000000U},
	{.addr = 0x00419ca8U, .prod = 0x00000046U, .disable = 0x00000000U},
	{.addr = 0x00419cb0U, .prod = 0x00000045U, .disable = 0x00000000U},
	{.addr = 0x00419a40U, .prod = 0x00000242U, .disable = 0x00000000U},
	{.addr = 0x00419a48U, .prod = 0x00000242U, .disable = 0x00000000U},
	{.addr = 0x00419a50U, .prod = 0x00000242U, .disable = 0x00000000U},
	{.addr = 0x00419a58U, .prod = 0x00000242U, .disable = 0x00000000U},
	{.addr = 0x00419a60U, .prod = 0x00000244U, .disable = 0x00000000U},
	{.addr = 0x00419a68U, .prod = 0x00000242U, .disable = 0x00000000U},
	{.addr = 0x00419a78U, .prod = 0x00000242U, .disable = 0x00000000U},
	{.addr = 0x00419a80U, .prod = 0x00000242U, .disable = 0x00000000U},
	{.addr = 0x00419a70U, .prod = 0x00000545U, .disable = 0x00000000U},
	{.addr = 0x00419a88U, .prod = 0x00000545U, .disable = 0x00000000U},
	{.addr = 0x00419c70U, .prod = 0x0000c444U, .disable = 0x00000000U},
	{.addr = 0x0041be28U, .prod = 0x00008242U, .disable = 0x00000000U},
	{.addr = 0x0041bfe8U, .prod = 0x0000c444U, .disable = 0x00000000U},
	{.addr = 0x0041bed0U, .prod = 0x0000c444U, .disable = 0x00000000U},
	{.addr = 0x0041b818U, .prod = 0x0000c242U, .disable = 0x00000000U},
	{.addr = 0x0041b874U, .prod = 0x0000c242U, .disable = 0x00000000U},
	{.addr = 0x0041b898U, .prod = 0x0000c242U, .disable = 0x00000000U},
	{.addr = 0x0041bc18U, .prod = 0x0000c242U, .disable = 0x00000000U},
	{.addr = 0x0041bc74U, .prod = 0x0000c242U, .disable = 0x00000000U},
	{.addr = 0x0041bc98U, .prod = 0x0000c242U, .disable = 0x00000000U},
};

/* blcg ltc */
static const struct gating_desc ga10b_blcg_ltc[] = {
	{.addr = 0x0017e030U, .prod = 0x00000044U, .disable = 0x00000000U},
	{.addr = 0x0017e040U, .prod = 0x00000044U, .disable = 0x00000000U},
	{.addr = 0x0017e3e0U, .prod = 0x00000044U, .disable = 0x00000000U},
	{.addr = 0x0017e3c8U, .prod = 0x00000044U, .disable = 0x00000000U},
};

/* blcg pmu */
static const struct gating_desc ga10b_blcg_pmu[] = {
	{.addr = 0x0010aa70U, .prod = 0x00000045U, .disable = 0x00000000U},
};

/* blcg Xbar */
static const struct gating_desc ga10b_blcg_xbar[] = {
	{.addr = 0x0013c820U, .prod = 0x0001004aU, .disable = 0x00000000U},
	{.addr = 0x0013dc04U, .prod = 0x0001004aU, .disable = 0x00000000U},
	{.addr = 0x0013c920U, .prod = 0x0000004aU, .disable = 0x00000000U},
	{.addr = 0x0013cb80U, .prod = 0x00000042U, .disable = 0x00000000U},
	{.addr = 0x0013cb88U, .prod = 0x00000042U, .disable = 0x00000000U},
};

/* blcg Hshub */
static const struct gating_desc ga10b_blcg_hshub[] = {
	{.addr = 0x000043f0U, .prod = 0x0000c242U, .disable = 0x00000000U},
	{.addr = 0x000047f0U, .prod = 0x0000c242U, .disable = 0x00000000U},
	{.addr = 0x00004bf0U, .prod = 0x0000c242U, .disable = 0x00000000U},
};

/* elcg ce */
static const struct gating_desc ga10b_elcg_ce[] = {
	{.addr = 0x00104044U, .prod = 0x00000001U, .disable = 0x00000000U},
};

/* inline functions */
void ga10b_slcg_bus_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_bus)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_bus[i].addr;
			u32 val = prod ? ga10b_slcg_bus[i].prod :
					 ga10b_slcg_bus[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_bus_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_bus));
}

const struct gating_desc *ga10b_slcg_bus_get_gating_prod(void)
{
	return ga10b_slcg_bus;
}

void ga10b_slcg_ce2_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_ce2)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_ce2[i].addr;
			u32 val = prod ? ga10b_slcg_ce2[i].prod :
					 ga10b_slcg_ce2[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_ce2_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_ce2));
}

const struct gating_desc *ga10b_slcg_ce2_get_gating_prod(void)
{
	return ga10b_slcg_ce2;
}

void ga10b_slcg_chiplet_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_chiplet)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_chiplet[i].addr;
			u32 val = prod ? ga10b_slcg_chiplet[i].prod :
					 ga10b_slcg_chiplet[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_chiplet_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_chiplet));
}

const struct gating_desc *ga10b_slcg_chiplet_get_gating_prod(void)
{
	return ga10b_slcg_chiplet;
}

void ga10b_slcg_fb_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_fb)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_fb[i].addr;
			u32 val = prod ? ga10b_slcg_fb[i].prod :
					 ga10b_slcg_fb[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_fb_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_fb));
}

const struct gating_desc *ga10b_slcg_fb_get_gating_prod(void)
{
	return ga10b_slcg_fb;
}

void ga10b_slcg_runlist_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_runlist)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			struct nvgpu_fifo *f = &g->fifo;
			struct nvgpu_runlist *runlist;
			u32 j, runlist_pri_base;
			u32 reg = ga10b_slcg_runlist[i].addr;
			u32 val = prod ? ga10b_slcg_runlist[i].prod :
					 ga10b_slcg_runlist[i].disable;
			for (j = 0U; j < f->num_runlists; j++) {
				runlist = &f->active_runlists[j];
				runlist_pri_base = runlist->runlist_pri_base;
				nvgpu_writel(g, nvgpu_safe_add_u32(reg, runlist_pri_base), val);
			}
		}
	}
}

u32 ga10b_slcg_runlist_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_runlist));
}

const struct gating_desc *ga10b_slcg_runlist_get_gating_prod(void)
{
	return ga10b_slcg_runlist;
}

void ga10b_slcg_gr_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_gr)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_gr[i].addr;
			u32 val = prod ? ga10b_slcg_gr[i].prod :
					 ga10b_slcg_gr[i].disable;
#ifdef CONFIG_NVGPU_MIG
			if ((nvgpu_is_enabled(g, NVGPU_SUPPORT_MIG)) &&
					(g->ops.gr.init.is_allowed_reg
						!= NULL) &&
					(!(g->ops.gr.init.is_allowed_reg(g,
						reg)))) {
				nvgpu_log(g, gpu_dbg_mig | gpu_dbg_gr,
					"(MIG) Skip slcg graphics gating reg "
						"index[%u] addr[%x] value[%x] ",
				i, reg, val);
				continue;
			}
#endif
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_gr_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_gr));
}

const struct gating_desc *ga10b_slcg_gr_get_gating_prod(void)
{
	return ga10b_slcg_gr;
}

void ga10b_slcg_ltc_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_ltc)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_ltc[i].addr;
			u32 val = prod ? ga10b_slcg_ltc[i].prod :
					 ga10b_slcg_ltc[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_ltc_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_ltc));
}

const struct gating_desc *ga10b_slcg_ltc_get_gating_prod(void)
{
	return ga10b_slcg_ltc;
}

void ga10b_slcg_perf_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_perf)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_perf[i].addr;
			u32 val = prod ? ga10b_slcg_perf[i].prod :
					 ga10b_slcg_perf[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_perf_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_perf));
}

const struct gating_desc *ga10b_slcg_perf_get_gating_prod(void)
{
	return ga10b_slcg_perf;
}

void ga10b_slcg_priring_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_priring)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_priring[i].addr;
			u32 val = prod ? ga10b_slcg_priring[i].prod :
					 ga10b_slcg_priring[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_priring_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_priring));
}

const struct gating_desc *ga10b_slcg_priring_get_gating_prod(void)
{
	return ga10b_slcg_priring;
}

void ga10b_slcg_rs_ctrl_fbp_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_rs_ctrl_fbp)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_rs_ctrl_fbp[i].addr;
			u32 val = prod ? ga10b_slcg_rs_ctrl_fbp[i].prod :
					 ga10b_slcg_rs_ctrl_fbp[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_rs_ctrl_fbp_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_rs_ctrl_fbp));
}

const struct gating_desc *ga10b_slcg_rs_ctrl_fbp_get_gating_prod(void)
{
	return ga10b_slcg_rs_ctrl_fbp;
}

void ga10b_slcg_rs_ctrl_gpc_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_rs_ctrl_gpc)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_rs_ctrl_gpc[i].addr;
			u32 val = prod ? ga10b_slcg_rs_ctrl_gpc[i].prod :
					 ga10b_slcg_rs_ctrl_gpc[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_rs_ctrl_gpc_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_rs_ctrl_gpc));
}

const struct gating_desc *ga10b_slcg_rs_ctrl_gpc_get_gating_prod(void)
{
	return ga10b_slcg_rs_ctrl_gpc;
}

void ga10b_slcg_rs_ctrl_sys_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_rs_ctrl_sys)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_rs_ctrl_sys[i].addr;
			u32 val = prod ? ga10b_slcg_rs_ctrl_sys[i].prod :
					 ga10b_slcg_rs_ctrl_sys[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_rs_ctrl_sys_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_rs_ctrl_sys));
}

const struct gating_desc *ga10b_slcg_rs_ctrl_sys_get_gating_prod(void)
{
	return ga10b_slcg_rs_ctrl_sys;
}

void ga10b_slcg_rs_fbp_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_rs_fbp)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_rs_fbp[i].addr;
			u32 val = prod ? ga10b_slcg_rs_fbp[i].prod :
					 ga10b_slcg_rs_fbp[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_rs_fbp_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_rs_fbp));
}

const struct gating_desc *ga10b_slcg_rs_fbp_get_gating_prod(void)
{
	return ga10b_slcg_rs_fbp;
}

void ga10b_slcg_rs_gpc_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_rs_gpc)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_rs_gpc[i].addr;
			u32 val = prod ? ga10b_slcg_rs_gpc[i].prod :
					 ga10b_slcg_rs_gpc[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_rs_gpc_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_rs_gpc));
}

const struct gating_desc *ga10b_slcg_rs_gpc_get_gating_prod(void)
{
	return ga10b_slcg_rs_gpc;
}

void ga10b_slcg_rs_sys_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_rs_sys)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_rs_sys[i].addr;
			u32 val = prod ? ga10b_slcg_rs_sys[i].prod :
					 ga10b_slcg_rs_sys[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_rs_sys_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_rs_sys));
}

const struct gating_desc *ga10b_slcg_rs_sys_get_gating_prod(void)
{
	return ga10b_slcg_rs_sys;
}

void ga10b_slcg_timer_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_timer)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_timer[i].addr;
			u32 val = prod ? ga10b_slcg_timer[i].prod :
					 ga10b_slcg_timer[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_timer_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_timer));
}

const struct gating_desc *ga10b_slcg_timer_get_gating_prod(void)
{
	return ga10b_slcg_timer;
}

void ga10b_slcg_pmu_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_pmu)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_pmu[i].addr;
			u32 val = prod ? ga10b_slcg_pmu[i].prod :
					 ga10b_slcg_pmu[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_pmu_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_pmu));
}

const struct gating_desc *ga10b_slcg_pmu_get_gating_prod(void)
{
	return ga10b_slcg_pmu;
}

void ga10b_slcg_therm_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_therm)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_therm[i].addr;
			u32 val = prod ? ga10b_slcg_therm[i].prod :
					 ga10b_slcg_therm[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_therm_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_therm));
}

const struct gating_desc *ga10b_slcg_therm_get_gating_prod(void)
{
	return ga10b_slcg_therm;
}

void ga10b_slcg_xbar_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_xbar)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_xbar[i].addr;
			u32 val = prod ? ga10b_slcg_xbar[i].prod :
					 ga10b_slcg_xbar[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_xbar_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_xbar));
}

const struct gating_desc *ga10b_slcg_xbar_get_gating_prod(void)
{
	return ga10b_slcg_xbar;
}

void ga10b_slcg_hshub_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_slcg_hshub)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_SLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_slcg_hshub[i].addr;
			u32 val = prod ? ga10b_slcg_hshub[i].prod :
					 ga10b_slcg_hshub[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_slcg_hshub_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_slcg_hshub));
}

const struct gating_desc *ga10b_slcg_hshub_get_gating_prod(void)
{
	return ga10b_slcg_hshub;
}

void ga10b_blcg_bus_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_blcg_bus)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_BLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_blcg_bus[i].addr;
			u32 val = prod ? ga10b_blcg_bus[i].prod :
					 ga10b_blcg_bus[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_blcg_bus_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_blcg_bus));
}

const struct gating_desc *ga10b_blcg_bus_get_gating_prod(void)
{
	return ga10b_blcg_bus;
}

void ga10b_blcg_ce_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_blcg_ce)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_BLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_blcg_ce[i].addr;
			u32 val = prod ? ga10b_blcg_ce[i].prod :
					 ga10b_blcg_ce[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_blcg_ce_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_blcg_ce));
}

const struct gating_desc *ga10b_blcg_ce_get_gating_prod(void)
{
	return ga10b_blcg_ce;
}

void ga10b_blcg_fb_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_blcg_fb)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_BLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_blcg_fb[i].addr;
			u32 val = prod ? ga10b_blcg_fb[i].prod :
					 ga10b_blcg_fb[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_blcg_fb_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_blcg_fb));
}

const struct gating_desc *ga10b_blcg_fb_get_gating_prod(void)
{
	return ga10b_blcg_fb;
}

void ga10b_blcg_runlist_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_blcg_runlist)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_BLCG)) {
		for (i = 0U; i < size; i++) {
			struct nvgpu_fifo *f = &g->fifo;
			struct nvgpu_runlist *runlist;
			u32 j, runlist_pri_base;
			u32 reg = ga10b_blcg_runlist[i].addr;
			u32 val = prod ? ga10b_blcg_runlist[i].prod :
					 ga10b_blcg_runlist[i].disable;
			for (j = 0U; j < f->num_runlists; j++) {
				runlist = &f->active_runlists[j];
				runlist_pri_base = runlist->runlist_pri_base;
				nvgpu_writel(g, nvgpu_safe_add_u32(reg, runlist_pri_base), val);
			}
		}
	}
}

u32 ga10b_blcg_runlist_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_blcg_runlist));
}

const struct gating_desc *ga10b_blcg_runlist_get_gating_prod(void)
{
	return ga10b_blcg_runlist;
}

void ga10b_blcg_gr_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_blcg_gr)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_BLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_blcg_gr[i].addr;
			u32 val = prod ? ga10b_blcg_gr[i].prod :
					 ga10b_blcg_gr[i].disable;
#ifdef CONFIG_NVGPU_MIG
			if ((nvgpu_is_enabled(g, NVGPU_SUPPORT_MIG)) &&
					(g->ops.gr.init.is_allowed_reg
						!= NULL) &&
					(!(g->ops.gr.init.is_allowed_reg(g,
						reg)))) {
				nvgpu_log(g, gpu_dbg_mig | gpu_dbg_gr,
					"(MIG) Skip blcg graphics gating reg "
						"index[%u] addr[%x] value[%x] ",
					i, reg, val);
				continue;
			}
#endif
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_blcg_gr_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_blcg_gr));
}

const struct gating_desc *ga10b_blcg_gr_get_gating_prod(void)
{
	return ga10b_blcg_gr;
}

void ga10b_blcg_ltc_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_blcg_ltc)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_BLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_blcg_ltc[i].addr;
			u32 val = prod ? ga10b_blcg_ltc[i].prod :
					 ga10b_blcg_ltc[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_blcg_ltc_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_blcg_ltc));
}

const struct gating_desc *ga10b_blcg_ltc_get_gating_prod(void)
{
	return ga10b_blcg_ltc;
}

void ga10b_blcg_pmu_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_blcg_pmu)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_BLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_blcg_pmu[i].addr;
			u32 val = prod ? ga10b_blcg_pmu[i].prod :
					 ga10b_blcg_pmu[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_blcg_pmu_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_blcg_pmu));
}

const struct gating_desc *ga10b_blcg_pmu_get_gating_prod(void)
{
	return ga10b_blcg_pmu;
}

void ga10b_blcg_xbar_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_blcg_xbar)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_BLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_blcg_xbar[i].addr;
			u32 val = prod ? ga10b_blcg_xbar[i].prod :
					 ga10b_blcg_xbar[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_blcg_xbar_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_blcg_xbar));
}

const struct gating_desc *ga10b_blcg_xbar_get_gating_prod(void)
{
	return ga10b_blcg_xbar;
}

void ga10b_blcg_hshub_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_blcg_hshub)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_BLCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_blcg_hshub[i].addr;
			u32 val = prod ? ga10b_blcg_hshub[i].prod :
					 ga10b_blcg_hshub[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_blcg_hshub_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_blcg_hshub));
}

const struct gating_desc *ga10b_blcg_hshub_get_gating_prod(void)
{
	return ga10b_blcg_hshub;
}

void ga10b_elcg_ce_load_gating_prod(struct gk20a *g,
	bool prod)
{
	u32 i;
	u32 size = nvgpu_safe_cast_u64_to_u32(sizeof(ga10b_elcg_ce)
							/ GATING_DESC_SIZE);

	if (nvgpu_is_enabled(g, NVGPU_GPU_CAN_ELCG)) {
		for (i = 0U; i < size; i++) {
			u32 reg = ga10b_elcg_ce[i].addr;
			u32 val = prod ? ga10b_elcg_ce[i].prod :
					 ga10b_elcg_ce[i].disable;
			nvgpu_writel(g, reg, val);
		}
	}
}

u32 ga10b_elcg_ce_gating_prod_size(void)
{
	return nvgpu_safe_cast_u64_to_u32(ARRAY_SIZE(ga10b_elcg_ce));
}

const struct gating_desc *ga10b_elcg_ce_get_gating_prod(void)
{
	return ga10b_elcg_ce;
}

