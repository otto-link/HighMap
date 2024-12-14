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

// Helper for generic gradient function to compute different types of gradients
void compute_gradient(const Array &array,
                      Array       &dx,
                      Array       &dy,
                      float        x_coeff[3],
                      float        y_coeff[3],
                      float        normalize_factor)
{
  for (int j = 1; j < array.shape.y - 1; ++j)
    for (int i = 1; i < array.shape.x - 1; ++i)
    {
      dx(i, j) = x_coeff[0] * (array(i + 1, j) - array(i - 1, j)) +
                 x_coeff[1] * (array(i + 1, j - 1) - array(i - 1, j - 1)) +
                 x_coeff[2] * (array(i + 1, j + 1) - array(i - 1, j + 1));

      dy(i, j) = y_coeff[0] * (array(i, j + 1) - array(i, j - 1)) +
                 y_coeff[1] * (array(i - 1, j + 1) - array(i - 1, j - 1)) +
                 y_coeff[2] * (array(i + 1, j + 1) - array(i + 1, j - 1));
    }

  extrapolate_borders(dx);
  extrapolate_borders(dy);

  dx *= normalize_factor;
  dy *= normalize_factor;
}

Array compute_gradient_norm(const Array &array,
                            float        x_coeff[3],
                            float        y_coeff[3],
                            float        normalize_factor,
                            Array       *p_dx = nullptr,
                            Array       *p_dy = nullptr)
{
  Array dx(array.shape), dy(array.shape);
  compute_gradient(array, dx, dy, x_coeff, y_coeff, normalize_factor);

  if (p_dx) *p_dx = dx;
  if (p_dy) *p_dy = dy;

  return hypot(dx, dy);
}

// functions

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
  if (p_dx) *p_dx = dx;
  if (p_dy) *p_dy = dy;

  Array dm = hypot(dx, dy);
  return dm;
}

Array gradient_norm_prewitt(const Array &array, Array *p_dx, Array *p_dy)
{
  float x_coeff[3] = {1.0f, 1.0f, 1.0f};
  float y_coeff[3] = {1.0f, 1.0f, 1.0f};
  return compute_gradient_norm(array,
                               x_coeff,
                               y_coeff,
                               1.0f / 6.0f,
                               p_dx,
                               p_dy);
}

// Sobel gradient function
Array gradient_norm_sobel(const Array &array, Array *p_dx, Array *p_dy)
{
  float x_coeff[3] = {2.0f, 1.0f, 1.0f};
  float y_coeff[3] = {2.0f, 1.0f, 1.0f};
  return compute_gradient_norm(array,
                               x_coeff,
                               y_coeff,
                               1.0f / 8.0f,
                               p_dx,
                               p_dy);
}

// Scharr gradient function
Array gradient_norm_scharr(const Array &array, Array *p_dx, Array *p_dy)
{
  float x_coeff[3] = {10.0f, 3.0f, 3.0f};
  float y_coeff[3] = {10.0f, 3.0f, 3.0f};
  return compute_gradient_norm(array,
                               x_coeff,
                               y_coeff,
                               1.0f / 32.0f,
                               p_dx,
                               p_dy);
}

Array gradient_x(const Array &array)
{
  Array dx = Array(array.shape);
  gradient_x(array, dx);
  return dx;
}

void gradient_x(const Array &array, Array &dx)
{
  for (int j = 0; j < array.shape.y; j++)
    for (int i = 1; i < array.shape.x - 1; i++)
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
  for (int j = 1; j < array.shape.y - 1; j++)
    for (int i = 0; i < array.shape.x; i++)
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
  for (int j = 0; j < talus.shape.y; j++)
    for (int i = 1; i < talus.shape.x - 1; i += 2)
    {
      float d = std::abs(array(i, j) - array(i + 1, j));
      talus(i, j) = std::max(talus(i, j), d);
      talus(i - 1, j) = std::max(talus(i - 1, j), talus(i, j));
      talus(i + 1, j) = d;
    }

  for (int j = 1; j < talus.shape.y - 1; j += 2)
    for (int i = 0; i < talus.shape.x; i++)
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

  for (int j = 1; j < array.shape.y - 1; j++)
    for (int i = 1; i < array.shape.x - 1; i++)
      delta(i, j) = -4.f * array(i, j) + array(i + 1, j) + array(i - 1, j) +
                    array(i, j - 1) + array(i, j + 1);

  extrapolate_borders(delta);
  return delta;
}

} // namespace hmap
