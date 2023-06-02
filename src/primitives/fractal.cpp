#include <cmath>

#include "FastNoiseLite.h"

#include "highmap/array.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

hmap::Array fbm_perlin(std::vector<int>   shape,
                       std::vector<float> kw,
                       uint               seed,
                       int                octaves,
                       float              persistence,
                       float              lacunarity,
                       float              offset,
                       std::vector<float> shift)
{
  hmap::Array   array = hmap::Array(shape);
  FastNoiseLite noise(seed);

  // frequency is taken into account in the coordinate systems (to
  // allow different wavelength in both directions)
  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  noise.SetFractalOctaves(octaves);
  noise.SetFractalLacunarity(lacunarity);
  noise.SetFractalGain(persistence);
  noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  noise.SetFractalWeightedStrength(offset);

  for (int i = 0; i < array.shape[0]; i++)
  {
    for (int j = 0; j < array.shape[1]; j++)
    {
      float ki = kw[0] / (float)shape[0];
      float kj = kw[1] / (float)shape[1];
      array(i, j) =
          noise.GetNoise(ki * (float)i + shift[0], kj * (float)j + shift[1]);
    }
  }
  return array;
}

hmap::Array multifractal_perlin(std::vector<int>   shape,
                                std::vector<float> kw,
                                uint               seed,
                                int                octaves,
                                float              persistence,
                                float              lacunarity,
                                float              offset,
                                std::vector<float> shift)
{
  hmap::Array array = hmap::constant(shape, 1.f);
  for (int k = 0; k < octaves; k++)
  {
    // pretty much the same as fBm but with a product instead of a
    // sum...
    float       ck = std::pow(lacunarity, k);
    hmap::Array array_k =
        offset + hmap::perlin(shape, {ck * kw[0], ck * kw[1]}, seed, shift);
    array = array * std::pow(persistence, k) * array_k;
  }
  return array;
}

hmap::Array ridged_perlin(std::vector<int>   shape,
                          std::vector<float> kw,
                          uint               seed,
                          int                octaves,
                          float              persistence,
                          float              lacunarity,
                          float              offset,
                          std::vector<float> shift)
{
  hmap::Array   array = hmap::Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  noise.SetFractalOctaves(octaves);
  noise.SetFractalLacunarity(lacunarity);
  noise.SetFractalGain(persistence);
  noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
  noise.SetFractalWeightedStrength(offset);

  for (int i = 0; i < array.shape[0]; i++)
  {
    for (int j = 0; j < array.shape[1]; j++)
    {
      float ki = kw[0] / (float)shape[0];
      float kj = kw[1] / (float)shape[1];
      array(i, j) =
          noise.GetNoise(ki * (float)i + shift[0], kj * (float)j + shift[1]);
    }
  }
  return array;
}

} // namespace hmap
