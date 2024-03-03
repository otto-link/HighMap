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

// --- (x, y) function definitions

PerlinFunction::PerlinFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(1.f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  this->function = [this](float x, float y, float)
  { return this->noise.GetNoise(this->kw.x * x, this->kw.y * y); };
}

PerlinBillowFunction::PerlinBillowFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(1.f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  this->function = [this](float x, float y, float)
  {
    float value = this->noise.GetNoise(this->kw.x * x, this->kw.y * y);
    return 2.f * std::abs(value) - 1.f;
  };
}

PerlinHalfFunction::PerlinHalfFunction(Vec2<float> kw, uint seed, float k)
    : NoiseFunction(kw, seed), k(k)
{
  this->set_seed(seed);
  this->noise.SetFrequency(1.f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  this->function = [this](float x, float y, float)
  {
    float value = this->noise.GetNoise(this->kw.x * x, this->kw.y * y);
    return clamp_min_smooth(value, 0.f, this->k);
  };
}

PerlinMixFunction::PerlinMixFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(1.f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  this->function = [this](float x, float y, float)
  {
    float value = this->noise.GetNoise(this->kw.x * x, this->kw.y * y);
    return 0.5f * value + std::abs(value) - 0.5f;
  };
}

// --- wrappers for array filling

Array perlin(Vec2<int>   shape,
             Vec2<float> kw,
             uint        seed,
             Array      *p_noise_x,
             Array      *p_noise_y,
             Array      *p_stretching,
             Vec2<float> shift,
             Vec2<float> scale)
{
  Array                array = Array(shape);
  hmap::PerlinFunction f = hmap::PerlinFunction(kw, seed);
  std::vector<float>   x, y;
  hmap::Vec4<float>    bbox = {0.f + shift.x,
                               scale.x + shift.x,
                               0.f + shift.y,
                               scale.y + shift.y};

  grid_xy_vector(x, y, shape, bbox);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

Array perlin_billow(Vec2<int>   shape,
                    Vec2<float> kw,
                    uint        seed,
                    Array      *p_noise_x,
                    Array      *p_noise_y,
                    Array      *p_stretching,
                    Vec2<float> shift,
                    Vec2<float> scale)
{
  Array                      array = Array(shape);
  hmap::PerlinBillowFunction f = hmap::PerlinBillowFunction(kw, seed);
  std::vector<float>         x, y;
  hmap::Vec4<float>          bbox = {0.f + shift.x,
                                     scale.x + shift.x,
                                     0.f + shift.y,
                                     scale.y + shift.y};

  grid_xy_vector(x, y, shape, bbox);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

Array perlin_half(Vec2<int>   shape,
                  Vec2<float> kw,
                  uint        seed,
                  float       k,
                  Array      *p_noise_x,
                  Array      *p_noise_y,
                  Array      *p_stretching,
                  Vec2<float> shift,
                  Vec2<float> scale)
{
  Array                    array = Array(shape);
  hmap::PerlinHalfFunction f = hmap::PerlinHalfFunction(kw, seed, k);
  std::vector<float>       x, y;
  hmap::Vec4<float>        bbox = {0.f + shift.x,
                                   scale.x + shift.x,
                                   0.f + shift.y,
                                   scale.y + shift.y};

  grid_xy_vector(x, y, shape, bbox);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

Array perlin_mix(Vec2<int>   shape,
                 Vec2<float> kw,
                 uint        seed,
                 Array      *p_noise_x,
                 Array      *p_noise_y,
                 Array      *p_stretching,
                 Vec2<float> shift,
                 Vec2<float> scale)
{
  Array                   array = Array(shape);
  hmap::PerlinMixFunction f = hmap::PerlinMixFunction(kw, seed);
  std::vector<float>      x, y;
  hmap::Vec4<float>       bbox = {0.f + shift.x,
                                  scale.x + shift.x,
                                  0.f + shift.y,
                                  scale.y + shift.y};

  grid_xy_vector(x, y, shape, bbox);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

} // namespace hmap
