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

Array fbm_perlin(Vec2<int>   shape,
                 Vec2<float> kw,
                 uint        seed,
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
  noise.SetFractalOctaves(octaves);
  noise.SetFractalLacunarity(lacunarity);
  noise.SetFractalGain(persistence);
  noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  noise.SetFractalWeightedStrength(weight);

  array = helper_get_noise(array,
                           kw,
                           p_noise_x,
                           p_noise_y,
                           shift,
                           scale,
                           [&noise](float x, float y)
                           { return noise.GetNoise(x, y); });
  return array;
}

Array fbm_perlin_advanced(Vec2<int>    shape,
                          Vec2<float>  kw,
                          uint         seed,
                          int          octaves,
                          const Array &weight,
                          float        persistence,
                          float        lacunarity,
                          Vec2<float>  shift)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  float amp0 = compute_fractal_bounding(octaves, persistence);
  float ki0 = kw.x / (float)shape.x;
  float kj0 = kw.y / (float)shape.y;

  Vec2<float> shift0 = Vec2<float>(shift.x / ki0, shift.y / kj0);

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      float sum = 0.f;
      float amp = amp0;
      float ki = ki0;
      float kj = kj0;
      int   kseed = seed;

      for (int k = 0; k < octaves; k++)
      {
        noise.SetSeed(kseed++);
        float value = noise.GetNoise(ki * ((float)i + shift0.x),
                                     kj * ((float)j + shift0.y));
        sum += value * amp;
        amp *= (1.f - weight(i, j)) +
               weight(i, j) * std::min(value + 1.f, 2.f) * 0.5f;

        ki *= lacunarity;
        kj *= lacunarity;
        amp *= persistence;
      }
      array(i, j) = sum;
    }

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

  array = helper_get_noise(array,
                           kw,
                           p_noise_x,
                           p_noise_y,
                           shift,
                           scale,
                           [&noise](float x, float y)
                           { return noise.GetNoise(x, y); });
  return array;
}

Array hybrid_fbm_perlin(Vec2<int>   shape,
                        Vec2<float> kw,
                        uint        seed,
                        int         octaves,
                        float       persistence,
                        float       lacunarity,
                        float       offset,
                        Vec2<float> shift)
{
  Array         array = Array(shape);
  Array         weight = constant(shape, 1.f);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  // keep track of the amplitudes of each harmonics to ensure that the
  // output is roughly within the same range as the fundamental
  float scaling = 1.f;

  for (int k = 0; k < octaves; k++)
  {
    float ck = std::pow(lacunarity, k);
    float ak = std::pow(persistence, k);
    Array noise =
        perlin(shape, {ck * kw.x, ck * kw.y}, seed++, nullptr, nullptr, shift);
    noise += offset;
    noise *= ak;
    weight *= noise;
    array += weight;
    scaling += ak;
    clamp_max(weight, 1.f); // to prevent divergence
  }

  return array / scaling;
}

Array multifractal_perlin(Vec2<int>   shape,
                          Vec2<float> kw,
                          uint        seed,
                          int         octaves,
                          float       persistence,
                          float       lacunarity,
                          float       offset,
                          Vec2<float> shift,
                          Vec2<float> scale)
{
  Array array = constant(shape, 1.f);
  for (int k = 0; k < octaves; k++)
  {
    // pretty much the same as fBm but with a product instead of a
    // sum...
    float ck = std::pow(lacunarity, k);
    Array noise = offset + perlin(shape,
                                  {ck * kw.x, ck * kw.y},
                                  seed,
                                  nullptr,
                                  nullptr,
                                  shift,
                                  scale);
    array = array * std::pow(persistence, k) * noise;
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

  array = helper_get_noise(array,
                           kw,
                           p_noise_x,
                           p_noise_y,
                           shift,
                           scale,
                           [&noise](float x, float y)
                           { return noise.GetNoise(x, y); });
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

  array = helper_get_noise(array,
                           kw,
                           p_noise_x,
                           p_noise_y,
                           shift,
                           scale,
                           [&noise](float x, float y)
                           { return noise.GetNoise(x, y); });
  return array;
}

} // namespace hmap
