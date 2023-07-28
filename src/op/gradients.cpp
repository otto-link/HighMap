/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap
{

Array gradient_angle(const Array &array, bool downward)
{
  Array dx = gradient_x(array);
  Array dy = gradient_y(array);
  Array alpha = Array(array.shape);

  if (downward)
  {
    dx = -1.f * dx;
    dy = -1.f * dy;
  }

  std::transform(dx.vector.begin(),
                 dx.vector.end(),
                 dy.vector.begin(),
                 alpha.vector.begin(),
                 [](float a, float b) { return std::atan2(b, a); });

  return alpha;
}

Array gradient_norm(const Array &array)
{
  Array dx = gradient_x(array);
  Array dy = gradient_y(array);
  Array dm = hypot(dx, dy);
  return dm;
}

Array gradient_x(const Array &array)
{
  Array dx = Array(array.shape);
  gradient_x(array, dx);
  return dx;
}

void gradient_x(const Array &array, Array &dx)
{
  for (int i = 1; i < array.shape[0] - 1; i++)
    for (int j = 0; j < array.shape[1]; j++)
      dx(i, j) = 0.5f * (array(i + 1, j) - array(i - 1, j));

  for (int j = 0; j < array.shape[1]; j++)
  {
    dx(0, j) = array(1, j) - array(0, j);
    dx(array.shape[0] - 1, j) = array(array.shape[0] - 1, j) -
                                array(array.shape[0] - 2, j);
  }
}

Array gradient_y(const Array &array)
{
  Array dy = Array(array.shape);
  gradient_y(array, dy);
  return dy;
}

void gradient_y(const Array &array, Array &dy)
{
  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 1; j < array.shape[1] - 1; j++)
      dy(i, j) = 0.5f * (array(i, j + 1) - array(i, j - 1));

  for (int i = 0; i < array.shape[0]; i++)
  {
    dy(i, 0) = array(i, 1) - array(i, 0);
    dy(i, array.shape[1] - 1) = array(i, array.shape[1] - 1) -
                                array(i, array.shape[1] - 2);
  }
}

Array gradient_talus(const Array &array)
{
  Array talus = Array(array.shape);
  gradient_talus(array, talus);
  return talus;
}

void gradient_talus(const Array &array, Array &talus)
{
  for (int i = 1; i < talus.shape[0] - 1; i += 2)
    for (int j = 0; j < talus.shape[1]; j++)
    {
      float d = std::abs(array(i, j) - array(i + 1, j));
      talus(i, j) = std::max(talus(i, j), d);
      talus(i - 1, j) = std::max(talus(i - 1, j), talus(i, j));
      talus(i + 1, j) = d;
    }

  for (int i = 0; i < talus.shape[0]; i++)
    for (int j = 1; j < talus.shape[1] - 1; j += 2)
    {
      float d = std::abs(array(i, j) - array(i, j + 1));
      talus(i, j) = std::max(talus(i, j), d);
      talus(i, j - 1) = std::max(talus(i, j - 1), talus(i, j));
      talus(i, j + 1) = d;
    }
}

Array laplacian(const Array &array)
{
  Array delta = Array(array.shape);

  for (int i = 1; i < array.shape[0] - 1; i++)
    for (int j = 1; j < array.shape[1] - 1; j++)
      delta(i, j) = -4.f * array(i, j) + array(i + 1, j) + array(i - 1, j) +
                    array(i, j - 1) + array(i, j + 1);

  extrapolate_borders(delta);
  return delta;
}

} // namespace hmap
