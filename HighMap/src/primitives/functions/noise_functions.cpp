/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "FastNoiseLite.h"
#include "delaunator-cpp.hpp"

#include "highmap/boundary.hpp"
#include "highmap/functions.hpp"
#include "highmap/geometry/grids.hpp"
#include "highmap/geometry/point_sampling.hpp"
#include "highmap/math.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

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
      { return 1.43f * this->noise.GetNoise(this->kw.x * x, this->kw.y * y); });
}

ValueDelaunayNoiseFunction::ValueDelaunayNoiseFunction(Vec2<float> kw,
                                                       uint        seed)
    : NoiseFunction(kw, seed)
{
  this->set_kw(kw);
  this->set_seed(seed);
  this->update_interpolation_function();
}

void ValueDelaunayNoiseFunction::update_interpolation_function()
{
  // --- generate 'n' random grid points
  int n = (int)(this->kw.x * this->kw.y);

  std::vector<float> x(n);
  std::vector<float> y(n);
  std::vector<float> value(n);

  auto xy = random_points(n,
                          this->seed,
                          PointSamplingMethod::RND_LHS,
                          {0.f, 1.f, 0.f, 1.f});
  x = xy[0];
  y = xy[1];

  expand_points_domain(x, y, value, {0.f, 1.f, 0.f, 1.f});

  // --- interpolation function

  // triangulate
  std::vector<float> coords(2 * x.size());

  for (size_t k = 0; k < x.size(); k++)
  {
    coords[2 * k] = x[k];
    coords[2 * k + 1] = y[k];
  }

  delaunator::Delaunator<float> d(coords);

  // compute and store triangles area
  std::vector<float> area(d.triangles.size());

  for (size_t k = 0; k < d.triangles.size(); k += 3)
  {
    int p0 = d.triangles[k];
    int p1 = d.triangles[k + 1];
    int p2 = d.triangles[k + 2];

    // true area
    area[k] = 0.5f * (-y[p1] * x[p2] + y[p0] * (-x[p1] + x[p2]) +
                      x[p0] * (y[p1] - y[p2]) + x[p1] * y[p2]);

    // but stored like this to avoid doing it at each evaluation while
    // interpolating
    area[k] = 1.f / (2.f * area[k]);
  }

  auto itp_fct = [x, y, value, d, area](float x_, float y_, float)
  {
    // https://stackoverflow.com/questions/2049582

    // compute barycentric coordinates to find in which triangle the
    // point (x_, y_) is inside
    for (size_t k = 0; k < d.triangles.size(); k += 3)
    {
      int p0 = d.triangles[k];
      int p1 = d.triangles[k + 1];
      int p2 = d.triangles[k + 2];

      float s = area[k] * (y[p0] * x[p2] - x[p0] * y[p2] +
                           (y[p2] - y[p0]) * x_ + (x[p0] - x[p2]) * y_);
      float t = area[k] * (x[p0] * y[p1] - y[p0] * x[p1] +
                           (y[p0] - y[p1]) * x_ + (x[p1] - x[p0]) * y_);

      if (s >= 0.f && t >= 0.f && s + t <= 1.f)
        return value[p0] + s * (value[p1] - value[p0]) +
               t * (value[p2] - value[p0]);
    }

    return 1.f;
  };

  this->set_delegate(itp_fct);
}

ValueLinearNoiseFunction::ValueLinearNoiseFunction(Vec2<float> kw, uint seed)
    : NoiseFunction(kw, seed)
{
  this->set_kw(kw);
  this->set_seed(seed);
  this->update_interpolation_function();
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

  Array value = 2.f * white(shape_base, 0.f, 1.f, seed) - 1.f;

  // corresponding grids
  std::vector<float> xv(shape_base.x);
  std::vector<float> yv(shape_base.y);

  for (int i = 0; i < shape_base.x; i++)
    xv[i] = bbox.a + lx * (float)i / (float)(shape_base.x - 1);

  for (int j = 0; j < shape_base.y; j++)
    yv[j] = bbox.c + ly * (float)j / (float)(shape_base.y - 1);

  auto itp_fct = [xv, yv, value, bbox](float x_, float y_, float)
  {
    float xn = (x_ - bbox.a) / (bbox.b - bbox.a) * (float)(value.shape.x - 1);
    float yn = (y_ - bbox.c) / (bbox.d - bbox.c) * (float)(value.shape.y - 1);

    int in = (int)xn;
    int jn = (int)yn;

    float u = xn - (float)in;
    float v = yn - (float)jn;

    if (in == value.shape.x - 1)
    {
      in = value.shape.x - 2;
      u = 1.f;
    }

    if (jn == value.shape.y - 1)
    {
      jn = value.shape.y - 2;
      v = 1.f;
    }

    return value.get_value_bilinear_at(in, jn, u, v);
  };

  this->set_delegate(itp_fct);
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
      {
        return 1.66f *
               (0.4f + this->noise.GetNoise(this->kw.x * x, this->kw.y * y));
      });
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
