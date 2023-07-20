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

Array gaussian_pulse(std::vector<int>   shape,
                     float              sigma,
                     Array             *p_noise,
                     std::vector<float> shift)
{
  Array array = Array(shape);
  int   ic = (int)((0.5f - shift[0]) * shape[0]);
  int   jc = (int)((0.5f - shift[1]) * shape[1]);

  float s2 = 1.f / (sigma * sigma);

  if (!p_noise)
  {
    for (int i = 0; i < shape[0]; i++)
      for (int j = 0; j < shape[1]; j++)
      {
        float r2 = (float)((i - ic) * (i - ic) + (j - jc) * (j - jc));
        array(i, j) = std::exp(-0.5f * r2 * s2);
      }
  }
  else
  {
    for (int i = 0; i < shape[0]; i++)
      for (int j = 0; j < shape[1]; j++)
      {
        float r2 = (float)((i - ic) * (i - ic) + (j - jc) * (j - jc)) +
                   (*p_noise)(i, j) * (*p_noise)(i, j) * (float)shape[0] *
                       (float)shape[1];
        array(i, j) = std::exp(-0.5f * r2 * s2);
      }
  }

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

Array step(std::vector<int>   shape,
           float              angle,
           float              talus,
           Array             *p_noise,
           std::vector<float> shift)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(-0.5f + shift[0], 0.5f + shift[0], shape[0]);
  std::vector<float> y = linspace(-0.5f + shift[1], 0.5f + shift[1], shape[1]);

  // talus value for a unit square domain
  float talus_n = talus * (float)std::max(shape[0], shape[1]);
  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);
  float dt = 0.5f / talus_n;

  auto lambda = [&dt, &talus_n](float x)
  {
    if (x > dt)
      x = 1.f;
    else if (x > -dt)
      x = talus_n * (x + dt);
    else
      x = 0.f;
    return x;
  };

  if (p_noise != nullptr)
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j] + (*p_noise)(i, j));
  else
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j]);

  return array;
}

Array wave_sine(std::vector<int>   shape,
                float              kw,
                float              angle,
                Array             *p_noise,
                std::vector<float> shift)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(0.f + shift[0], 1.f + shift[0], shape[0]);
  std::vector<float> y = linspace(0.f + shift[1], 1.f + shift[1], shape[1]);
  float              ca = std::cos(angle / 180.f * M_PI);
  float              sa = std::sin(angle / 180.f * M_PI);

  auto lambda = [&kw](float x) { return std::cos(2.f * M_PI * kw * x); };

  if (p_noise != nullptr)
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j] + (*p_noise)(i, j));
  else
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j]);

  return array;
}

Array wave_square(std::vector<int>   shape,
                  float              kw,
                  float              angle,
                  Array             *p_noise,
                  std::vector<float> shift)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(0.f + shift[0], 1.f + shift[0], shape[0]);
  std::vector<float> y = linspace(0.f + shift[1], 1.f + shift[1], shape[1]);
  float              ca = std::cos(angle / 180.f * M_PI);
  float              sa = std::sin(angle / 180.f * M_PI);

  auto lambda = [&kw](float x)
  { return 2.f * (int)(kw * x) - (int)(2.f * kw * x) + 1.f; };

  if (p_noise != nullptr)
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j] + (*p_noise)(i, j));
  else
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j]);

  return array;
}

Array wave_triangular(std::vector<int>   shape,
                      float              kw,
                      float              angle,
                      float              slant_ratio,
                      Array             *p_noise,
                      std::vector<float> shift)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(0.f + shift[0], 1.f + shift[0], shape[0]);
  std::vector<float> y = linspace(0.f + shift[1], 1.f + shift[1], shape[1]);
  float              ca = std::cos(angle / 180.f * M_PI);
  float              sa = std::sin(angle / 180.f * M_PI);

  auto lambda = [&kw, &slant_ratio](float x)
  {
    x = kw * x - (int)(kw * x);
    if (x < slant_ratio)
      x /= slant_ratio;
    else
      x = 1.f - (x - slant_ratio) / (1.f - slant_ratio);
    return x;
  };

  if (p_noise != nullptr)
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j] + (*p_noise)(i, j));
  else
    for (int i = 0; i < array.shape[0]; i++)
      for (int j = 0; j < array.shape[1]; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j]);

  return array;
}

} // namespace hmap
