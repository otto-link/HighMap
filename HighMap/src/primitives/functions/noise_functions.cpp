/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "FastNoiseLite.h"
#include "libInterpolate/AnyInterpolator.hpp"
#include "libInterpolate/Interpolate.hpp"

#include "highmap/functions.hpp"
#include "highmap/math.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

//----------------------------------------------------------------------
// derived from NoiseFunction class
//----------------------------------------------------------------------

PerlinFunction::PerlinFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(1.f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  this->set_delegate(
      [this](float x, float y, float)
      { return this->noise.GetNoise(this->kw.x * x, this->kw.y * y); });
}

PerlinBillowFunction::PerlinBillowFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(1.f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  this->set_delegate(
      [this](float x, float y, float)
      {
        float value = this->noise.GetNoise(this->kw.x * x, this->kw.y * y);
        return 2.f * std::abs(value) - 1.f;
      });
}

PerlinHalfFunction::PerlinHalfFunction(Vec2<float> kw, uint seed, float k)
    : NoiseFunction(kw, seed), k(k)
{
  this->set_seed(seed);
  this->noise.SetFrequency(1.f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  this->set_delegate(
      [this](float x, float y, float)
      {
        float value = this->noise.GetNoise(this->kw.x * x, this->kw.y * y);
        return clamp_min_smooth(value, 0.f, this->k);
      });
}

PerlinMixFunction::PerlinMixFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(1.f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  this->set_delegate(
      [this](float x, float y, float)
      {
        float value = this->noise.GetNoise(this->kw.x * x, this->kw.y * y);
        return 0.5f * value + std::abs(value) - 0.5f;
      });
}

Simplex2Function::Simplex2Function(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(0.5f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

  this->set_delegate(
      [this](float x, float y, float)
      { return this->noise.GetNoise(this->kw.x * x, this->kw.y * y); });
}

Simplex2SFunction::Simplex2SFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(0.5f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);

  this->set_delegate(
      [this](float x, float y, float)
      { return this->noise.GetNoise(this->kw.x * x, this->kw.y * y); });
}

ValueNoiseFunction::ValueNoiseFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(1.f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_Value);

  this->set_delegate(
      [this](float x, float y, float)
      { return this->noise.GetNoise(this->kw.x * x, this->kw.y * y); });
}

ValueCubicNoiseFunction::ValueCubicNoiseFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(1.f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);

  this->set_delegate(
      [this](float x, float y, float)
      { return this->noise.GetNoise(this->kw.x * x, this->kw.y * y); });
}

ValueDelaunayNoiseFunction::ValueDelaunayNoiseFunction(Vec2<float> kw,
                                                       uint        seed)
    : NoiseFunction(kw, seed)
{
  this->set_kw(kw);
  this->set_seed(seed);

  this->set_delegate([this](float x, float y, float)
                     { return this->interp(x, y); });
}

void ValueDelaunayNoiseFunction::update_interpolation_function()
{
  // generate 'n' random grid points
  int n = (int)(this->kw.x * this->kw.y);

  std::vector<float> x(n);
  std::vector<float> y(n);
  std::vector<float> value(n);

  random_grid(x, y, value, this->seed, {0.f, 1.f, 0.f, 1.f});
  expand_grid(x, y, value, {0.f, 1.f, 0.f, 1.f});
  this->interp = _2D::LinearDelaunayTriangleInterpolator<float>();
  this->interp.setData(x, y, value);
}

ValueLinearNoiseFunction::ValueLinearNoiseFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_kw(kw);
  this->set_seed(seed);

  this->set_delegate([this](float x, float y, float)
                     { return this->interp(x, y); });
}

void ValueLinearNoiseFunction::update_interpolation_function()
{
  // generate random values on a regular coarse grid (adjust extent
  // according to the input noise in order to avoid "holes" in the
  // data for large noise displacement)
  Vec4<float> bbox = {-1.f, 2.f, -1.f, 2.f}; // bounding box

  float lx = bbox.b - bbox.a;
  float ly = bbox.d - bbox.c;

  Vec2<int> shape_base = Vec2<int>((int)(this->kw.x * lx) + 1,
                                   (int)(this->kw.y * ly) + 1);

  std::vector<float> value;
  value.reserve(shape_base.x * shape_base.y);

  {
    std::mt19937                          gen(this->seed);
    std::uniform_real_distribution<float> dis(0.f, 1.f);
    for (int k = 0; k < shape_base.x * shape_base.y; k++)
      value.push_back(dis(gen));
  }

  // corresponding grids
  Array xv = Array(shape_base);
  Array yv = Array(shape_base);

  for (int i = 0; i < shape_base.x; i++)
    for (int j = 0; j < shape_base.y; j++)
    {
      xv(i, j) = bbox.a + lx * (float)i / (float)(shape_base.x - 1);
      yv(i, j) = bbox.c + ly * (float)j / (float)(shape_base.y - 1);
    }

  this->interp = _2D::BilinearInterpolator<float>();
  this->interp.setData(xv.vector, yv.vector, value);
}

