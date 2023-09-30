/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>
#include <random>

#include "Interpolate.hpp"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/kernels.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

void recurve(Array                    &array,
             const std::vector<float> &t,
             const std::vector<float> &v)
{
  _1D::MonotonicInterpolator<float> interp;
  interp.setData(t, v);

  auto lambda = [&interp](float a) { return interp(a); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void recurve(Array                    &array,
             const std::vector<float> &t,
             const std::vector<float> &v,
             Array                    *p_mask)
{
  {
    if (!p_mask)
      recurve(array, t, v);
    else
    {
      Array array_f = array;
      recurve(array_f, t, v);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

void recurve_bexp(Array &array, float tau)
{
  float c = -1.f / tau;
  auto  lambda = [&c](float a) { return 1.f - std::exp(c * a); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void recurve_bexp(Array &array, float tau, Array *p_mask)
{
  {
    if (!p_mask)
      recurve_bexp(array, tau);
    else
    {
      Array array_f = array;
      recurve_bexp(array_f, tau);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

void recurve_exp(Array &array, float tau)
{
  float c = -1.f / tau;
  auto  lambda = [&c](float a) { return std::exp(c * (1.f - a)); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void recurve_exp(Array &array, float tau, Array *p_mask)
{
  {
    if (!p_mask)
      recurve_exp(array, tau);
    else
    {
      Array array_f = array;
      recurve_exp(array_f, tau);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

void recurve_kura(Array &array, float a, float b)
{
  auto lambda = [&a, &b](float v)
  { return 1.f - std::pow(1.f - std::pow(v, a), b); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void recurve_kura(Array &array, float a, float b, Array *p_mask)
{
  {
    if (!p_mask)
      recurve_kura(array, a, b);
    else
    {
      Array array_f = array;
      recurve_kura(array_f, a, b);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

void recurve_s(Array &array)
{
  auto lambda = [](float a) { return a * a * (3.f - 2.f * a); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void recurve_s(Array &array, Array *p_mask)
{
  {
    if (!p_mask)
      recurve_s(array);
    else
    {
      Array array_f = array;
      recurve_s(array_f);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

void recurve_smoothstep_rational(Array &array, float n)
{
  auto lambda = [&n](float a)
  {
    float an = std::pow(a, n);
    return an / (an + std::pow(1.f - a, n));
  };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void recurve_smoothstep_rational(Array &array, float n, Array *p_mask)
{
  {
    if (!p_mask)
      recurve_smoothstep_rational(array, n);
    else
    {
      Array array_f = array;
      recurve_smoothstep_rational(array_f, n);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

} // namespace hmap
