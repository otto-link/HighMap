/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "FastNoiseLite.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

//----------------------------------------------------------------------
// Helper(s)
//----------------------------------------------------------------------

float compute_fractal_bounding(int octaves, float persistence)
{
  float amp = persistence;
  float amp_fractal = 1.0f;
  for (int i = 1; i < octaves; i++)
  {
    amp_fractal += amp;
    amp *= persistence;
  }
  return 1.f / amp_fractal;
}

//----------------------------------------------------------------------
// Main operator(s)
//----------------------------------------------------------------------

namespace hmap
{

Array fbm_iq_perlin(Vec2<int>   shape,
                    Vec2<float> kw,
                    uint        seed,
                    float       gradient_weight,
                    float       value_weight,
                    int         octaves,
                    float       weight,
                    float       persistence,
                    float       lacunarity,
                    Array      *p_noise_x,
                    Array      *p_noise_y,
                    Vec2<float> shift,
                    Vec2<float> scale)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  float amp0 = compute_fractal_bounding(octaves, persistence);

  std::vector<float> x = linspace(kw.x * shift.x,
                                  kw.x * (shift.x + scale.x),
                                  array.shape.x,
                                  false);
  std::vector<float> y = linspace(kw.y * shift.y,
                                  kw.y * (shift.y + scale.y),
                                  array.shape.y,
                                  false);

  float eps = 1e-2f;

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      float sum = 0.f;
      float amp = amp0;
      float ki = 1.f;
      float kj = 1.f;
      int   kseed = seed;

      float dx_sum = 0.f;
      float dy_sum = 0.f;

      for (int k = 0; k < octaves; k++)
      {
        noise.SetSeed(kseed++);
        float value = noise.GetNoise(ki * x[i], kj * y[j]);

        float dx = (noise.GetNoise(ki * (x[i] + eps), kj * y[j]) - value) / eps;
        float dy = (noise.GetNoise(ki * x[i], kj * (y[j] + eps)) - value) / eps;
        dx_sum += dx;
        dy_sum += dy;

        float factor = std::min(value + 1.f, 2.f) * 0.5f;

        sum += value * amp /
               (1.f + gradient_weight * (dx_sum * dx_sum + dy_sum * dy_sum) +
                value_weight * factor);
        amp *= (1.f - weight) + weight * factor;

        ki *= lacunarity;
        kj *= lacunarity;
        amp *= persistence;
      }
      array(i, j) = sum;
    }

  return array;
}

Array fbm_perlin(Vec2<int>   shape,
                 Vec2<float> kw,
                 uint        seed,
                 int         octaves,
                 float       weight,
                 float       persistence,
                 float       lacunarity,
                 Array      *p_noise_x,
                 Array      *p_noise_y,
                 Array      *p_stretching,
                 Vec2<float> shift,
                 Vec2<float> scale)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  noise.SetFractalOctaves(octaves);
  noise.SetFractalLacunarity(lacunarity);
  noise.SetFractalGain(persistence);
  noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  noise.SetFractalWeightedStrength(weight);

  std::vector<float> x = linspace(kw.x * shift.x,
                                  kw.x * (shift.x + scale.x),
                                  array.shape.x,
                                  false);
  std::vector<float> y = linspace(kw.y * shift.y,
                                  kw.y * (shift.y + scale.y),
                                  array.shape.y,
                                  false);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               [&noise](float x_, float y_, float)
                               { return noise.GetNoise(x_, y_); });
  return array;
}

Array fbm_simplex(Vec2<int>   shape,
                  Vec2<float> kw,
                  uint        seed,
                  int         octaves,
                  float       weight,
                  float       persistence,
                  float       lacunarity,
                  Array      *p_noise_x,
                  Array      *p_noise_y,
                  Array      *p_stretching,
                  Vec2<float> shift,
                  Vec2<float> scale)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  noise.SetFractalOctaves(octaves);
  noise.SetFractalLacunarity(lacunarity);
  noise.SetFractalGain(persistence);
  noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  noise.SetFractalWeightedStrength(weight);

  std::vector<float> x = linspace(0.5f * kw.x * shift.x,
                                  0.5f * kw.x * (shift.x + scale.x),
                                  array.shape.x,
                                  false);
  std::vector<float> y = linspace(0.5f * kw.y * shift.y,
                                  0.5f * kw.y * (shift.y + scale.y),
                                  array.shape.y,
                                  false);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               [&noise](float x_, float y_, float)
                               { return noise.GetNoise(x_, y_); });
  return array;
}

