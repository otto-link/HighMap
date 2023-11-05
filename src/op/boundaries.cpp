/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap
{

void extrapolate_borders(Array &array, int nbuffer)
{
  const int ni = array.shape.x;
  const int nj = array.shape.y;

  for (int j = 0; j < nj; j++)
    for (int k = nbuffer - 1; k > -1; k--)
    {
      array(k, j) = 2.f * array(k + 1, j) - array(k + 2, j);
      array(ni - 1 - k, j) = 2.f * array(ni - 2 - k, j) - array(ni - 3 - k, j);
    }

  for (int i = 0; i < ni; i++)
    for (int k = nbuffer - 1; k > -1; k--)
    {
      array(i, k) = 2.f * array(i, k + 1) - array(i, k + 2);
      array(i, nj - 1 - k) = 2.f * array(i, nj - 2 - k) - array(i, nj - 3 - k);
    }
}

void fill_borders(Array &array)
{
  const int ni = array.shape.x;
  const int nj = array.shape.y;

  for (int j = 0; j < nj; j++)
  {
    array(0, j) = array(1, j);
    array(ni - 1, j) = array(ni - 2, j);
  }

  for (int i = 0; i < ni; i++)
  {
    array(i, 0) = array(i, 1);
    array(i, nj - 1) = array(i, nj - 2);
  }
}

Array generate_buffered_array(const Array &array, Vec4<int> buffers)
{
  Array array_out = Array(Vec2<int>(array.shape.x + buffers.a + buffers.b,
                                    array.shape.y + buffers.c + buffers.d));

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
      array_out(i + buffers.a, j + buffers.c) = array(i, j);

  int i1 = buffers.a;
  int i2 = buffers.b;
  int j1 = buffers.c;
  int j2 = buffers.d;

  for (int i = 0; i < i1; i++)
    for (int j = j1; j < array_out.shape.y - j2; j++)
      array_out(i, j) = array_out(2 * i1 - i, j);

  for (int i = array_out.shape.x - i2; i < array_out.shape.x; i++)
    for (int j = j1; j < array_out.shape.y - j2; j++)
      array_out(i, j) = array_out(2 * (array_out.shape.x - i2) - i - 1, j);

  for (int i = 0; i < array_out.shape.x; i++)
    for (int j = 0; j < j1; j++)
      array_out(i, j) = array_out(i, 2 * j1 - j);

  for (int i = 0; i < array_out.shape.x; i++)
    for (int j = array_out.shape.y - j2; j < array_out.shape.y; j++)
      array_out(i, j) = array_out(i, 2 * (array_out.shape.y - j2) - j - 1);

  return array_out;
}

void make_periodic(Array &array, int nbuffer)
{
  int ni = array.shape.x;
  int nj = array.shape.y;

  Array a1 = array;
  for (int i = 0; i < nbuffer; i++)
  {
    float r = 0.5f * (float)i / ((float)nbuffer - 1.f);
    for (int j = 0; j < nj; j++)
    {
      a1(i, j) = (0.5f + r) * array(i, j) + (0.5f - r) * array(ni - 1 - i, j);
      a1(ni - 1 - i, j) = (0.5f + r) * array(ni - 1 - i, j) +
                          (0.5f - r) * array(i, j);
    }
  }

  Array a2 = a1;
  for (int j = 0; j < nbuffer; j++)
  {
    float r = 0.5f * (float)j / ((float)nbuffer - 1);
    for (int i = 0; i < ni; i++)
    {
      a2(i, j) = (0.5 + r) * a1(i, j) + (0.5 - r) * a1(i, nj - 1 - j);
      a2(i, nj - 1 - j) = (0.5 + r) * a1(i, nj - 1 - j) + (0.5 - r) * a1(i, j);
    }
  }

  array = a2;
}

void set_borders(Array      &array,
                 Vec4<float> border_values,
                 Vec4<int>   buffer_sizes)
{
  // west
  for (int i = 0; i < buffer_sizes.a; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      float r = (float)i / (float)buffer_sizes.a;
      r = r * r * (3.f - 2.f * r);
      array(i, j) = (1.f - r) * border_values.a + r * array(i, j);
    }

  // east
  for (int i = array.shape.x - buffer_sizes.b; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      float r = 1.f - (float)(i - array.shape.x + buffer_sizes.b) /
                          (float)buffer_sizes.b;
      r = r * r * (3.f - 2.f * r);
      array(i, j) = (1.f - r) * border_values.b + r * array(i, j);
    }

  // south
  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < buffer_sizes.c; j++)
    {
      float r = (float)j / (float)buffer_sizes.c;
      r = r * r * (3.f - 2.f * r);
      array(i, j) = (1.f - r) * border_values.c + r * array(i, j);
    }

  // north
  for (int i = 0; i < array.shape.x; i++)
    for (int j = array.shape.y - buffer_sizes.d; j < array.shape.y; j++)
    {
      float r = 1.f - (float)(j - array.shape.y + buffer_sizes.d) /
                          (float)buffer_sizes.d;
      r = r * r * (3.f - 2.f * r);
      array(i, j) = (1.f - r) * border_values.d + r * array(i, j);
    }
}

