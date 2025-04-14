/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file interpolator_array.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2025 Otto Link
 */
#pragma once
#include "highmap/array.hpp"

namespace hmap
{

void interpolate_array_bicubic(const Array &source, Array &target);

void interpolate_array_bilinear(const Array &source, Array &target);

void interpolate_array_nearest(const Array &source, Array &target);

} // namespace hmap

namespace hmap::gpu
{

void interpolate_array_bicubic(const Array &source, Array &target);

void interpolate_array_bilinear(const Array &source, Array &target);

void interpolate_array_lagrange(const Array &source, Array &target, int order);

void interpolate_array_nearest(const Array &source, Array &target);

} // namespace hmap::gpu