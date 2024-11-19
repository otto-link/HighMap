/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/kernels.hpp"
#include "highmap/operator.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

namespace hmap
{

Array phase_field(const Array &array,
                  float        kw,
                  int          width,
                  uint         seed,
                  float        noise_amp,
                  int          prefilter_ir,
                  float        density_factor,
                  bool         rotate90,
                  Array       *p_gnoise_x,
                  Array       *p_gnoise_y)
{
  Vec2<int>                       shape = array.shape;
  std::mt19937                    gen(seed);
  std::uniform_int_distribution<> dis_i(0, shape.x - 1);
  std::uniform_int_distribution<> dis_j(0, shape.y - 1);

  // rule of thumb scaling of the prefilter... if not provided by the
  // user
  if (prefilter_ir < 0) prefilter_ir = std::max(1, (int)(0.25f * width));

  Array arrayf = array;
  if (prefilter_ir > 0) smooth_cpulse(arrayf, prefilter_ir);

  // compute phase field
  float phi = rotate90 ? M_PI : 0.5 * M_PI;
  Array theta = gradient_angle(arrayf) + phi;

  // generate Gabor noise with spatially varying parameters
  float density = density_factor * 5.f / (float)(width * width);
  int   npoints = (int)(density * shape.x * shape.y);
  Array gnoise_x(shape);
  Array gnoise_y(shape);

  for (int k = 0; k < npoints; k++)
  {
    int i = dis_i(gen);
    int j = dis_j(gen);

    float angle = theta(i, j) * 180.f / M_PI;
    Array kernel;

    Vec2<int> kernel_shape(width, width);

    kernel = gabor(kernel_shape, kw, angle);
    add_kernel(gnoise_x, kernel, i, j);

    kernel = gabor(kernel_shape, kw, angle, true);
    add_kernel(gnoise_y, kernel, i, j);
  }

  // output if requested
  if (p_gnoise_x) *p_gnoise_x = gnoise_x;
  if (p_gnoise_y) *p_gnoise_y = gnoise_y;

  // phase field
  Array phase = atan2(gnoise_y, gnoise_x);

  // add noise if requested
  if (noise_amp)
  {
    int   octaves = 4;
    float kw_noise = kw * shape.x / width;
    Array phase_noise = noise_fbm(hmap::NoiseType::PERLIN,
                                  shape,
                                  {kw_noise, kw_noise},
                                  ++seed,
                                  octaves);
    remap(phase_noise, -noise_amp, noise_amp);
    phase += phase_noise;
  }

  return phase;
}

} // namespace hmap
