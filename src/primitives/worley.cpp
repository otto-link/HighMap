#include "FastNoiseLite.h"

#include "highmap/array.hpp"

namespace hmap
{

hmap::Array worley(std::vector<int>   shape,
                   std::vector<float> kw,
                   uint               seed,
                   Array             *p_noise,
                   std::vector<float> shift)
{
  hmap::Array   array = hmap::Array(shape);
  FastNoiseLite noise(seed);

  // frequency is taken into account in the coordinate systems (to
  // allow different wavelength in both directions)
  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
  noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
  noise.SetCellularJitter(1.f);

  if (!p_noise)
  {
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
      {
        float ki = kw[0] / (float)shape[0];
        float kj = kw[1] / (float)shape[1];
        array(i, j) = noise.GetNoise(ki * ((float)i + shift[0]),
                                     kj * ((float)j + shift[1]));
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
              j) = noise.GetNoise(ki * ((float)i + shift[0] + (*p_noise)(i, j)),
                                  kj * ((float)j + shift[1]));
      }
  }

  return array;
}

} // namespace hmap
