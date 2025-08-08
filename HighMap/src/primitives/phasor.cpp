/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry/point_sampling.hpp"
#include "highmap/kernels.hpp"
#include "highmap/math.hpp"
#include "highmap/operator.hpp"
#include "highmap/range.hpp"

namespace hmap
{

Array phasor(PhasorProfile phasor_profile,
             Vec2<int>     shape,
             float         kw,
             const Array  &angle,
             uint          seed,
             float         profile_delta,
             float         density_factor,
             float         kernel_width_ratio,
             float         phase_smoothing)
{
  //

  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0.f, 1.f);

  // define Gabor kernel base parameters
  int   phase_ir = std::max(1, (int)(shape.x / kw));
  int   width = (int)(kernel_width_ratio * phase_ir);
  float kw_kernel = kernel_width_ratio;

  // if the kernel support is too small, return an zeroed array
  if (width < 4) return Array(shape);

  // Gabor noise
  float density = density_factor * 20.f / (float)(width * width);
  int   npoints = (int)(density * shape.x * shape.y);
  Array gnoise_x(shape);
  Array gnoise_y(shape);

  // generate Gabor kernel "spawn" points
  std::vector<float> x(npoints), y(npoints);
  Vec4<float>        bbox(0.f, (float)shape.x - 1.f, 0.f, (float)shape.y - 1.f);

  const Vec2<float> jitter_amount = {0.5f, 0.5f};
  const Vec2<float> stagger_ratio = {0.f, 0.f};

  auto xy = random_points_jittered(npoints,
                                   jitter_amount,
                                   stagger_ratio,
                                   seed,
                                   bbox);
  x = xy[0];
  y = xy[1];

  for (int k = 0; k < npoints; k++)
  {
    int i = x[k];
    int j = y[k];

    Array kernel;

    Vec2<int> kernel_shape(width, width);

    kernel = gabor(kernel_shape, kw_kernel, angle(i, j));
    add_kernel(gnoise_x, kernel, i, j);

    kernel = gabor(kernel_shape, kw_kernel, angle(i, j), true);
    add_kernel(gnoise_y, kernel, i, j);
  }

  // phase field
  Array phase = atan2(gnoise_y, gnoise_x);

  // apply phase profile
  float profile_avg;
  auto  lambda_p = get_phasor_profile_function(phasor_profile,
                                              profile_delta,
                                              &profile_avg);

  Array phasor_noise(shape);

  if (phase_smoothing > 0.f)
  {
    for (int j = 0; j < shape.y; j++)
      for (int i = 0; i < shape.x; i++)
      {
        float rho = 2.f / M_PI *
                    std::atan(phase_smoothing *
                              std::hypot(gnoise_x(i, j), gnoise_y(i, j)));

        phasor_noise(i, j) = rho * lambda_p(phase(i, j)) +
                             (1.f - rho) * profile_avg;
      }
  }
  else
  {
    for (int j = 0; j < shape.y; j++)
      for (int i = 0; i < shape.x; i++)
        phasor_noise(i, j) = lambda_p(phase(i, j));
  }

  // return phase;
  return phasor_noise;
}

Array phasor_fbm(PhasorProfile phasor_profile,
                 Vec2<int>     shape,
                 float         kw,
                 const Array  &angle,
                 uint          seed,
                 float         profile_delta,
                 float         density_factor,
                 float         kernel_width_ratio,
                 float         phase_smoothing,
                 int           octaves,
                 float         weight,
                 float         persistence,
                 float         lacunarity)
{
  // initial amplitude
  float amp = persistence;
  float amp_fractal = 1.f;
  for (int i = 1; i < octaves; i++)
  {
    amp_fractal += amp;
    amp *= persistence;
  }
  float amp0 = 1.f / amp_fractal;

  // fbm layering
  Array famp(shape, amp0);
  Array sum(shape);
  float kw_factor = 1.f;

  for (int k = 0; k < octaves; k++)
  {
    Array value = phasor(phasor_profile,
                         shape,
                         kw_factor * kw,
                         angle,
                         seed++,
                         profile_delta,
                         density_factor,
                         kernel_width_ratio,
                         phase_smoothing);

    sum += value * famp;
    famp *= (1.f - weight) + weight * 0.5f * minimum(value + 1.f, 2.f);
    famp *= persistence;
    kw_factor *= lacunarity;
  }

  return sum;
}

} // namespace hmap
