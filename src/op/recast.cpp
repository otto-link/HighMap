/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>
#include <random>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/kernels.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

void recast_canyon(Array &array, const Array &vcut, float gamma)
{
  auto lambda = [&gamma](float a, float b)
  { return a > b ? a : b * std::pow(a / b, gamma); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 vcut.vector.begin(),
                 array.vector.begin(),
                 lambda);
}

void recast_canyon(Array &array, const Array &vcut, Array *p_mask, float gamma)
{
  if (!p_mask)
    recast_canyon(array, vcut, gamma);
  else
  {
    Array array_f = array;
    recast_canyon(array_f, vcut, gamma);
    array = lerp(array, array_f, *(p_mask));
  }
}

void recast_canyon(Array &array, float vcut, float gamma, Array *p_noise)
{
  if (!p_noise)
  {
    auto lambda = [&vcut, &gamma](float a)
    { return a > vcut ? a : vcut * std::pow(a / vcut, gamma); };

    std::transform(array.vector.begin(),
                   array.vector.end(),
                   array.vector.begin(),
                   lambda);
  }
  else
  {
    auto lambda = [&vcut, &gamma](float a, float b)
    { return a > vcut ? a : vcut * std::pow(a / (vcut + b), gamma); };

    std::transform(array.vector.begin(),
                   array.vector.end(),
                   (*p_noise).vector.begin(),
                   array.vector.begin(),
                   lambda);
  }
}

void recast_canyon(Array &array,
                   float  vcut,
                   Array *p_mask,
                   float  gamma,
                   Array *p_noise)
{
  if (!p_mask)
    recast_canyon(array, vcut, gamma, p_noise);
  else
  {
    Array array_f = array;
    recast_canyon(array_f, vcut, gamma, p_noise);
    array = lerp(array, array_f, *(p_mask));
  }
}

void recast_peak(Array &array, int ir, float gamma, float k)
{
  Array ac = array;
  smooth_cpulse(ac, ir);
  array = maximum_smooth(array, ac, k);
  clamp_min(array, 0.f);
  array = ac * pow(array, gamma);
}

void recast_peak(Array &array, int ir, Array *p_mask, float gamma, float k)
{
  if (!p_mask)
    recast_peak(array, ir, gamma, k);
  else
  {
    Array array_f = array;
    recast_peak(array_f, ir, gamma, k);
    array = lerp(array, array_f, *(p_mask));
  }
}

void recast_rocky_slopes(Array &array,
                         float  talus,
                         int    ir,
                         float  amplitude,
                         uint   seed,
                         float  kw,
                         float  gamma,
                         Array *p_noise)
{
  // slope-based criteria
  Array c = select_gradient_binary(array, talus);
  smooth_cpulse(c, ir);

  if (!p_noise)
  {
    Array noise = fbm_perlin(array.shape, {kw, kw}, seed, 4, 0.f);
    gamma_correction_local(noise, gamma, ir, 0.1f);
    {
      int ir2 = (int)(ir / 4.f);
      if (ir2 > 1)
        gamma_correction_local(noise, gamma, ir2, 0.1f);
    }
    array += amplitude * noise * c;
  }
  else
    array += amplitude * (*p_noise) * c;
}

void recast_rocky_slopes(Array &array,
                         float  talus,
                         int    ir,
                         float  amplitude,
                         uint   seed,
                         float  kw,
                         Array *p_mask,
                         float  gamma,
                         Array *p_noise)
{
  {
    if (!p_mask)
      recast_rocky_slopes(array,
                          talus,
                          ir,
                          amplitude,
                          seed,
                          kw,
                          gamma,
                          p_noise);
    else
    {
      Array array_f = array;
      recast_rocky_slopes(array_f,
                          talus,
                          ir,
                          amplitude,
                          seed,
                          kw,
                          gamma,
                          p_noise);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

} // namespace hmap
