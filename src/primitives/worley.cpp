#include "FastNoiseLite.h"

#include "highmap/array.hpp"

namespace hmap
{

hmap::Array worley(std::vector<int>   shape,
                   std::vector<float> kw,
                   uint               seed,
                   Array             *p_noise,
                   std::vector<float> shift,
                   std::vector<float> scale)
{
  hmap::Array   array = hmap::Array(shape);
  FastNoiseLite noise(seed);

  // frequency is taken into account in the coordinate systems (to
  // allow different wavelength in both directions)
  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
  noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
  noise.SetCellularJitter(1.f);

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

} // namespace hmap
