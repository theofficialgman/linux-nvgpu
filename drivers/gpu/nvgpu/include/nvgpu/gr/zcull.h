/*
 * Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
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

#ifndef NVGPU_GR_ZCULL_H
#define NVGPU_GR_ZCULL_H

#include <nvgpu/types.h>

struct gk20a;
struct nvgpu_gr_config;

struct nvgpu_gr_zcull {
	struct gk20a *g;

	u32 aliquot_width;
	u32 aliquot_height;
	u32 aliquot_size;
	u32 total_aliquots;

	u32 width_align_pixels;
	u32 height_align_pixels;
	u32 pixel_squares_by_aliquots;

	u32 zcull_ctxsw_image_size;
};

struct nvgpu_gr_zcull_info {
	u32 width_align_pixels;
	u32 height_align_pixels;
	u32 pixel_squares_by_aliquots;
	u32 aliquot_total;
	u32 region_byte_multiplier;
	u32 region_header_size;
	u32 subregion_header_size;
	u32 subregion_width_align_pixels;
	u32 subregion_height_align_pixels;
	u32 subregion_count;
};

int nvgpu_gr_zcull_init(struct gk20a *g, struct nvgpu_gr_zcull **gr_zcull);
void nvgpu_gr_zcull_deinit(struct gk20a *g, struct nvgpu_gr_zcull *gr_zcull);

u32 nvgpu_gr_get_ctxsw_zcull_size(struct gk20a *g,
				struct nvgpu_gr_zcull *gr_zcull);
int nvgpu_gr_zcull_init_hw(struct gk20a *g,
			struct nvgpu_gr_zcull *gr_zcull,
			struct nvgpu_gr_config *gr_config);

#endif /* NVGPU_GR_ZCULL_H */