Array fbm_worley(Vec2<int>   shape,
                 Vec2<float> kw,
                 uint        seed,
                 int         octaves,
                 float       weight,
                 float       persistence,
                 float       lacunarity,
                 Array      *p_noise_x,
                 Array      *p_noise_y,
                 Array      *p_stretching,
                 Vec2<float> shift,
                 Vec2<float> scale)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
  noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
  noise.SetFractalOctaves(octaves);
  noise.SetFractalLacunarity(lacunarity);
  noise.SetFractalGain(persistence);
  noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  noise.SetFractalWeightedStrength(weight);

  std::vector<float> x = linspace(kw.x * shift.x,
                                  kw.x * (shift.x + scale.x),
                                  array.shape.x,
                                  false);
  std::vector<float> y = linspace(kw.y * shift.y,
                                  kw.y * (shift.y + scale.y),
                                  array.shape.y,
                                  false);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               [&noise](float x_, float y_, float)
                               { return noise.GetNoise(x_, y_); });

  return array;
}

Array fbm_worley_double(Vec2<int>   shape,
                        Vec2<float> kw,
                        uint        seed,
                        float       ratio,
                        float       k,
                        int         octaves,
                        float       weight,
                        float       persistence,
                        float       lacunarity,
                        Array      *p_noise_x,
                        Array      *p_noise_y,
                        Vec2<float> shift,
                        Vec2<float> scale)
{
  Array array = Array(shape);

  Array amp = constant(shape, compute_fractal_bounding(octaves, persistence));

  for (int k = 0; k < octaves; k++)
  {
    Array value = worley_double(shape,
                                kw,
                                seed++,
                                ratio,
                                k,
                                p_noise_x,
                                p_noise_y,
                                shift,
                                scale);
    array += value * amp;
    amp *= (1.f - weight) + weight * minimum(value + 1.f, 2.f) * 0.5f;

    kw = {kw.x * lacunarity, kw.y * lacunarity};
    amp *= persistence;
  }

  return array;
}

Array pingpong_perlin(Vec2<int>   shape,
                      Vec2<float> kw,
                      uint        seed,
                      int         octaves,
                      float       weight,
                      float       persistence,
                      float       lacunarity,
                      Array      *p_noise_x,
                      Array      *p_noise_y,
                      Array      *p_stretching,
                      Vec2<float> shift,
                      Vec2<float> scale)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  noise.SetFractalOctaves(octaves);
  noise.SetFractalLacunarity(lacunarity);
  noise.SetFractalGain(persistence);
  noise.SetFractalType(FastNoiseLite::FractalType_PingPong);
  noise.SetFractalWeightedStrength(weight);

  std::vector<float> x = linspace(kw.x * shift.x,
                                  kw.x * (shift.x + scale.x),
                                  array.shape.x,
                                  false);
  std::vector<float> y = linspace(kw.y * shift.y,
                                  kw.y * (shift.y + scale.y),
                                  array.shape.y,
                                  false);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               [&noise](float x_, float y_, float)
                               { return noise.GetNoise(x_, y_); });
  return array;
}

Array ridged_perlin(Vec2<int>   shape,
                    Vec2<float> kw,
                    uint        seed,
                    int         octaves,
                    float       weight,
                    float       persistence,
                    float       lacunarity,
                    Array      *p_noise_x,
                    Array      *p_noise_y,
                    Array      *p_stretching,
                    Vec2<float> shift,
                    Vec2<float> scale)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  noise.SetFractalOctaves(octaves);
  noise.SetFractalLacunarity(lacunarity);
  noise.SetFractalGain(persistence);
  noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
  noise.SetFractalWeightedStrength(weight);

  std::vector<float> x = linspace(kw.x * shift.x,
                                  kw.x * (shift.x + scale.x),
                                  array.shape.x,
                                  false);
  std::vector<float> y = linspace(kw.y * shift.y,
                                  kw.y * (shift.y + scale.y),
                                  array.shape.y,
                                  false);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               [&noise](float x_, float y_, float)
                               { return noise.GetNoise(x_, y_); });
  return array;
}

} // namespace hmap