ValueThinplateNoiseFunction::ValueThinplateNoiseFunction(Vec2<float> kw,
                                                         uint        seed)
    : NoiseFunction(kw, seed)
{
  this->set_kw(kw);
  this->set_seed(seed);

  this->set_delegate([this](float x, float y, float)
                     { return this->interp(x, y); });
}

void ValueThinplateNoiseFunction::update_interpolation_function()
{
  // generate 'n' random grid points
  int n = (int)(this->kw.x * this->kw.y);

  std::vector<float> x(n);
  std::vector<float> y(n);
  std::vector<float> value(n);

  random_grid(x, y, value, this->seed, {0.f, 1.f, 0.f, 1.f});
  expand_grid(x, y, value, {0.f, 1.f, 0.f, 1.f});
  this->interp = _2D::ThinPlateSplineInterpolator<float>();
  this->interp.setData(x, y, value);
}

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

  this->set_delegate(
      [this](float x, float y, float)
      { return this->noise.GetNoise(this->kw.x * x, this->kw.y * y); });
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

  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        float local_ratio = ctrl_param * this->ratio;

        float w1 = this->noise1.GetNoise(this->kw.x * x, this->kw.y * y);
        float w2 = this->noise2.GetNoise(this->kw.x * x, this->kw.y * y);
        if (this->k)
          return maximum_smooth(local_ratio * w1,
                                (1.f - local_ratio) * w2,
                                this->k);
        else
          return std::max(local_ratio * w1, (1.f - local_ratio) * w2);
      });
}

// --- helper

std::unique_ptr<NoiseFunction> create_noise_function_from_type(
    NoiseType   noise_type,
    Vec2<float> kw,
    uint        seed)
{
  switch (noise_type)
  {
    // clang-format off
  case (NoiseType::PERLIN):
    return std::unique_ptr<NoiseFunction>(new PerlinFunction(kw, seed));
  case (NoiseType::PERLIN_BILLOW):
    return std::unique_ptr<NoiseFunction>(new PerlinBillowFunction(kw, seed));
  case (NoiseType::PERLIN_HALF):
  {
    float k = 0.5f;
    return std::unique_ptr<NoiseFunction>(new PerlinHalfFunction(kw, seed, k));
  }
  case (NoiseType::SIMPLEX2):
    return std::unique_ptr<NoiseFunction>(new Simplex2Function(kw, seed));
  case (NoiseType::SIMPLEX2S):
    return std::unique_ptr<NoiseFunction>(new Simplex2SFunction(kw, seed));
  case (NoiseType::VALUE):
    return std::unique_ptr<NoiseFunction>(new ValueNoiseFunction(kw, seed));
  case (NoiseType::VALUE_CUBIC):
    return std::unique_ptr<NoiseFunction>(new ValueCubicNoiseFunction(kw, seed));
  case (NoiseType::VALUE_DELAUNAY):
    return std::unique_ptr<NoiseFunction>(new ValueDelaunayNoiseFunction(kw, seed));
  case (NoiseType::VALUE_LINEAR):
    return std::unique_ptr<NoiseFunction>(new ValueLinearNoiseFunction(kw, seed));
  case (NoiseType::VALUE_THINPLATE):
    return std::unique_ptr<NoiseFunction>(new ValueThinplateNoiseFunction(kw, seed));
  case (NoiseType::WORLEY):
    return std::unique_ptr<NoiseFunction>(new WorleyFunction(kw, seed, false));
  case (NoiseType::WORLEY_DOUBLE):
  {
    float ratio = 0.5f;
    float k = 0.5f;
    return std::unique_ptr<NoiseFunction>(new WorleyDoubleFunction(kw, seed, ratio, k));
  }
  case (NoiseType::WORLEY_VALUE):
    return std::unique_ptr<NoiseFunction>(new WorleyFunction(kw, seed, true));
    // clang-format on
  }

  return nullptr;
}

} // namespace hmap
