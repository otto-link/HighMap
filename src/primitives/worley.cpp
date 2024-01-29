/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "FastNoiseLite.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

hmap::Array worley(Vec2<int>   shape,
                   Vec2<float> kw,
                   uint        seed,
                   Array      *p_noise_x,
                   Array      *p_noise_y,
                   Vec2<float> shift,
                   Vec2<float> scale)
{
  hmap::Array   array = hmap::Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
  noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
  noise.SetCellularJitter(1.f);

  std::vector<float> x = linspace(kw.x * shift.x,
                                  kw.x * (shift.x + scale.x),
                                  array.shape.x,
                                  false);
  std::vector<float> y = linspace(kw.y * shift.y,
                                  kw.y * (shift.y + scale.y),
                                  array.shape.y,
                                  false);

  helper_get_noise(array,
                   x,
                   y,
                   p_noise_x,
                   p_noise_y,
                   [&noise](float x_, float y_)
                   { return noise.GetNoise(x_, y_); });
  return array;
}

Array worley_double(Vec2<int>   shape,
                    Vec2<float> kw,
                    uint        seed,
                    float       ratio,
                    float       k,
                    Array      *p_noise_x,
                    Array      *p_noise_y,
                    Vec2<float> shift,
                    Vec2<float> scale)
{
  Array w1 = worley(shape, kw, seed++, p_noise_x, p_noise_y, shift, scale);
  Array w2 = worley(shape, kw, seed++, p_noise_y, p_noise_x, shift, scale);
  Array z;

  if (k > 0.f)
    z = maximum_smooth(ratio * w1, (1.f - ratio) * w2, k);
  else
    z = maximum(ratio * w1, (1.f - ratio) * w2);

  return z;
}

hmap::Array worley_polyline(Vec2<int>   shape,
                            float       kw,
                            uint        seed,
                            float       decay,
                            Array      *p_noise_x,
                            Array      *p_noise_y,
                            Vec2<float> shift,
                            Vec2<float> scale)
{
  hmap::Array array = hmap::Array(shape);

  // create a random path
  hmap::Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f};

  int  npoints = 2 * (int)kw;
  Path path = hmap::Path(npoints, seed, bbox);
  path.reorder_nns();

  std::vector<float> x =
      linspace(shift.x, shift.x + scale.x, array.shape.x, false);
  std::vector<float> y =
      linspace(shift.y, shift.y + scale.y, array.shape.y, false);

  helper_get_noise(array,
                   x,
                   y,
                   p_noise_x,
                   p_noise_y,
                   [&path, &decay](float x_, float y_)
                   {
                     float h = path.sdf_open(x_, y_);
                     h = std::exp(-1.386f * decay * h);
                     return h;
                   });

  return array;
}

hmap::Array worley_value(Vec2<int>   shape,
                         Vec2<float> kw,
                         uint        seed,
                         Array      *p_noise_x,
                         Array      *p_noise_y,
                         Vec2<float> shift,
                         Vec2<float> scale)
{
  hmap::Array   array = hmap::Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
  noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
  noise.SetCellularJitter(1.f);

  std::vector<float> x = linspace(kw.x * shift.x,
                                  kw.x * (shift.x + scale.x),
                                  array.shape.x,
                                  false);
  std::vector<float> y = linspace(kw.y * shift.y,
                                  kw.y * (shift.y + scale.y),
                                  array.shape.y,
                                  false);

  helper_get_noise(array,
                   x,
                   y,
                   p_noise_x,
                   p_noise_y,
                   [&noise](float x_, float y_)
                   { return noise.GetNoise(x_, y_); });
  return array;
}

} // namespace hmap
