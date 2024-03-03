/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "FastNoiseLite.h"

#include "highmap/array.hpp"
#include "highmap/noise_function.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

// --- (x, y) function definitions

WorleyFunction::WorleyFunction(Vec2<float> kw,
                               uint        seed,
                               bool        return_cell_value)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(1.f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
  this->noise.SetCellularJitter(1.f);

  if (return_cell_value)
    this->noise.SetCellularReturnType(
        FastNoiseLite::CellularReturnType_CellValue);
  else
    this->noise.SetCellularReturnType(
        FastNoiseLite::CellularReturnType_Distance);

  this->function = [this](float x, float y, float)
  { return this->noise.GetNoise(this->kw.x * x, this->kw.y * y); };
}

WorleyDoubleFunction::WorleyDoubleFunction(Vec2<float> kw,
                                           uint        seed,
                                           float       ratio,
                                           float       k)
    : NoiseFunction(kw, seed), ratio(ratio), k(k)
{
  this->set_seed(seed);

  this->noise1.SetFrequency(1.0f);
  this->noise2.SetFrequency(1.0f);

  this->noise1.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
  this->noise2.SetNoiseType(FastNoiseLite::NoiseType_Cellular);

  this->function = [this](float x, float y, float)
  {
    float w1 = this->noise1.GetNoise(this->kw.x * x, this->kw.y * y);
    float w2 = this->noise2.GetNoise(this->kw.x * x, this->kw.y * y);
    if (this->k)
      return maximum_smooth(this->ratio * w1,
                            (1.f - this->ratio) * w2,
                            this->k);
    else
      return std::max(this->ratio * w1, (1.f - this->ratio) * w2);
  };
}

// --- wrappers for array filling

hmap::Array worley(Vec2<int>   shape,
                   Vec2<float> kw,
                   uint        seed,
                   Array      *p_noise_x,
                   Array      *p_noise_y,
                   Array      *p_stretching,
                   Vec2<float> shift,
                   Vec2<float> scale)
{
  hmap::Array          array = hmap::Array(shape);
  hmap::WorleyFunction f = hmap::WorleyFunction(kw, seed, false);
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

Array worley_double(Vec2<int>   shape,
                    Vec2<float> kw,
                    uint        seed,
                    float       ratio,
                    float       k,
                    Array      *p_noise_x,
                    Array      *p_noise_y,
                    Array      *p_stretching,
                    Vec2<float> shift,
                    Vec2<float> scale)
{
  hmap::Array                array = hmap::Array(shape);
  hmap::WorleyDoubleFunction f = hmap::WorleyDoubleFunction(kw, seed, ratio, k);
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

hmap::Array worley_value(Vec2<int>   shape,
                         Vec2<float> kw,
                         uint        seed,
                         Array      *p_noise_x,
                         Array      *p_noise_y,
                         Array      *p_stretching,
                         Vec2<float> shift,
                         Vec2<float> scale)
{
  hmap::Array          array = hmap::Array(shape);
  hmap::WorleyFunction f = hmap::WorleyFunction(kw, seed, true);
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

} // namespace hmap
