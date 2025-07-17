/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <memory>

#include "NoiseLib/include/noise.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/internal/dendry_array_control_function.hpp"
#include "highmap/operator.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array dendry(Vec2<int>    shape,
             Vec2<float>  kw,
             uint         seed,
             Array       &control_array,
             float        eps,
             int          resolution,
             float        displacement,
             int          primitives_resolution_steps,
             float        slope_power,
             float        noise_amplitude_proportion,
             bool         add_control_function,
             float        control_function_overlap,
             const Array *p_noise_x,
             const Array *p_noise_y,
             const Array *p_stretching,
             Vec4<float>  bbox,
             int          subsampling)
{
  Array array = Array(shape);

  int nbuffer = (int)(control_function_overlap * control_array.shape.x);

  Vec4<int> buffers = {nbuffer, nbuffer, nbuffer, nbuffer};
  Array     control_array_buffered = generate_buffered_array(control_array,
                                                         buffers);

  std::unique_ptr<ArrayControlFunction> control_function(
      std::make_unique<ArrayControlFunction>(control_array_buffered));

  const Point2D noise_top_left(0.f, 0.f);
  const Point2D noise_bottom_right(kw.x, kw.x);

  const Point2D control_function_top_left(0.5f * control_function_overlap,
                                          0.5f * control_function_overlap);
  const Point2D control_function_bottom_right(
      1.f - 0.5f * control_function_overlap,
      1.f - 0.5f * control_function_overlap);

  const Noise<ArrayControlFunction> noise(std::move(control_function),
                                          noise_top_left,
                                          noise_bottom_right,
                                          control_function_top_left,
                                          control_function_bottom_right,
                                          seed,
                                          eps,
                                          resolution,
                                          displacement,
                                          primitives_resolution_steps,
                                          slope_power,
                                          noise_amplitude_proportion,
                                          add_control_function,
                                          false,
                                          false,
                                          false,
                                          false);

  fill_array_using_xy_function(
      array,
      bbox,
      nullptr,
      p_noise_x,
      p_noise_y,
      nullptr,
      [&noise, &kw](float x, float y, float)
      { return noise.evaluateTerrain(kw.x * x, kw.y * y); },
      subsampling);

  return array;
}

Array dendry(Vec2<int>      shape,
             Vec2<float>    kw,
             uint           seed,
             NoiseFunction &noise_function,
             float          noise_function_offset,
             float          noise_function_scaling,
             float          eps,
             int            resolution,
             float          displacement,
             int            primitives_resolution_steps,
             float          slope_power,
             float          noise_amplitude_proportion,
             bool           add_control_function,
             float          control_function_overlap,
             const Array   *p_noise_x,
             const Array   *p_noise_y,
             const Array   *p_stretching,
             Vec4<float>    bbox)
{
  Array array = Array(shape);

  std::unique_ptr<XyControlFunction> control_function(
      std::make_unique<XyControlFunction>(noise_function,
                                          noise_function_offset,
                                          noise_function_scaling));

  const Point2D noise_top_left(0.f, 0.f);
  const Point2D noise_bottom_right(kw.x, kw.x);
  const Point2D control_function_top_left(0.f, 0.f);
  const Point2D control_function_bottom_right(1.f, 1.f);

  const Noise<XyControlFunction> noise(std::move(control_function),
                                       noise_top_left,
                                       noise_bottom_right,
                                       control_function_top_left,
                                       control_function_bottom_right,
                                       seed,
                                       eps,
                                       resolution,
                                       displacement,
                                       primitives_resolution_steps,
                                       slope_power,
                                       noise_amplitude_proportion,
                                       add_control_function,
                                       false,
                                       false,
                                       false,
                                       false);

  fill_array_using_xy_function(
      array,
      bbox,
      nullptr,
      p_noise_x,
      p_noise_y,
      nullptr,
      [&noise, &kw](float x, float y, float)
      { return noise.evaluateTerrain(kw.x * x, kw.y * y); });

  return array;
}

} // namespace hmap
