#define _USE_MATH_DEFINES

#include <cmath>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array biweight(std::vector<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape[0] - 1.f));
  int   rj = (int)(0.5f * ((float)shape[1] - 1.f));

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
    {
      float xi = ((float)i - ri) / ((float)(ri + 1));
      float yi = ((float)j - rj) / ((float)(rj + 1));
      float r2 = xi * xi + yi * yi;
      if (r2 < 1.f)
        array(i, j) = (1.f - r2) * (1.f - r2);
    }

  return array;
}

Array bump_field(std::vector<int> shape,
                 float            kw,
                 uint             seed,
                 float            shape_factor)
{
  Array z = Array(shape);
  float density = kw * kw / (float)(shape[0] * shape[1]);
  float rmax = shape_factor * (float)std::min(shape[0], shape[1]) / kw;

  z = white_sparse(shape, 0.f, 1.f, density, seed);
  z = distance_transform(z);

  clamp_max(z, rmax);
  z /= rmax;
  z = 1.f - z * z * (2.f - z);

  return z;
}

Array cone(std::vector<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape[0] - 1.f));
  int   rj = (int)(0.5f * ((float)shape[1] - 1.f));

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      float r = std::hypot(xi / float(ri + 1), yi / float(rj + 1));
      array(i, j) = std::max(0.f, 1.f - r);
    }

  return array;
}

Array cone_smooth(std::vector<int> shape)
{
  Array array = cone(shape);
  almost_unit_identity(array);
  return array;
}

Array cone_talus(float height, float talus)
{
  // define output array size so that starting from an amplitude h,
  // zero is indeed reached with provided slope (talus) over the
  // half-width of the domain (since we build a cone)
  int   n = std::max(1, (int)(2.f * height / talus));
  Array array = Array({n, n});

  if (n > 0)
    array = height * cone({n, n});
  else
    array = 1.f;

  return array;
}

Array cubic_pulse(std::vector<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape[0] - 1.f));
  int   rj = (int)(0.5f * ((float)shape[1] - 1.f));

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      float r = std::hypot(xi / float(ri + 1), yi / float(rj + 1));

      if (r < 1.f)
        array(i, j) = 1.f - r * r * (3.f - 2.f * r);
    }

  return array;
}

Array disk(std::vector<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape[0] - 1.f));
  int   rj = (int)(0.5f * ((float)shape[1] - 1.f));

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
    {
      if ((i - ri) * (i - ri) + (j - rj) * (j - rj) <= ri * rj)
        array(i, j) = 1.f;
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
  // TODO !!!
  return array;
}

Array gabor(std::vector<int> shape,
            float            kw,
            float            angle,
            float            footprint_threshold)
{
  Array array = Array(shape);

  std::vector<float> x = linspace(-1.f, 1.f, array.shape[0]);
  std::vector<float> y = linspace(-1.f, 1.f, array.shape[1]);

  float width = std::sqrt(-0.5f * M_PI / std::log(footprint_threshold));
  float iw2 = 1.f / (width * width);
  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
      array(i, j) = std::exp(-M_PI * (x[i] * x[i] + y[j] * y[j]) * 0.5f * iw2) *
                    std::cos(kw * (x[i] * ca + y[j] * sa));

  return array;
}

Array smooth_cosine(std::vector<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape[0] - 1.f));
  int   rj = (int)(0.5f * ((float)shape[1] - 1.f));

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      float r = M_PI * std::hypot(xi / float(ri + 1), yi / float(rj + 1));
      if (r < M_PI)
        array(i, j) = 0.5f + 0.5f * std::cos(r);
    }

  return array;
}

Array tricube(std::vector<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape[0] - 1.f));
  int   rj = (int)(0.5f * ((float)shape[1] - 1.f));

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      float r = std::hypot(xi / float(ri + 1), yi / float(rj + 1));
      if (r < 1.f)
        array(i, j) = std::pow(1.f - std::pow(r, 3.f), 3.f);
    }

  return array;
}

} // namespace hmap
