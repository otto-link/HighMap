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

void avg(Array &array, int ir);

void median_3x3(Array &array);

} // namespace hmap::gpu

#endif