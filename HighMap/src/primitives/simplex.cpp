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

Simplex2Function::Simplex2Function(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(0.5f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

  this->function = [this](float x, float y, float)
  { return this->noise.GetNoise(this->kw.x * x, this->kw.y * y); };
}

Simplex2SFunction::Simplex2SFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(0.5f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);

  this->function = [this](float x, float y, float)
  { return this->noise.GetNoise(this->kw.x * x, this->kw.y * y); };
}

// --- wrappers for array filling

Array simplex(Vec2<int>   shape,
              Vec2<float> kw,
              uint        seed,
              Array      *p_noise_x,
              Array      *p_noise_y,
              Array      *p_stretching,
              Vec2<float> shift,
              Vec2<float> scale)
{
  Array                  array = Array(shape);
  hmap::Simplex2Function f = hmap::Simplex2Function(kw, seed);
  std::vector<float>     x, y;
  hmap::Vec4<float>      bbox = {0.f + shift.x,
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
