/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "FastNoiseLite.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/noise_function.hpp"
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
                    Array      *p_stretching,
                    Vec4<float> bbox)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  Vec2<float> shift = {bbox.a, bbox.c};
  Vec2<float> scale = {bbox.b - bbox.a, bbox.d - bbox.c};

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
                 Vec4<float> bbox)
{
  Array                array = Array(shape);
  hmap::PerlinFunction b = hmap::PerlinFunction(kw, seed);
  hmap::FbmFunction    f = hmap::FbmFunction(b.get_base_ref(),
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

Array fbm_pingpong_perlin(Vec2<int>   shape,
                          Vec2<float> kw,
                          uint        seed,
                          int         octaves,
                          float       weight,
                          float       persistence,
                          float       lacunarity,
                          Array      *p_noise_x,
                          Array      *p_noise_y,
                          Array      *p_stretching,
                          Vec4<float> bbox)
{
  Array                     array = Array(shape);
  hmap::PerlinFunction      b = hmap::PerlinFunction(kw, seed);
  hmap::FbmPingpongFunction f = hmap::FbmPingpongFunction(b.get_base_ref(),
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

Array fbm_ridged_perlin(Vec2<int>   shape,
                        Vec2<float> kw,
                        uint        seed,
                        int         octaves,
                        float       weight,
                        float       persistence,
                        float       lacunarity,
                        float       k_smoothing,
                        Array      *p_noise_x,
                        Array      *p_noise_y,
                        Array      *p_stretching,
                        Vec4<float> bbox)
{
  Array                   array = Array(shape);
  hmap::PerlinFunction    b = hmap::PerlinFunction(kw, seed);
  hmap::FbmRidgedFunction f = hmap::FbmRidgedFunction(b.get_base_ref(),
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
                  Vec4<float> bbox)
{
  Array                  array = Array(shape);
  hmap::Simplex2Function b = hmap::Simplex2Function(kw, seed);
  hmap::FbmFunction      f = hmap::FbmFunction(b.get_base_ref(),
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
                 Vec4<float> bbox)
{
  Array                array = Array(shape);
  hmap::WorleyFunction b = hmap::WorleyFunction(kw, seed);
  hmap::FbmFunction    f = hmap::FbmFunction(b.get_base_ref(),
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
                        Array      *p_stretching,
                        Vec4<float> bbox)
{
  Array                      array = Array(shape);
  hmap::WorleyDoubleFunction b = hmap::WorleyDoubleFunction(kw, seed, ratio, k);
  hmap::FbmFunction          f = hmap::FbmFunction(b.get_base_ref(),
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

} // namespace hmap
