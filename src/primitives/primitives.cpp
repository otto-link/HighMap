#include <cmath>
#include <vector>

#include "highmap/array.hpp"

namespace hmap
{

hmap::Array cone(std::vector<int> shape)
{
  hmap::Array array = hmap::Array(shape);

  int ri = (int)(0.5f * ((float)shape[0] - 1.f));
  int rj = (int)(0.5f * ((float)shape[1] - 1.f));

  for (int i = 0; i < array.shape[0]; i++)
  {
    for (int j = 0; j < array.shape[1]; j++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      array(i, j) =
          std::max(0.f,
                   1.f - std::hypot(xi / float(ri + 1), yi / float(rj + 1)));
    }
  }
  return array;
}

hmap::Array cone_talus(float height, float talus)
{
  // define output array size so that starting from an amplitude h,
  // zero is indeed reached with provided slope (talus) over the
  // half-width of the domain (since we build a cone)
  int n = std::max(1, (int)(2.f * height / talus));

  hmap::Array array = hmap::Array({n, n});

  if (n > 0)
    array = height * cone({n, n});
  else
    array = 1.f;

  return array;
}

hmap::Array constant(std::vector<int> shape, float value)
{
  hmap::Array array = hmap::Array(shape);
  for (auto &v : array.vector)
    v = value;
  return array;
}

Array plane(std::vector<int>   shape,
            float              talus,
            float              yaw_angle,
            std::vector<float> xyz_center)
{
  hmap::Array array = hmap::Array(shape);
  return array;
}

} // namespace hmap
