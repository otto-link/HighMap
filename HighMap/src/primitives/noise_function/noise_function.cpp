/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "FastNoiseLite.h"
#include "libInterpolate/AnyInterpolator.hpp"
#include "libInterpolate/Interpolate.hpp"

#include "highmap/math.hpp"
#include "highmap/noise_function.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

ArrayFunction::ArrayFunction(hmap::Array array, Vec2<float> kw, bool periodic)
    : NoiseFunction(kw), array(array)
{
  if (periodic)
    this->function = [this](float x, float y, float)
    {
      float xp = 0.5f * this->kw.x * x;
      float yp = 0.5f * this->kw.y * y;

      xp = 2.f * std::abs(xp - int(xp));
      yp = 2.f * std::abs(yp - int(yp));

      xp = xp < 1.f ? xp : 2.f - xp;
      yp = yp < 1.f ? yp : 2.f - yp;

      smoothstep5(xp);
      smoothstep5(yp);

      float xg = xp * (this->array.shape.x - 1);
      float yg = yp * (this->array.shape.y - 1);
      int   i = (int)xg;
      int   j = (int)yg;
      return this->array.get_value_bilinear_at(i, j, xg - i, yg - j);
    };
  else
    this->function = [this](float x, float y, float)
    {
      float xp = std::clamp(this->kw.x * x,
                            0.f,
                            1.f - std::numeric_limits<float>::min());
      float yp = std::clamp(this->kw.y * y,
                            0.f,
                            1.f - std::numeric_limits<float>::min());

      xp = xp - int(xp);
      yp = yp - int(yp);

      float xg = xp * (this->array.shape.x - 1);
      float yg = yp * (this->array.shape.y - 1);
      int   i = (int)xg;
      int   j = (int)yg;
      return this->array.get_value_bilinear_at(i, j, xg - i, yg - j);
    };
}

BumpFunction::BumpFunction(float gain, Vec2<float> center)
    : NoiseFunction(), center(center)
{
  this->set_gain(gain);
  this->function = [this](float x, float y, float)
  {
    float xc = x - this->center.x;
    float yc = y - this->center.y;

    float r2 = xc * xc + yc * yc;
    return r2 > 0.25f
               ? 0.f
               : std::pow(std::exp(-1.f / (1.f - 4.f * r2)), this->inv_gain);
  };
}

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

SlopeFunction::SlopeFunction(float angle, float slope, Vec2<float> center)
    : NoiseFunction(), slope(slope), center(center)
{
  this->set_angle(angle);

  this->function = [this](float x, float y, float)
  {
    float r = this->ca * (x - this->center.x) + this->sa * (y - this->center.y);
    return this->slope * r;
  };
}

StepFunction::StepFunction(float angle, float slope, Vec2<float> center)
    : NoiseFunction(), slope(slope), center(center)
{
  this->set_angle(angle);

  this->function = [this](float x, float y, float)
  {
    float r = this->ca * (x - this->center.x) + this->sa * (y - this->center.y);
    float dt = 0.5f / this->slope;
    if (r > dt)
      r = 1.f;
    else if (r > -dt)
      r = this->slope * (r + dt);
    else
      r = 0.f;
    return r * r * (3.f - 2.f * r);
  };
}

ValueNoiseFunction::ValueNoiseFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(1.f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_Value);

  this->function = [this](float x, float y, float)
  { return this->noise.GetNoise(this->kw.x * x, this->kw.y * y); };
}

ValueCubicNoiseFunction::ValueCubicNoiseFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_seed(seed);
  this->noise.SetFrequency(1.f);
  this->noise.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);

  this->function = [this](float x, float y, float)
  { return this->noise.GetNoise(this->kw.x * x, this->kw.y * y); };
}

ValueDelaunayNoiseFunction::ValueDelaunayNoiseFunction(Vec2<float> kw,
                                                       uint        seed)
    : NoiseFunction(kw, seed)
{
  this->set_kw(kw);
  this->set_seed(seed);

  this->function = [this](float x, float y, float)
  { return this->interp(x, y); };
}

ValueLinearNoiseFunction::ValueLinearNoiseFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_kw(kw);
  this->set_seed(seed);

  this->function = [this](float x, float y, float)
  { return this->interp(x, y); };
}

ValueThinplateNoiseFunction::ValueThinplateNoiseFunction(Vec2<float> kw,
                                                         uint        seed)
    : NoiseFunction(kw, seed)
{
  this->set_kw(kw);
  this->set_seed(seed);

  this->function = [this](float x, float y, float)
  { return this->interp(x, y); };
}

WaveDuneFunction::WaveDuneFunction(Vec2<float> kw,
                                   float       angle,
                                   float       xtop,
                                   float       xbottom,
                                   float       phase_shift)
    : NoiseFunction(kw, 0), xtop(xtop), xbottom(xbottom),
      phase_shift(phase_shift)
{
  this->set_angle(angle);

  this->function = [this](float x, float y, float)
  {
    float r = ca * this->kw.x * x + sa * this->kw.y * y;
    float xp = std::fmod(r + this->phase_shift + 10.f, 1.f);
    float yp = 0.f;

    if (xp < this->xtop)
    {
      float r = xp / this->xtop;
      yp = r * r * (3.f - 2.f * r);
    }
    else if (xp < this->xbottom)
    {
      float r = (xp - this->xbottom) / (this->xtop - this->xbottom);
      yp = r * r * (2.f - r);
    }
    return yp;
  };
}

WaveSineFunction::WaveSineFunction(Vec2<float> kw,
                                   float       angle,
                                   float       phase_shift)
    : NoiseFunction(kw, 0), phase_shift(phase_shift)
{
  this->set_angle(angle);

  this->function = [this](float x, float y, float)
  {
    float r = ca * this->kw.x * x + sa * this->kw.y * y;
    return std::cos(2.f * M_PI * r + this->phase_shift);
  };
}

WaveSquareFunction::WaveSquareFunction(Vec2<float> kw,
                                       float       angle,
                                       float       phase_shift)
    : NoiseFunction(kw, 0), phase_shift(phase_shift)
{
  this->set_angle(angle);

  this->function = [this](float x, float y, float)
  {
    float r = ca * this->kw.x * x + sa * this->kw.y * y;
    return r = 2.f * (int)r - (int)(2.f * r) + 1.f;
  };
}

WaveTriangularFunction::WaveTriangularFunction(Vec2<float> kw,
                                               float       angle,
                                               float       slant_ratio,
                                               float       phase_shift)
    : NoiseFunction(kw, 0), slant_ratio(slant_ratio), phase_shift(phase_shift)
{
  this->set_angle(angle);

  this->function = [this](float x, float y, float)
  {
    float r = ca * this->kw.x * x + sa * this->kw.y * y;

    r = r - (int)r;
    if (r < this->slant_ratio)
      r /= this->slant_ratio;
    else
      r = 1.f - (r - this->slant_ratio) / (1.f - this->slant_ratio);
    return r * r * (3.f - 2.f * r);
  };
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
