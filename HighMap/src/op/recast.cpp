/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>
#include <random>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/kernels.hpp"
#include "highmap/math.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

void recast_billow(Array &array, float vref, float k)
{
  array = 2.f * (vref + abs_smooth(array - vref, k)) - 1.f;
}

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
    auto lambda = [&vcut, &gamma](float a, float b) {
      return a > (vcut + b) ? a : (vcut + b) * std::pow(a / (vcut + b), gamma);
    };

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

void recast_cliff(Array &array,
                  float  talus,
                  int    ir,
                  float  amplitude,
                  float  gain)
{
  // scale with gradient regions where the gradient is larger than the
  // reference talus (0 elsewhere)
  Array dn = gradient_norm(array);
  dn -= talus;
  dn *= array.shape.x;
  clamp_min(dn, 0.f);
  smooth_cpulse(dn, ir);

  Array vmin = mean_local(array, ir);
  Array vmax = vmin + amplitude * dn;

  // apply gain filter
  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      if (array(i, j) > vmin(i, j) && array(i, j) < vmax(i, j))
      {
        float vn = (array(i, j) - vmin(i, j)) / (vmax(i, j) - vmin(i, j));
        vn = vn < 0.5 ? 0.5f * std::pow(2.f * vn, gain)
                      : 1.f - 0.5f * std::pow(2.f * (1.f - vn), gain);
        array(i, j) += (vmax(i, j) - vmin(i, j)) * vn;
      }
      else if (array(i, j) >= vmax(i, j))
        array(i, j) += (vmax(i, j) - vmin(i, j));
    }
}

void recast_cliff(Array &array,
                  float  talus,
                  int    ir,
                  float  amplitude,
                  Array *p_mask,
                  float  gain)
{
  if (!p_mask)
    recast_cliff(array, talus, ir, amplitude, gain);
  else
  {
    Array array_f = array;
    recast_cliff(array_f, talus, ir, amplitude, gain);
    array = lerp(array, array_f, *(p_mask));
  }
}

void recast_cliff_directional(Array &array,
                              float  talus,
                              int    ir,
                              float  amplitude,
                              float  angle,
                              float  gain)
{
  float alpha = angle / 180.f * M_PI;

  // scale with gradient regions where the gradient is larger than the
  // reference talus (0 elsewhere)
  Array dn = gradient_norm(array);
  dn -= talus;
  dn *= array.shape.x;
  clamp_min(dn, 0.f);
  smooth_cpulse(dn, ir);

  // orientation scaling
  Array da = gradient_angle(array);
  da -= alpha;
  da = cos(da);
  clamp_min(da, 0.f);
  smooth_cpulse(da, ir);

  Array vmin = mean_local(array, ir);
  Array vmax = vmin + amplitude * dn * da;

  // apply gain filter
  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      if (array(i, j) > vmin(i, j) && array(i, j) < vmax(i, j))
      {
        float vn = (array(i, j) - vmin(i, j)) / (vmax(i, j) - vmin(i, j));
        vn = vn < 0.5 ? 0.5f * std::pow(2.f * vn, gain)
                      : 1.f - 0.5f * std::pow(2.f * (1.f - vn), gain);
        array(i, j) += (vmax(i, j) - vmin(i, j)) * vn;
      }
      else if (array(i, j) >= vmax(i, j))
        array(i, j) += (vmax(i, j) - vmin(i, j));
    }
}

void recast_cliff_directional(Array &array,
                              float  talus,
                              int    ir,
                              float  amplitude,
                              float  angle,
                              Array *p_mask,
                              float  gain)
{
  if (!p_mask)
    recast_cliff_directional(array, talus, ir, amplitude, angle, gain);
  else
  {
    Array array_f = array;
    recast_cliff_directional(array_f, talus, ir, amplitude, angle, gain);
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

void recast_rocky_slopes(Array      &array,
                         float       talus,
                         int         ir,
                         float       amplitude,
                         uint        seed,
                         float       kw,
                         float       gamma,
                         Array      *p_noise,
                         Vec4<float> bbox)
{
  // slope-based criteria
  Array c = select_gradient_binary(array, talus);
  smooth_cpulse(c, ir);

  if (!p_noise)
  {
    Array noise = noise_fbm(NoiseType::PERLIN,
                            array.shape,
                            Vec2<float>(kw, kw),
                            seed,
                            8,
                            0.f,
                            0.5f,
                            2.f,
                            nullptr,
                            nullptr,
                            nullptr,
                            nullptr,
                            bbox);
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

void recast_rocky_slopes(Array      &array,
                         float       talus,
                         int         ir,
                         float       amplitude,
                         uint        seed,
                         float       kw,
                         Array      *p_mask,
                         float       gamma,
                         Array      *p_noise,
                         Vec4<float> bbox)
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
                          p_noise,
                          bbox);
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
                          p_noise,
                          bbox);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

void recast_sag(Array &array, float vref, float k)
{
  array = 0.5f * array + vref - abs_smooth(array - vref, k);
}

} // namespace hmap
