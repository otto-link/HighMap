#include <cmath>

#include "FastNoiseLite.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

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

  // frequency is taken into account in the coordinate systems (to
  // allow different wavelength in both directions)
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

      array(i, j) =
          noise.GetNoise(ki * (float)i + shift[0], kj * (float)j + shift[1]);
    }
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
