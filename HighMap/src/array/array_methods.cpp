/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/vector.hpp"

#include "op/vector_utils.hpp"

namespace hmap

{

void add_kernel(Array &array, const Array &kernel, int ic, int jc)
{
  // truncate kernel to make it fit into the heightmap array
  int nk_i0 = (int)(std::floor(0.5f * kernel.shape.x)); // left
  int nk_i1 = kernel.shape.x - nk_i0;                   // right
  int nk_j0 = (int)(std::floor(0.5f * kernel.shape.y));
  int nk_j1 = kernel.shape.y - nk_j0;

  int ik0 = std::max(0, nk_i0 - ic);
  int jk0 = std::max(0, nk_j0 - jc);
  int ik1 = std::min(kernel.shape.x,
                     kernel.shape.x - (ic + nk_i1 - array.shape.x));
  int jk1 = std::min(kernel.shape.y,
                     kernel.shape.y - (jc + nk_j1 - array.shape.y));

  // LOG_DEBUG("%d %d", ic, jc);
  // LOG_DEBUG("%d %d %d %d", ik0, ik1, jk0, jk1);

  // where it goes in the array
  int i0 = std::max(ic - nk_i0, 0);
  int j0 = std::max(jc - nk_j0, 0);
  // i1 = std::min(i + nk1, array.shape.x);
  // j1 = std::min(j + nk1, array.shape.y);

  for (int i = ik0; i < ik1; i++)
    for (int j = jk0; j < jk1; j++)
    {
      // int ia = ic + i - ik1;
      // int ja = jc + j - jk1;
      // LOG_DEBUG("%d %d", ia, ja);
      array(i - ik0 + i0, j - jk0 + j0) += kernel(i, j);
    }
}

std::vector<float> Array::col_to_vector(int j)
{
  std::vector<float> vec(this->shape.x);
  for (int i = 0; i < this->shape.x; i++)
    vec[i] = (*this)(i, j);
  return vec;
}

void Array::depose_amount_bilinear_at(int   i,
                                      int   j,
                                      float u,
                                      float v,
                                      float amount)
{
  (*this)(i, j) += amount * (1 - u) * (1 - v);
  (*this)(i + 1, j) += amount * u * (1 - v);
  (*this)(i, j + 1) += amount * (1 - u) * v;
  (*this)(i + 1, j + 1) += amount * u * v;
}

void Array::depose_amount_kernel_bilinear_at(int   i,
                                             int   j,
                                             float u,
                                             float v,
                                             int   ir,
                                             float amount)
{
  Array kernel = Array(Vec2<int>(2 * ir + 1, 2 * ir + 1));

  // compute kernel first
  for (int p = -ir; p < ir + 1; p++)
  {
    for (int q = -ir; q < ir + 1; q++)
    {
      float x = (float)p - u;
      float y = (float)q - v;
      float r = std::max(0.f, 1.f - std::hypot(x, y));
      kernel(p + ir, q + ir) = r;
    }
  }
  kernel.normalize();

  // perform deposition
  this->depose_amount_kernel_at(i, j, kernel, amount);
}

void Array::depose_amount_kernel_at(int i, int j, Array &kernel, float amount)
{
  const int ir = (kernel.shape.x - 1) / 2;
  const int jr = (kernel.shape.y - 1) / 2;

  for (int p = 0; p < kernel.shape.x; p++)
  {
    for (int q = 0; q < kernel.shape.y; q++)
    {
      (*this)(i + p - ir, j + q - jr) += amount * kernel(p, q);
    }
  }
}

Array Array::extract_slice(Vec4<int> idx)
{
  Array array_out = Array(Vec2<int>(idx.b - idx.a, idx.d - idx.c));

  for (int i = idx.a; i < idx.b; i++)
    for (int j = idx.c; j < idx.d; j++)
      array_out(i - idx.a, j - idx.c) = (*this)(i, j);

  return array_out;
}

void fill_array_using_xy_function(
    Array                                    &array,
    std::vector<float>                       &x,
    std::vector<float>                       &y,
    Array                                    *p_noise_x,
    Array                                    *p_noise_y,
    Array                                    *p_stretching,
    std::function<float(float, float, float)> fct_xy)
{
}

void fill_array_using_xy_function(
    Array                                    &array,
    Vec4<float>                               bbox,
    Array                                    *p_noise_x,
    Array                                    *p_noise_y,
    Array                                    *p_stretching,
    std::function<float(float, float, float)> fct_xy)
{
  Vec2<int>          shape = array.shape;
  std::vector<float> x, y;
  grid_xy_vector(x, y, shape, bbox);

  if (p_stretching) // with stretching
  {
    if ((!p_noise_x) and (!p_noise_y))
    {
      for (int i = 0; i < shape.x; i++)
        for (int j = 0; j < shape.y; j++)
          array(i, j) = fct_xy(x[i] * (*p_stretching)(i, j),
                               y[j] * (*p_stretching)(i, j),
                               array(i, j));
    }
    else if (p_noise_x and (!p_noise_y))
    {
      for (int i = 0; i < shape.x; i++)
        for (int j = 0; j < shape.y; j++)
          array(i,
                j) = fct_xy(x[i] * (*p_stretching)(i, j) + (*p_noise_x)(i, j),
                            y[j] * (*p_stretching)(i, j),
                            array(i, j));
    }
    else if ((!p_noise_x) and p_noise_y)
    {
      for (int i = 0; i < shape.x; i++)
        for (int j = 0; j < shape.y; j++)
          array(i,
                j) = fct_xy(x[i] * (*p_stretching)(i, j),
                            y[j] * (*p_stretching)(i, j) + (*p_noise_y)(i, j),
                            array(i, j));
    }
    else if (p_noise_x and p_noise_y)
    {
      for (int i = 0; i < shape.x; i++)
        for (int j = 0; j < shape.y; j++)
          array(i,
                j) = fct_xy(x[i] * (*p_stretching)(i, j) + (*p_noise_x)(i, j),
                            y[j] * (*p_stretching)(i, j) + (*p_noise_y)(i, j),
                            array(i, j));
    }
  }
  else // without stretching
  {
    if ((!p_noise_x) and (!p_noise_y))
    {
      for (int i = 0; i < shape.x; i++)
        for (int j = 0; j < shape.y; j++)
          array(i, j) = fct_xy(x[i], y[j], array(i, j));
    }
    else if (p_noise_x and (!p_noise_y))
    {
      for (int i = 0; i < shape.x; i++)
        for (int j = 0; j < shape.y; j++)
          array(i, j) = fct_xy(x[i] + (*p_noise_x)(i, j), y[j], array(i, j));
    }
    else if ((!p_noise_x) and p_noise_y)
    {
      for (int i = 0; i < shape.x; i++)
        for (int j = 0; j < shape.y; j++)
          array(i, j) = fct_xy(x[i], y[j] + (*p_noise_y)(i, j), array(i, j));
    }
    else if (p_noise_x and p_noise_y)
    {
      for (int i = 0; i < shape.x; i++)
        for (int j = 0; j < shape.y; j++)
          array(i, j) = fct_xy(x[i] + (*p_noise_x)(i, j),
                               y[j] + (*p_noise_y)(i, j),
                               array(i, j));
    }
  }
}

float Array::get_gradient_x_at(int i, int j) const
{
  return 0.5f * ((*this)(i + 1, j) - (*this)(i - 1, j));
}

float Array::get_gradient_y_at(int i, int j) const
{
  return 0.5f * ((*this)(i, j + 1) - (*this)(i, j - 1));
}

float Array::get_gradient_x_bilinear_at(int i, int j, float u, float v) const
{
  float f00 = (*this)(i, j) - (*this)(i - 1, j);
  float f10 = (*this)(i + 1, j) - (*this)(i, j);
  float f01 = (*this)(i, j + 1) - (*this)(i - 1, j + 1);
  float f11 = (*this)(i + 1, j + 1) - (*this)(i, j + 1);

  float a10 = f10 - f00;
  float a01 = f01 - f00;
  float a11 = f11 - f10 - f01 + f00;

  return f00 + a10 * u + a01 * v + a11 * u * v;
}

float Array::get_gradient_y_bilinear_at(int i, int j, float u, float v) const
{
  float f00 = (*this)(i, j) - (*this)(i, j - 1);
  float f10 = (*this)(i + 1, j) - (*this)(i + 1, j - 1);
  float f01 = (*this)(i, j + 1) - (*this)(i, j);
  float f11 = (*this)(i + 1, j + 1) - (*this)(i + 1, j);

  float a10 = f10 - f00;
  float a01 = f01 - f00;
  float a11 = f11 - f10 - f01 + f00;

  return f00 + a10 * u + a01 * v + a11 * u * v;
}

Vec3<float> Array::get_normal_at(int i, int j) const
{
  Vec3<float> normal;

  normal.x = -this->get_gradient_x_at(i, j);
  normal.y = -this->get_gradient_y_at(i, j);
  normal.z = 1.f;

  float norm = std::hypot(normal.x, normal.y, normal.z);
  normal /= norm;

  return normal;
}

size_t Array::get_sizeof() const
{
  return sizeof(float) * this->vector.size();
}

Vec2<float> Array::normalization_coeff(float vmin, float vmax)
{
  float a = 0.f;
  float b = 0.f;
  if (vmin != vmax)
  {
    a = 1.f / (vmax - vmin);
    b = -vmin / (vmax - vmin);
  }
  return Vec2<float>(a, b);
}

float Array::get_value_bilinear_at(int i, int j, float u, float v) const
{
  float a10 = (*this)(i + 1, j) - (*this)(i, j);
  float a01 = (*this)(i, j + 1) - (*this)(i, j);
  float a11 = (*this)(i + 1, j + 1) - (*this)(i + 1, j) - (*this)(i, j + 1) +
              (*this)(i, j);

  return (*this)(i, j) + a10 * u + a01 * v + a11 * u * v;
}

float Array::get_value_nearest(float x, float y, Vec4<float> bbox)
{
  int i = (int)(std::clamp((x - bbox.a) / (bbox.b - bbox.a), 0.f, 1.f) *
                (this->shape.x - 1));
  int j = (int)(std::clamp((y - bbox.c) / (bbox.d - bbox.c), 0.f, 1.f) *
                (this->shape.y - 1));
  return (*this)(i, j);
}

Array hstack(const Array &array1, const Array &array2) // friend function
{
  Array array_out = Array(
      Vec2<int>(array1.shape.x + array2.shape.x, array1.shape.y));

  for (int i = 0; i < array1.shape.x; i++)
    for (int j = 0; j < array1.shape.y; j++)
      array_out(i, j) = array1(i, j);

  for (int i = 0; i < array2.shape.x; i++)
    for (int j = 0; j < array1.shape.y; j++)
      array_out(i + array1.shape.x, j) = array2(i, j);

  return array_out;
}

int Array::linear_index(int i, int j) const
{
  return i * this->shape.y + j;
}

Vec2<int> Array::linear_index_reverse(int k) const
{
  Vec2<int> ij;
  ij.x = std::floor(k / shape.y);
  ij.y = k - ij.x * shape.y;
  return ij;
}

float Array::max() const
{
  return *std::max_element(this->vector.begin(), this->vector.end());
}

float Array::mean() const
{
  return this->sum() / (float)this->size();
};

float Array::min() const
{
  return *std::min_element(this->vector.begin(), this->vector.end());
};

void Array::normalize()
{
  float sum = this->sum();

  std::transform(this->vector.begin(),
                 this->vector.end(),
                 this->vector.begin(),
                 [&sum](float v) { return v / sum; });
}

float Array::ptp() const
{
  return this->max() - this->min();
}

Array Array::resample_to_shape(Vec2<int> new_shape) const
{
  Array array_out = Array(new_shape);

  // interpolation grid scaled to the starting grid to ease seeking of
  // the reference (i, j) indices during bilinear interpolation
  std::vector<float> x = linspace(0.f, (float)this->shape.x - 1, new_shape.x);
  std::vector<float> y = linspace(0.f, (float)this->shape.y - 1, new_shape.y);

  for (int i = 0; i < new_shape.x; i++)
  {
    int iref = (int)x[i];
    for (int j = 0; j < new_shape.y; j++)
    {
      int   jref = (int)y[j];
      float u = x[i] - (float)iref;
      float v = y[j] - (float)jref;

      // handle bordline cases
      if (iref == this->shape.x - 1)
      {
        iref = this->shape.x - 2;
        u = 1.f;
      }

      if (jref == this->shape.y - 1)
      {
        jref = this->shape.y - 2;
        v = 1.f;
      }

      array_out(i, j) = this->get_value_bilinear_at(iref, jref, u, v);
    }
  }

  return array_out;
}

Array Array::resample_to_shape_nearest(Vec2<int> new_shape) const
{
  Array array_out = Array(new_shape);

  // interpolation grid scaled to the starting grid to ease seeking of
  // the reference (i, j) indices during bilinear interpolation
  std::vector<float> x = linspace(0.f, (float)this->shape.x - 1, new_shape.x);
  std::vector<float> y = linspace(0.f, (float)this->shape.y - 1, new_shape.y);

  for (int i = 0; i < new_shape.x; i++)
  {
    int iref = (int)std::floor(x[i]);
    for (int j = 0; j < new_shape.y; j++)
    {
      int jref = (int)std::floor(y[j]);
      array_out(i, j) = (*this)(iref, jref);
    }
  }

  return array_out;
}

std::vector<float> Array::row_to_vector(int i)
{
  std::vector<float> vec(this->shape.y);
  for (int j = 0; j < this->shape.y; j++)
    vec[j] = (*this)(i, j);
  return vec;
}

void Array::set_slice(Vec4<int> idx, float value)
{
  for (int i = idx.a; i < idx.b; i++)
    for (int j = idx.c; j < idx.d; j++)
      (*this)(i, j) = value;
}

int Array::size() const
{
  return this->shape.x * this->shape.y;
}

float Array::sum() const
{
  return std::accumulate(this->vector.begin(), this->vector.end(), 0.f);
}

std::vector<float> Array::unique_values()
{
  std::vector<float> v = this->vector;
  vector_unique_values(v);
  return v;
}

Array vstack(const Array &array1, const Array &array2) // friend function
{
  Array array_out = Array(
      Vec2<int>(array1.shape.x, array1.shape.y + array2.shape.y));

  for (int i = 0; i < array1.shape.x; i++)
  {
    for (int j = 0; j < array1.shape.y; j++)
      array_out(i, j) = array1(i, j);

    for (int j = 0; j < array2.shape.y; j++)
      array_out(i, j + array1.shape.y) = array2(i, j);
  }

  return array_out;
}

} // namespace hmap
