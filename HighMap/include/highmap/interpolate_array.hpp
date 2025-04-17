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
#include "highmap/coord_frame.hpp"

namespace hmap
{

// in-place source1 <= source1 & source2
void flatten_heightmap(Heightmap        &h_source1,
                       const Heightmap  &h_source2,
                       const CoordFrame &t_source1,
                       const CoordFrame &t_source2);

// input kept target <= source1 & source2
void flatten_heightmap(const Heightmap  &h_source1,
                       const Heightmap  &h_source2,
                       Heightmap        &h_target,
                       const CoordFrame &t_source1,
                       const CoordFrame &t_source2,
                       const CoordFrame &t_target);

void flatten_heightmap(const std::vector<const Heightmap *>  &h_sources,
                       Heightmap                             &h_target,
                       const std::vector<const CoordFrame *> &t_sources,
                       const CoordFrame                      &t_target);

void interpolate_array_bicubic(const Array &source, Array &target);

void interpolate_array_bicubic(const Array       &source,
                               Array             &target,
                               const Vec4<float> &bbox_source,
                               const Vec4<float> &bbox_target);

void interpolate_array_bilinear(const Array &source, Array &target);

void interpolate_array_bilinear(const Array       &source,
                                Array             &target,
                                const Vec4<float> &bbox_source,
                                const Vec4<float> &bbox_target);

void interpolate_array_nearest(const Array &source, Array &target);

void interpolate_array_nearest(const Array       &source,
                               Array             &target,
                               const Vec4<float> &bbox_source,
                               const Vec4<float> &bbox_target);

void interpolate_heightmap(const hmap::Heightmap &h_source,
                           hmap::Heightmap       &h_target,
                           const CoordFrame      &t_source,
                           const CoordFrame      &t_target);

} // namespace hmap

namespace hmap::gpu
{

void interpolate_array_bicubic(const Array &source, Array &target);

void interpolate_array_bicubic(const Array       &source,
                               Array             &target,
                               const Vec4<float> &bbox_source,
                               const Vec4<float> &bbox_target);

void interpolate_array_bilinear(const Array &source, Array &target);

void interpolate_array_bilinear(const Array       &source,
                                Array             &target,
                                const Vec4<float> &bbox_source,
                                const Vec4<float> &bbox_target);

void interpolate_array_lagrange(const Array &source, Array &target, int order);

void interpolate_array_nearest(const Array &source, Array &target);

void interpolate_array_nearest(const Array       &source,
                               Array             &target,
                               const Vec4<float> &bbox_source,
                               const Vec4<float> &bbox_target);

} // namespace hmap::gpu