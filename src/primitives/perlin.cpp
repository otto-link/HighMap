/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "FastNoiseLite.h"

#include "highmap/array.hpp"

namespace hmap
{

Array perlin(std::vector<int>   shape,
             std::vector<float> kw,
             uint               seed,
             Array             *p_noise,
             std::vector<float> shift,
             std::vector<float> scale)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  float ki = kw[0] / (float)shape[0] * scale[0];
  float kj = kw[1] / (float)shape[1] * scale[1];

  if (!p_noise)
  {
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
        array(i, j) = noise.GetNoise(ki * (float)i + kw[0] * shift[0],
                                     kj * (float)j + kw[1] * shift[1]);
  }
  else
  {
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
        array(i, j) = noise.GetNoise(ki * (float)i + kw[0] * shift[0] +
                                         (*p_noise)(i, j),
                                     kj * (float)j + kw[1] * shift[1]);
  }
  return array;
}

Array perlin_billow(std::vector<int>   shape,
                    std::vector<float> kw,
                    uint               seed,
                    Array             *p_noise,
                    std::vector<float> shift,
                    std::vector<float> scale)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  float ki = kw[0] / (float)shape[0] * scale[0];
  float kj = kw[1] / (float)shape[1] * scale[1];

  if (!p_noise)
  {
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
        array(i, j) = 2.f * std::abs(noise.GetNoise(
                                ki * (float)i + kw[0] * shift[0],
                                kj * (float)j + kw[1] * shift[1])) -
                      1.f;
  }
  else
  {
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
        array(i, j) = 2.f * std::abs(noise.GetNoise(
                                ki * (float)i + kw[0] * shift[0] +
                                    (*p_noise)(i, j),
                                kj * (float)j + kw[1] * shift[1])) -
                      1.f;
  }
  return array;
}

Array perlin_mix(std::vector<int>   shape,
                 std::vector<float> kw,
                 uint               seed,
                 Array             *p_noise,
                 std::vector<float> shift,
                 std::vector<float> scale)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  float ki = kw[0] / (float)shape[0] * scale[0];
  float kj = kw[1] / (float)shape[1] * scale[1];

  if (!p_noise)
  {
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
      {
        float value = noise.GetNoise(ki * (float)i + kw[0] * shift[0],
                                     kj * (float)j + kw[1] * shift[1]);
        array(i, j) = 0.5f * value + std::abs(value) - 0.5f;
      }
  }
  else
  {
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
      {
        float value = noise.GetNoise(ki * (float)i + kw[0] * shift[0] +
                                         (*p_noise)(i, j),
                                     kj * (float)j + kw[1] * shift[1]);
        array(i, j) = 0.5f * value + std::abs(value) - 0.5f;
      }
  }
  return array;
}

} // namespace hmap
