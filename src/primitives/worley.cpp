#include "FastNoiseLite.h"

#include "highmap/array.hpp"

namespace hmap
{

hmap::Array worley(std::vector<int>   shape,
                   std::vector<float> kw,
                   uint               seed,
                   std::vector<float> shift)
{
  hmap::Array   array = hmap::Array(shape);
  FastNoiseLite noise(seed);

  // frequency is taken into account in the coordinate systems (to
  // allow different wavelength in both directions)
  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
  noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);

  for (int i = 0; i < array.shape[0]; i++)
  {
    for (int j = 0; j < array.shape[1]; j++)
    {
      float ki = kw[0] / (float)shape[0];
      float kj = kw[1] / (float)shape[1];
      array(i, j) = noise.GetNoise(ki * (float)i + shift[0],
                                   kj * (float)j + shift[1]);
    }
  }
  return array;
}

} // namespace hmap
