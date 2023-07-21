#include "FastNoiseLite.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array value_noise(std::vector<int>   shape,
                  std::vector<float> kw,
                  uint               seed,
                  Array             *p_noise,
                  std::vector<float> shift)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Value);

  if (!p_noise)
  {
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
      {
        float ki = kw[0] / (float)shape[0];
        float kj = kw[1] / (float)shape[1];
        array(i, j) = noise.GetNoise(ki * (float)i + shift[0],
                                     kj * (float)j + shift[1]);
      }
  }
  else
  {
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
      {
        float ki = kw[0] / (float)shape[0];
        float kj = kw[1] / (float)shape[1];
        array(i,
              j) = noise.GetNoise(ki * (float)i + shift[0] + (*p_noise)(i, j),
                                  kj * (float)j + shift[1]);
      }
  }

  return array;
}

Array value_noise_linear(std::vector<int>   shape,
                         std::vector<float> kw,
                         uint               seed,
                         Array             *p_noise,
                         std::vector<float> shift) // TODO
{
  // TODO - shift for white noise
  Array array = white({(int)kw[0] + 1, (int)kw[1] + 1}, 0.f, 1.f, seed);
  array = array.resample_to_shape(shape);
  return array;
}

} // namespace hmap
