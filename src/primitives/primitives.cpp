#define _USE_MATH_DEFINES

#include <cmath>
#include <vector>

#include "highmap/array.hpp"

namespace hmap
{

Array biweight(std::vector<int> shape)
{
  Array array = Array(shape);

  int ri = (int)(0.5f * ((float)shape[0] - 1.f));
  int rj = (int)(0.5f * ((float)shape[1] - 1.f));

  for (int i = 0; i < array.shape[0]; i++)
  {
    for (int j = 0; j < array.shape[1]; j++)
    {
      float xi = ((float)i - ri) / ((float)(ri + 1));
      float yi = ((float)j - rj) / ((float)(rj + 1));
      float r2 = xi * xi + yi * yi;
      array(i, j) = std::max(0.f, (1.f - r2) * (1.f - r2));
    }
  }
  return array;
}

Array cone(std::vector<int> shape)
{
  Array array = Array(shape);

  int ri = (int)(0.5f * ((float)shape[0] - 1.f));
  int rj = (int)(0.5f * ((float)shape[1] - 1.f));

  for (int i = 0; i < array.shape[0]; i++)
  {
    for (int j = 0; j < array.shape[1]; j++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      float r = std::hypot(xi / float(ri + 1), yi / float(rj + 1));
      array(i, j) = std::max(0.f, 1.f - r);
    }
  }
  return array;
}

Array cone_talus(float height, float talus)
{
  // define output array size so that starting from an amplitude h,
  // zero is indeed reached with provided slope (talus) over the
  // half-width of the domain (since we build a cone)
  int n = std::max(1, (int)(2.f * height / talus));

  Array array = Array({n, n});

  if (n > 0)
    array = height * cone({n, n});
  else
    array = 1.f;

  return array;
}

Array disk(std::vector<int> shape)
{
  Array array = Array(shape);

  int ri = (int)(0.5f * ((float)shape[0] - 1.f));
  int rj = (int)(0.5f * ((float)shape[1] - 1.f));

  for (int i = 0; i < array.shape[0]; i++)
  {
    for (int j = 0; j < array.shape[1]; j++)
    {
      if ((i - ri) * (i - ri) + (j - rj) * (j - rj) <= ri * rj)
        array(i, j) = 1.f;
    }
  }
  return array;
}

Array constant(std::vector<int> shape, float value)
{
  Array array = Array(shape);
  for (auto &v : array.vector)
    v = value;
  return array;
}

Array plane(std::vector<int>   shape,
            float              talus,
            float              yaw_angle,
            std::vector<float> xyz_center)
{
  Array array = Array(shape);
  return array;
}

Array smooth_cosine(std::vector<int> shape)
{
  Array array = Array(shape);

  int ri = (int)(0.5f * ((float)shape[0] - 1.f));
  int rj = (int)(0.5f * ((float)shape[1] - 1.f));

  for (int i = 0; i < array.shape[0]; i++)
  {
    for (int j = 0; j < array.shape[1]; j++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      float r = M_PI * std::hypot(xi / float(ri + 1), yi / float(rj + 1));
      array(i, j) = std::max(0.f, 0.5f + 0.5f * std::cos(r));
    }
  }
  return array;
}

} // namespace hmap
