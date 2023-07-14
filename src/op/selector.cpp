#define _USE_MATH_DEFINES

#include <cmath>

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap
{

Array select_gt(const Array &array, float value)
{
  Array c = array;
  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
      c(i, j) = c(i, j) > value ? 1.f : 0.f;
  return c;
}

Array select_interval(const Array &array, float value1, float value2)
{
  Array c = array;
  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
    {
      if ((c(i, j) > value1) and (c(i, j) < value2))
        c(i, j) = 1.f;
      else
        c(i, j) = 0.f;
    }
  return c;
}

Array select_lt(const Array &array, float value)
{
  Array c = array;
  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
      c(i, j) = c(i, j) < value ? 1.f : 0.f;
  return c;
}

Array select_blob_log(const Array &array, int ir)
{
  Array c = array;
  smooth_cpulse(c, ir);
  c = -laplacian(c);
  return c;
}

Array select_gradient_angle(const Array &array, float angle)
{
  Array c = gradient_angle(array);
  float alpha = angle / 180.f * M_PI;

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
      c(i, j) = std::max(0.f, std::cos(alpha + c(i, j)));
  return c;
}

Array select_gradient_binary(const Array &array, float talus_center)
{
  Array c = gradient_norm(array);
  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
      c(i, j) = c(i, j) > talus_center ? 1.f : 0.f;
  return c;
}

Array select_gradient_exp(const Array &array,
                          float        talus_center,
                          float        talus_sigma)
{
  Array c = gradient_norm(array);
  c -= talus_center;
  c = exp(-c * c / (2.f * talus_sigma * talus_sigma));
  return c;
}

Array select_gradient_inv(const Array &array,
                          float        talus_center,
                          float        talus_sigma)
{
  Array c = gradient_norm(array);
  c -= talus_center;
  c = 1.f / (1.f + c * c / (talus_sigma * talus_sigma));
  return c;
}

} // namespace hmap
