/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/math.hpp"
#include "highmap/range.hpp"

namespace hmap
{

std::function<float(float, float)> get_distance_function(
    DistanceFunction dist_fct)
{
  std::function<float(float, float)> r_fct;
  float ks = 0.1f; // TODO hardcoded smoothing parameter

  switch (dist_fct)
  {
  case DistanceFunction::CHEBYSHEV:
    r_fct = [ks](float x, float y)
    {
      float r = maximum_smooth(std::abs(x), std::abs(y), ks);
      return r;
    };
    break;

  case DistanceFunction::EUCLIDIAN:
    r_fct = [](float x, float y)
    {
      float r = x * x + y * y;
      return std::sqrt(r);
    };
    break;

  case DistanceFunction::EUCLISHEV:
    r_fct = [ks](float x, float y)
    {
      float rc = maximum_smooth(std::abs(x), std::abs(y), ks);
      rc = rc < 1.f ? rc : 1.f;

      float re = x * x + y * y;
      re = re < 1.f ? std::sqrt(re) : 1.f;

      // add a smoothstep to avoid a gradient discontinuity at re = 1
      re = smoothstep3(re);

      return (1.f - rc) * rc + rc * re;
    };
    break;

  case DistanceFunction::MANHATTAN:
    r_fct = [ks](float x, float y)
    {
      float r = abs_smooth(x, ks) + abs_smooth(y, ks) - ks;
      return r;
    };
    break;
  }

  return r_fct;
}

} // namespace hmap
