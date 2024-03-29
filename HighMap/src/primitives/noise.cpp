/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "FastNoiseLite.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/noise_function.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array noise(NoiseType   noise_type,
            Vec2<int>   shape,
            Vec2<float> kw,
            uint        seed,
            Array      *p_noise_x,
            Array      *p_noise_y,
            Array      *p_stretching,
            Vec4<float> bbox)
{
  Array                          array = Array(shape);
  std::unique_ptr<NoiseFunction> p = create_noise_function_from_type(noise_type,
                                                                     kw,
                                                                     seed);

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               p.get()->get_function());
  return array;
}

Array noise_fbm(NoiseType   noise_type,
                Vec2<int>   shape,
                Vec2<float> kw,
                uint        seed,
                int         octaves,
                float       weight,
                float       persistence,
                float       lacunarity,
                Array      *p_base_elevation,
                Array      *p_noise_x,
                Array      *p_noise_y,
                Array      *p_stretching,
                Vec4<float> bbox)
{
  Array array = p_base_elevation == nullptr ? Array(shape) : *p_base_elevation;

  std::unique_ptr<NoiseFunction> p = create_noise_function_from_type(noise_type,
                                                                     kw,
                                                                     seed);

  hmap::FbmFunction f = hmap::FbmFunction(p.get()->get_base_ref(),
                                          octaves,
                                          weight,
                                          persistence,
                                          lacunarity);

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

Array noise_iq(NoiseType   noise_type,
               Vec2<int>   shape,
               Vec2<float> kw,
               uint        seed,
               int         octaves,
               float       weight,
               float       persistence,
               float       lacunarity,
               float       gradient_scale,
               Array      *p_base_elevation,
               Array      *p_noise_x,
               Array      *p_noise_y,
               Array      *p_stretching,
               Vec4<float> bbox)
{
  Array array = p_base_elevation == nullptr ? Array(shape) : *p_base_elevation;

  std::unique_ptr<NoiseFunction> p = create_noise_function_from_type(noise_type,
                                                                     kw,
                                                                     seed);

  hmap::FbmIqFunction f = hmap::FbmIqFunction(p.get()->get_base_ref(),
                                              octaves,
                                              weight,
                                              persistence,
                                              lacunarity,
                                              gradient_scale);

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

Array noise_jordan(NoiseType   noise_type,
                   Vec2<int>   shape,
                   Vec2<float> kw,
                   uint        seed,
                   int         octaves,
                   float       weight,
                   float       persistence,
                   float       lacunarity,
                   float       warp0,
                   float       damp0,
                   float       warp_scale,
                   float       damp_scale,
                   Array      *p_base_elevation,
                   Array      *p_noise_x,
                   Array      *p_noise_y,
                   Array      *p_stretching,
                   Vec4<float> bbox)
{
  Array array = p_base_elevation == nullptr ? Array(shape) : *p_base_elevation;

  std::unique_ptr<NoiseFunction> p = create_noise_function_from_type(noise_type,
                                                                     kw,
                                                                     seed);

  hmap::FbmJordanFunction f = hmap::FbmJordanFunction(p.get()->get_base_ref(),
                                                      octaves,
                                                      weight,
                                                      persistence,
                                                      lacunarity,
                                                      warp0,
                                                      damp0,
                                                      warp_scale,
                                                      damp_scale);

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

Array noise_pingpong(NoiseType   noise_type,
                     Vec2<int>   shape,
                     Vec2<float> kw,
                     uint        seed,
                     int         octaves,
                     float       weight,
                     float       persistence,
                     float       lacunarity,
                     Array      *p_base_elevation,
                     Array      *p_noise_x,
                     Array      *p_noise_y,
                     Array      *p_stretching,
                     Vec4<float> bbox)
{
  Array array = p_base_elevation == nullptr ? Array(shape) : *p_base_elevation;

  std::unique_ptr<NoiseFunction> p = create_noise_function_from_type(noise_type,
                                                                     kw,
                                                                     seed);

  hmap::FbmPingpongFunction f = hmap::FbmPingpongFunction(
      p.get()->get_base_ref(),
      octaves,
      weight,
      persistence,
      lacunarity);

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

Array noise_ridged(NoiseType   noise_type,
                   Vec2<int>   shape,
                   Vec2<float> kw,
                   uint        seed,
                   int         octaves,
                   float       weight,
                   float       persistence,
                   float       lacunarity,
                   float       k_smoothing,
                   Array      *p_base_elevation,
                   Array      *p_noise_x,
                   Array      *p_noise_y,
                   Array      *p_stretching,
                   Vec4<float> bbox)
{
  Array array = p_base_elevation == nullptr ? Array(shape) : *p_base_elevation;

  std::unique_ptr<NoiseFunction> p = create_noise_function_from_type(noise_type,
                                                                     kw,
                                                                     seed);

  hmap::FbmRidgedFunction f = hmap::FbmRidgedFunction(p.get()->get_base_ref(),
                                                      octaves,
                                                      weight,
                                                      persistence,
                                                      lacunarity,
                                                      k_smoothing);

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

Array noise_swiss(NoiseType   noise_type,
                  Vec2<int>   shape,
                  Vec2<float> kw,
                  uint        seed,
                  int         octaves,
                  float       weight,
                  float       persistence,
                  float       lacunarity,
                  float       warp_scale,
                  Array      *p_base_elevation,
                  Array      *p_noise_x,
                  Array      *p_noise_y,
                  Array      *p_stretching,
                  Vec4<float> bbox)
{
  Array array = p_base_elevation == nullptr ? Array(shape) : *p_base_elevation;

  std::unique_ptr<NoiseFunction> p = create_noise_function_from_type(noise_type,
                                                                     kw,
                                                                     seed);

  hmap::FbmSwissFunction f = hmap::FbmSwissFunction(p.get()->get_base_ref(),
                                                    octaves,
                                                    weight,
                                                    persistence,
                                                    lacunarity,
                                                    warp_scale);

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

} // namespace hmap
