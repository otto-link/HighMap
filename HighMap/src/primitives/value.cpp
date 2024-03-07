/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "FastNoiseLite.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/interpolate.hpp"
#include "highmap/noise_function.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"
#include "highmap/vector.hpp"

namespace hmap
{

Array value_noise(Vec2<int>   shape,
                  Vec2<float> kw,
                  uint        seed,
                  Array      *p_noise_x,
                  Array      *p_noise_y,
                  Array      *p_stretching,
                  Vec4<float> bbox)
{
  Array                    array = Array(shape);
  hmap::ValueNoiseFunction f = hmap::ValueNoiseFunction(kw, seed);

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

Array value_noise_delaunay(Vec2<int>   shape,
                           Vec2<float> kw,
                           uint        seed,
                           Array      *p_noise_x,
                           Array      *p_noise_y,
                           Array      *p_stretching,
                           Vec4<float> bbox)
{
  Array                            array = Array(shape);
  hmap::ValueDelaunayNoiseFunction f = hmap::ValueDelaunayNoiseFunction(kw,
                                                                        seed);

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

Array value_noise_linear(Vec2<int>   shape,
                         Vec2<float> kw,
                         uint        seed,
                         Array      *p_noise_x,
                         Array      *p_noise_y,
                         Array      *p_stretching,
                         Vec4<float> bbox)
{
  Array                          array = Array(shape);
  hmap::ValueLinearNoiseFunction f = hmap::ValueLinearNoiseFunction(kw, seed);

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

Array value_noise_thinplate(Vec2<int>   shape,
                            Vec2<float> kw,
                            uint        seed,
                            Array      *p_noise_x,
                            Array      *p_noise_y,
                            Array      *p_stretching,
                            Vec4<float> bbox)
{
  Array                             array = Array(shape);
  hmap::ValueThinplateNoiseFunction f = hmap::ValueThinplateNoiseFunction(kw,
                                                                          seed);

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

} // namespace hmap
