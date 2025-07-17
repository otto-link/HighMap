/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "FastNoiseLite.h"

#include "highmap/array.hpp"
#include "highmap/functions.hpp"
#include "highmap/operator.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array wave_dune(Vec2<int>    shape,
                float        kw,
                float        angle,
                float        xtop,
                float        xbottom,
                float        phase_shift,
                const Array *p_noise_x,
                const Array *p_noise_y,
                const Array *p_stretching,
                Vec4<float>  bbox)
{
  Array                  array = Array(shape);
  hmap::WaveDuneFunction f = hmap::WaveDuneFunction({kw, kw},
                                                    angle,
                                                    xtop,
                                                    xbottom,
                                                    phase_shift);

  fill_array_using_xy_function(array,
                               bbox,
                               nullptr,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_delegate());
  return array;
}

Array wave_sine(Vec2<int>    shape,
                float        kw,
                float        angle,
                float        phase_shift,
                const Array *p_noise_x,
                const Array *p_noise_y,
                const Array *p_stretching,
                Vec4<float>  bbox)
{
  Array                  array = Array(shape);
  hmap::WaveSineFunction f = hmap::WaveSineFunction({kw, kw},
                                                    angle,
                                                    phase_shift);

  fill_array_using_xy_function(array,
                               bbox,
                               nullptr,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_delegate());
  return array;
}

Array wave_square(Vec2<int>    shape,
                  float        kw,
                  float        angle,
                  float        phase_shift,
                  const Array *p_noise_x,
                  const Array *p_noise_y,
                  const Array *p_stretching,
                  Vec4<float>  bbox)
{
  Array                    array = Array(shape);
  hmap::WaveSquareFunction f = hmap::WaveSquareFunction({kw, kw},
                                                        angle,
                                                        phase_shift);

  fill_array_using_xy_function(array,
                               bbox,
                               nullptr,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_delegate());
  return array;
}

Array wave_triangular(Vec2<int>    shape,
                      float        kw,
                      float        angle,
                      float        slant_ratio,
                      float        phase_shift,
                      const Array *p_noise_x,
                      const Array *p_noise_y,
                      const Array *p_stretching,
                      Vec4<float>  bbox)
{
  Array                        array = Array(shape);
  hmap::WaveTriangularFunction f = hmap::WaveTriangularFunction({kw, kw},
                                                                angle,
                                                                slant_ratio,
                                                                phase_shift);

  fill_array_using_xy_function(array,
                               bbox,
                               nullptr,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_delegate());
  return array;
}

} // namespace hmap
