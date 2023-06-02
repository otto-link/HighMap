#include "FastNoiseLite.h"

#include "highmap/array.hpp"

namespace hmap
{

Array value_noise(std::vector<int>   shape,
                  std::vector<float> kw,
                  uint               seed,
                  std::vector<float> shift)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Value);

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
