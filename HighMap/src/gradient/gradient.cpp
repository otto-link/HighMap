/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/gradient.hpp"
#include "highmap/math.hpp"

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

Array gradient_norm(const Array &array, Array *p_dx, Array *p_dy)
{
  Array dx = gradient_x(array);
  Array dy = gradient_y(array);

  // store directional gradients is requested
  if (p_dx)
    *p_dx = dx;
  if (p_dy)
    *p_dy = dy;

  Array dm = hypot(dx, dy);
  return dm;
}

Array gradient_norm_prewitt(const Array &array, Array *p_dx, Array *p_dy)
{
  Array dx = Array(array.shape);
  Array dy = Array(array.shape);

  for (int i = 1; i < array.shape.x - 1; i++)
    for (int j = 1; j < array.shape.y - 1; j++)
    {
      dx(i, j) = array(i + 1, j) - array(i - 1, j);
      dx(i, j) += array(i + 1, j - 1) - array(i - 1, j - 1);
      dx(i, j) += array(i + 1, j + 1) - array(i - 1, j + 1);

      dy(i, j) = array(i, j + 1) - array(i, j - 1);
      dy(i, j) += array(i - 1, j + 1) - array(i - 1, j - 1);
      dy(i, j) += array(i + 1, j + 1) - array(i + 1, j - 1);
    }

  extrapolate_borders(dx);
  extrapolate_borders(dy);

  // store directional gradients is requested
  if (p_dx)
    *p_dx = dx;
  if (p_dy)
    *p_dy = dy;

  // normalize by 1/6th to get the proper gradient amplitudes
  Array dm = hypot(dx, dy) / 6.f;
  return dm;
}

Array gradient_norm_scharr(const Array &array, Array *p_dx, Array *p_dy)
{
  Array dx = Array(array.shape);
  Array dy = Array(array.shape);

  for (int i = 1; i < array.shape.x - 1; i++)
    for (int j = 1; j < array.shape.y - 1; j++)
    {
      dx(i, j) = 10.f * array(i + 1, j) - 10.f * array(i - 1, j);
      dx(i, j) += 3.f * array(i + 1, j - 1) - 3.f * array(i - 1, j - 1);
      dx(i, j) += 3.f * array(i + 1, j + 1) - 3.f * array(i - 1, j + 1);

      dy(i, j) = 10.f * array(i, j + 1) - 10.f * array(i, j - 1);
      dy(i, j) += 3.f * array(i - 1, j + 1) - 3.f * array(i - 1, j - 1);
      dy(i, j) += 3.f * array(i + 1, j + 1) - 3.f * array(i + 1, j - 1);
    }

  extrapolate_borders(dx);
  extrapolate_borders(dy);

  // store directional gradients is requested
  if (p_dx)
    *p_dx = dx;
  if (p_dy)
    *p_dy = dy;

  // normalize by 1/32th to get the proper gradient amplitudes
  Array dm = hypot(dx, dy) / 32.f;
  return dm;
}

Array gradient_norm_sobel(const Array &array, Array *p_dx, Array *p_dy)
{
  Array dx = Array(array.shape);
  Array dy = Array(array.shape);

  for (int i = 1; i < array.shape.x - 1; i++)
    for (int j = 1; j < array.shape.y - 1; j++)
    {
      dx(i, j) = 2.f * array(i + 1, j) - 2.f * array(i - 1, j);
      dx(i, j) += array(i + 1, j - 1) - array(i - 1, j - 1);
      dx(i, j) += array(i + 1, j + 1) - array(i - 1, j + 1);

      dy(i, j) = 2.f * array(i, j + 1) - 2.f * array(i, j - 1);
      dy(i, j) += array(i - 1, j + 1) - array(i - 1, j - 1);
      dy(i, j) += array(i + 1, j + 1) - array(i + 1, j - 1);
    }

  extrapolate_borders(dx);
  extrapolate_borders(dy);

  // store directional gradients is requested
  if (p_dx)
    *p_dx = dx;
  if (p_dy)
    *p_dy = dy;

  // normalize by 1/8th to get the proper gradient amplitudes
  Array dm = hypot(dx, dy) / 8.f;
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
  for (int i = 1; i < array.shape.x - 1; i++)
    for (int j = 0; j < array.shape.y; j++)
      dx(i, j) = 0.5f * (array(i + 1, j) - array(i - 1, j));

  for (int j = 0; j < array.shape.y; j++)
  {
    dx(0, j) = array(1, j) - array(0, j);
    dx(array.shape.x - 1, j) = array(array.shape.x - 1, j) -
                               array(array.shape.x - 2, j);
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
  for (int i = 0; i < array.shape.x; i++)
    for (int j = 1; j < array.shape.y - 1; j++)
      dy(i, j) = 0.5f * (array(i, j + 1) - array(i, j - 1));

  for (int i = 0; i < array.shape.x; i++)
  {
    dy(i, 0) = array(i, 1) - array(i, 0);
    dy(i, array.shape.y - 1) = array(i, array.shape.y - 1) -
                               array(i, array.shape.y - 2);
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
  for (int i = 1; i < talus.shape.x - 1; i += 2)
    for (int j = 0; j < talus.shape.y; j++)
    {
      float d = std::abs(array(i, j) - array(i + 1, j));
      talus(i, j) = std::max(talus(i, j), d);
      talus(i - 1, j) = std::max(talus(i - 1, j), talus(i, j));
      talus(i + 1, j) = d;
    }

  for (int i = 0; i < talus.shape.x; i++)
    for (int j = 1; j < talus.shape.y - 1; j += 2)
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

  for (int i = 1; i < array.shape.x - 1; i++)
    for (int j = 1; j < array.shape.y - 1; j++)
      delta(i, j) = -4.f * array(i, j) + array(i + 1, j) + array(i - 1, j) +
                    array(i, j - 1) + array(i, j + 1);

  extrapolate_borders(delta);
  return delta;
}

} // namespace hmap
