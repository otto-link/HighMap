/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/array.hpp"
#include "highmap/curvature.hpp"
#include "highmap/filters.hpp"
#include "highmap/morphology.hpp"
#include "highmap/range.hpp"

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

Array ruggedness(const Array &array, int ir)
{
  Array rg(array.shape);

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      int i1 = std::max(i - ir, 0);
      int i2 = std::min(i + ir + 1, array.shape.x);
      int j1 = std::max(j - ir, 0);
      int j2 = std::min(j + ir + 1, array.shape.y);

      for (int p = i1; p < i2; p++)
        for (int q = j1; q < j2; q++)
        {
          float delta = array(i, j) - array(p, q);
          rg(i, j) += delta * delta;
        }
    }

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      rg(i, j) = std::sqrt(rg(i, j));

  return rg;
}

Array rugosity(const Array &z, int ir, bool convex)
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

  for (int j = 0; j < z.shape.y; j++)
    for (int i = 0; i < z.shape.x; i++)
      if (z_std(i, j) > tol)
        z_skw(i, j) /= std::pow(z_std(i, j), 1.5f);
      else
        z_skw(i, j) = 0.f;

  // keep only "bumpy" rugosities
  if (convex)
    clamp_min(z_skw, 0.f);
  else
    clamp_max(z_skw, 0.f);

  return z_skw;
}

Array valley_width(const Array &z, int ir, bool ridge_select)
{
  Array vw = z;
  if (ir > 0) smooth_cpulse(vw, ir);

  if (ridge_select) vw *= -1.f;

  vw = curvature_mean(vw);
  vw = distance_transform_approx(vw);

  return vw;
}

} // namespace hmap
