/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "dkm.hpp"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/filters.hpp"
#include "highmap/op.hpp"

namespace hmap
{

Array relative_elevation(const Array &array, int ir)
{
  Array amin = minimum_local(array, ir);
  Array amax = maximum_local(array, ir);

  smooth_cpulse(amin, ir);
  smooth_cpulse(amax, ir);

  return (array - amin) / (amax - amin + std::numeric_limits<float>::min());
}

Array rugosity(const Array &z, int ir)
{
  hmap::Array z_avg = Array(z.shape);
  hmap::Array z_std = Array(z.shape);
  hmap::Array z_skw = Array(z.shape);

  // pre high-pass filter to remove low wavenumbers
  Array zf = z;
  smooth_cpulse(zf, 2 * ir);
  zf = z - zf;

  // use Gaussian windowing instead of a real arithmetic averaging to
  // limit boundary artifacts
  z_avg = zf;
  smooth_cpulse(z_avg, ir);

  z_std = (zf - z_avg) * (zf - z_avg);
  smooth_cpulse(z_std, ir);

  // Fisher-Pearson coefficient of skewness
  z_skw = (zf - z_avg) * (zf - z_avg) * (zf - z_avg);

  // do not filter, surprisingly yields much better results
  // smooth_cpulse(z_skw, ir);

  float tol = 1e-30f * z.ptp();

  for (int i = 0; i < z.shape.x; i++)
    for (int j = 0; j < z.shape.y; j++)
      if (z_std(i, j) > tol)
        z_skw(i, j) /= std::pow(z_std(i, j), 1.5f);
      else
        z_skw(i, j) = 0.f;

  // keep only "bumpy" rugosities
  clamp_min(z_skw, 0.f);

  return z_skw;
}

} // namespace hmap