void set_borders(Array &array, float border_values, int buffer_sizes)
{
  Vec4<float> bv =
      Vec4<float>(border_values, border_values, border_values, border_values);
  Vec4<int> bs =
      Vec4<int>(buffer_sizes, buffer_sizes, buffer_sizes, buffer_sizes);
  set_borders(array, bv, bs);
}

void sym_borders(Array &array, Vec4<int> buffer_sizes)
{
  const int i1 = buffer_sizes.a;
  const int i2 = buffer_sizes.b;
  const int j1 = buffer_sizes.c;
  const int j2 = buffer_sizes.d;

  // fill-in the blanks...
  for (int i = 0; i < i1; i++)
  {
    for (int j = j1; j < array.shape.y - j2; j++)
    {
      array(i, j) = array(2 * i1 - i, j);
    }
  }

  for (int i = array.shape.x - i2; i < array.shape.x; i++)
  {
    for (int j = j1; j < array.shape.y - j2; j++)
    {
      array(i, j) = array(2 * (array.shape.x - i2) - i - 1, j);
    }
  }

  for (int i = 0; i < array.shape.x; i++)
  {
    for (int j = 0; j < j1; j++)
    {
      array(i, j) = array(i, 2 * j1 - j);
    }
  }

  for (int i = 0; i < array.shape.x; i++)
  {
    for (int j = array.shape.y - j2; j < array.shape.y; j++)
    {
      array(i, j) = array(i, 2 * (array.shape.y - j2) - j - 1);
    }
  }
}

void zeroed_borders(Array &array)
{
  const int ni = array.shape.x;
  const int nj = array.shape.y;

  for (int j = 0; j < nj; j++)
  {
    array(0, j) = 0.f;
    array(ni - 1, j) = 0.f;
  }

  for (int i = 0; i < ni; i++)
  {
    array(i, 0) = 0.f;
    array(i, nj - 1) = 0.f;
  }
}

void zeroed_edges(Array      &array,
                  float       sigma,
                  Array      *p_noise,
                  Vec2<float> shift,
                  Vec2<float> scale)
{
  std::vector<float> x =
      linspace(shift.x - 0.5f, shift.x - 0.5f + scale.x, array.shape.x, false);
  std::vector<float> y =
      linspace(shift.y - 0.5f, shift.y - 0.5f + scale.y, array.shape.y, false);

  float s = 0.5f / (sigma * sigma);

  if (!p_noise)
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float r2 = x[i] * x[i] + y[j] * y[j];
        array(i, j) *= std::exp(-s * r2);
      }
  else
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float r2 = x[i] * x[i] + y[j] * y[j] +
                   (*p_noise)(i, j) * (*p_noise)(i, j);
        array(i, j) *= std::exp(-s * r2);
      }
}

} // namespace hmap
