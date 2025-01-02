/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>
#include <random>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/operator.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

namespace hmap
{

//----------------------------------------------------------------------
// Helper
//----------------------------------------------------------------------

std::function<float(float)> helper_get_profile_function(
    const ErosionProfile &erosion_profile,
    float                 delta,
    float                &profile_avg)
{
  std::function<float(float)> lambda_p;

  switch (erosion_profile)
  {
  case ErosionProfile::COSINE:
    lambda_p = [](float phi) { return 0.5f - 0.5f * std::cos(phi); };
    break;
  //
  case ErosionProfile::SAW_SHARP:
  {
    lambda_p = [](float phi)
    {
      float t = phi / M_PI;
      t = std::fmod(t + 2.f, 2.f) - 1.f;
      return t - int(t);
    };
  }
  break;
  //
  case ErosionProfile::SAW_SMOOTH:
  {
    float n = 1.f + 0.02f / delta;
    float dn = 2.f * n + 1.f;
    float coeff = std::pow(1.f / dn, 1.f / 2.f / n) * 2.f * n / dn;
    coeff = 1.f / coeff;

    lambda_p = [n, coeff](float phi)
    {
      float t = phi / M_PI;
      t = std::fmod(t + 2.f, 2.f) - 1.f;
      t = coeff * t * (1.f - std::pow(t, 2.f * n));
      t = 0.5f * (1.f + t);
      return t;
    };
  }
  break;
  //
  case ErosionProfile::SHARP_VALLEYS:
  {
    lambda_p = [delta](float phi)
    {
      float t = phi / M_PI;
      t = std::fmod(t + 2.f, 2.f) - 1.f;
      float v = (1.f - t * t) / (1.f + t * t / delta);
      return v;
    };
  }
  break;
  //
  case ErosionProfile::SQUARE_SMOOTH:
  {
    // https://mathematica.stackexchange.com/questions/38293
    lambda_p = [delta](float phi)
    {
      float v = 2.f * std::atan(std::sin(phi) / 25.f / delta) / M_PI;
      return v;
    };
  }
  break;
  //
  case ErosionProfile::TRIANGLE_GRENIER:
  {
    // https://onlinelibrary.wiley.com/doi/epdf/10.1111/cgf.14992
    lambda_p = [delta](float phi)
    {
      float t = phi / M_PI;
      t = std::fmod(t + 2.f, 2.f) - 1.f;

      float value = std::sqrt((1.f + 2.f * std::sqrt(delta)) * t * t + delta) -
                    std::sqrt(delta);
      return value;
    };
  }
  break;
  //
  case ErosionProfile::TRIANGLE_SHARP:
  {
    lambda_p = [](float phi)
    {
      float t = phi / M_PI;
      t = std::fmod(t + 2.f, 2.f) - 1.f;
      return 1.f + std::abs(t);
    };
  }
  break;
  //
  case ErosionProfile::TRIANGLE_SMOOTH:
  {
    // https://mathematica.stackexchange.com/questions/38293
    float coeff = 0.5f / (std::acos(delta - 1.f) / M_PI - 0.5f);

    lambda_p = [delta, coeff](float phi)
    {
      float v = 0.5f +
                coeff *
                    (std::acos((1.f - delta) * std::sin(phi)) / M_PI - 0.5f);
      return v;
    };
  }
  break;
  }

  // average value
  profile_avg = 0.f;
  {
    int                nd = 50;
    std::vector<float> phi = linspace(-M_PI, M_PI, nd);
    for (auto &v : phi)
      profile_avg += lambda_p(v);
    profile_avg /= (float)nd;
  }

  return lambda_p;
}

//----------------------------------------------------------------------
// Main operator
//----------------------------------------------------------------------

void hydraulic_procedural(Array         &z,
                          uint           seed,
                          float          ridge_wavelength,
                          float          ridge_scaling,
                          ErosionProfile erosion_profile,
                          float          delta,
                          float          noise_ratio,
                          int            prefilter_ir,
                          float          density_factor,
                          float          kernel_width_ratio,
                          float          phase_smoothing,
                          float          phase_noise_amp,
                          bool           reverse_phase,
                          bool           rotate90,
                          bool           use_default_mask,
                          float          talus_mask,
                          Array         *p_mask,
                          Array         *p_ridge_mask,
                          float          vmin,
                          float          vmax)
{
  Vec2<int> shape = z.shape;

  // --- setup input parameters

  // define Gabor kernel base parameters
  int   ridge_ir = std::max(1, (int)(ridge_wavelength * shape.x));
  int   width = (int)(kernel_width_ratio * ridge_ir);
  float kw = kernel_width_ratio;

  // rule of thumb scaling of the prefilter... if not provided by the
  // user
  if (prefilter_ir < 0) prefilter_ir = std::max(1, (int)(0.25f * width));

  // redefine min/max if sentinels values are detected
  if (vmax < vmin)
  {
    vmin = z.min();
    vmax = z.max();
  }

  Array zf = z;
  if (prefilter_ir > 0) smooth_cpulse(zf, prefilter_ir);

  // --- compute phase field

  Array gnoise_x;
  Array gnoise_y;

  Array phase = phase_field(z,
                            kw,
                            width,
                            seed,
                            phase_noise_amp,
                            prefilter_ir,
                            density_factor,
                            rotate90,
                            &gnoise_x,
                            &gnoise_y);

  if (reverse_phase) phase *= -1.f;

  // --- apply profile

  float                       profile_avg = 0.f;
  std::function<float(float)> lambda_p = helper_get_profile_function(
      erosion_profile,
      delta,
      profile_avg);

  Array ridges(shape);
  Array rho(shape);

  for (int j = 0; j < shape.y; j++)
    for (int i = 0; i < shape.x; i++)
    {
      rho(i, j) = 2.f / M_PI *
                  std::atan(phase_smoothing *
                            std::hypot(gnoise_x(i, j), gnoise_y(i, j)));

      ridges(i, j) = rho(i, j) * lambda_p(phase(i, j)) +
                     (1.f - rho(i, j)) * profile_avg;
    }

  // --- add noise on the ridge crest lines

  Array noise(shape);

  if (noise_ratio > 0.f)
  {
    Vec2<float> kw_noise = {4.f / ridge_wavelength, 4.f / ridge_wavelength};
    noise = noise_fbm(hmap::NoiseType::PERLIN, shape, kw_noise, ++seed);
    remap(noise, 0.f, noise_ratio);
  }

  Array ridge_mask(shape);

  for (int j = 0; j < shape.y; j++)
    for (int i = 0; i < shape.x; i++)
    {
      ridge_mask(i, j) = ridges(i, j);
      ridges(i, j) += ridge_mask(i, j) * noise(i, j);
    }

  // shift amplitude to "dig" instead of adding elevation
  ridges -= 1.f;

  // --- mask

  Array mask(shape, 1.f);

  if (p_mask)
  {
    mask = *p_mask;
  }
  else if (use_default_mask)
  {
    // default mask is a combination of gradient and mid-range
    // elevation selection
    mask = gradient_norm(zf);

    if (talus_mask == 0.f) talus_mask = 2.f / shape.x;

    for (int j = 0; j < shape.y; j++)
      for (int i = 0; i < shape.x; i++)
      {
        if (mask(i, j) > talus_mask) mask(i, j) = talus_mask;
        mask(i, j) = mask(i, j) / talus_mask;
        mask(i, j) = smoothstep3(mask(i, j));
      }

    Array zn = (z - vmin) / (vmax - vmin);
    mask *= 4.f * zn * (1.f - zn);
  }

  // backup ridge mask as an output field if requested
  if (p_ridge_mask) *p_ridge_mask = ridge_mask * mask;

  // --- eventually apply erosion

  z = lerp(z, z + ridge_scaling * ridges, mask);
}

} // namespace hmap
