/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file math.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#ifdef ENABLE_OPENCL

#include "cl_wrapper.hpp"

#include "highmap/array.hpp"

namespace hmap::gpu
{

bool init_opencl();

// --- function wrappers

// clang-format off
void expand(Array &array, int ir);
void expand(Array &array, int ir, Array *p_mask);
void expand(Array &array, Array &kernel);
void expand(Array &array, Array &kernel, Array *p_mask);

Array gradient_norm(const Array &array);

void laplace(Array &array, float sigma = 0.2f, int iterations = 3);
void laplace(Array &array, Array *p_mask, float sigma = 0.2f, int iterations = 3);

Array maximum_local_disk(const Array &array, int ir);

Array maximum_smooth(const Array &array1, const Array &array2, float k = 0.2f);

void median_3x3(Array &array);

Array minimum_local_disk(const Array &array, int ir);

Array minimum_smooth(const Array &array1, const Array &array2, float k = 0.2f);

Array rugosity(const Array &z, int ir, bool convex = true);

void shrink(Array &array, int ir);
void shrink(Array &array, int ir, Array *p_mask);       
void shrink(Array &array, Array &kernel);              
void shrink(Array &array, Array &kernel, Array *p_mask);

void smooth_cpulse(Array &array, int ir);
void smooth_cpulse(Array &array, int ir, Array *p_mask);

void smooth_fill(Array &array, int ir, float k = 0.1f, Array *p_deposition_map = nullptr);
void smooth_fill(Array &array, int ir, Array *p_mask, float k = 0.1f, Array *p_deposition_map = nullptr);

void thermal(Array &z, const Array &talus, int iterations = 10);

void thermal_bf(Array &z, const Array &talus, int iterations);

void warp(Array &array, Array *p_dx, Array *p_dy);
// clang-format on

} // namespace hmap::gpu

#endif
