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

Array fbm_perlin(std::vector<int>   shape,
                 std::vector<float> kw,
                 uint               seed,
                 int                octaves,
                 float              persistence,
                 float              lacunarity,
                 float              weight,
                 std::vector<float> shift)
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

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
    {
      float ki = kw[0] / (float)shape[0];
      float kj = kw[1] / (float)shape[1];
      float v =
          noise.GetNoise(ki * (float)i + shift[0], kj * (float)j + shift[1]);
      array(i, j) = v;
    }
  return array;
}

Array fbm_perlin_advanced(std::vector<int>   shape,
                          std::vector<float> kw,
                          uint               seed,
                          int                octaves,
                          float              persistence,
                          float              lacunarity,
                          Array             &weight,
                          float              clamp_min,
                          float              clamp_k,
                          std::vector<float> shift)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  float amp0 = compute_fractal_bounding(octaves, persistence);
  float ki0 = kw[0] / (float)shape[0];
  float kj0 = kw[1] / (float)shape[1];

  std::vector<float> shift0 = {shift[0] / ki0, shift[1] / kj0};

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
    {
      float sum = 0.f;
      float amp = amp0;
      float ki = ki0;
      float kj = kj0;
      int   kseed = seed;

      for (int k = 0; k < octaves; k++)
      {
        noise.SetSeed(kseed++);
        float value = noise.GetNoise(ki * ((float)i + shift0[0]),
                                     kj * ((float)j + shift0[1]));
        sum += value * amp;
        amp *= (1.f - weight(i, j)) +
               weight(i, j) * std::min(value + 1.f, 2.f) * 0.5f;

        ki *= lacunarity;
        kj *= lacunarity;
        amp *= persistence;
      }
      array(i, j) = sum;
    }

  clamp_min_smooth(array, clamp_min, clamp_k);

  return array;
}

Array fbm_worley(std::vector<int>   shape,
                 std::vector<float> kw,
                 uint               seed,
                 int                octaves,
                 float              persistence,
                 float              lacunarity,
                 float              weight,
                 std::vector<float> shift)
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

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
    {
      float ki = kw[0] / (float)shape[0];
      float kj = kw[1] / (float)shape[1];
      array(i, j) =
          noise.GetNoise(ki * (float)i + shift[0], kj * (float)j + shift[1]);
    }
  return array;
}

Array hybrid_fbm_perlin(std::vector<int>   shape,
                        std::vector<float> kw,
                        uint               seed,
                        int                octaves,
                        float              persistence,
                        float              lacunarity,
                        float              offset,
                        std::vector<float> shift)
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
    Array noise = perlin(shape, {ck * kw[0], ck * kw[1]}, seed++, shift);
    noise += offset;
    noise *= ak;
    weight *= noise;
    array += weight;
    scaling += ak;
    clamp_max(weight, 1.f); // to prevent divergence
  }

  return array / scaling;
}

Array multifractal_perlin(std::vector<int>   shape,
                          std::vector<float> kw,
                          uint               seed,
                          int                octaves,
                          float              persistence,
                          float              lacunarity,
                          float              offset,
                          std::vector<float> shift)
{
  Array array = constant(shape, 1.f);
  for (int k = 0; k < octaves; k++)
  {
    // pretty much the same as fBm but with a product instead of a
    // sum...
    float ck = std::pow(lacunarity, k);
    Array noise = offset + perlin(shape, {ck * kw[0], ck * kw[1]}, seed, shift);
    array = array * std::pow(persistence, k) * noise;
  }
  return array;
}

Array ridged_perlin(std::vector<int>   shape,
                    std::vector<float> kw,
                    uint               seed,
                    int                octaves,
                    float              persistence,
                    float              lacunarity,
                    float              weight,
                    std::vector<float> shift)
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

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
    {
      float ki = kw[0] / (float)shape[0];
      float kj = kw[1] / (float)shape[1];
      array(i, j) =
          noise.GetNoise(ki * (float)i + shift[0], kj * (float)j + shift[1]);
    }
  return array;
}

} // namespace hmap
