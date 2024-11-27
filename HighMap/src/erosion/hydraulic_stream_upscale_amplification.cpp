/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/math.hpp"
#include "highmap/multiscale/upscaling.hpp"

namespace hmap
{

void hydraulic_stream_upscale_amplification(Array &z,
                                            float  c_erosion,
                                            float  talus_ref,
                                            int    upscaling_levels,
                                            float  persistence,
                                            int    ir,
                                            float  clipping_ratio)
{
  auto lambda_erode =
      [c_erosion, talus_ref, ir, clipping_ratio](Array &x,
                                                 float  current_scaling)
  {
    hydraulic_stream(x,
                     current_scaling * c_erosion,
                     talus_ref,
                     nullptr,
                     nullptr,
                     nullptr,
                     ir,
                     clipping_ratio);
  };

  upscale_amplification(z, upscaling_levels, persistence, lambda_erode);
}

void hydraulic_stream_upscale_amplification(Array &z,
                                            Array *p_mask,
                                            float  c_erosion,
                                            float  talus_ref,
                                            int    upscaling_levels,
                                            float  persistence,
                                            int    ir,
                                            float  clipping_ratio)
{
  if (!p_mask)
    hydraulic_stream_upscale_amplification(z,
                                           c_erosion,
                                           talus_ref,
                                           upscaling_levels,
                                           persistence,
                                           ir,
                                           clipping_ratio);
  else
  {
    Array z_f = z;
    hydraulic_stream_upscale_amplification(z_f,
                                           c_erosion,
                                           talus_ref,
                                           upscaling_levels,
                                           persistence,
                                           ir,
                                           clipping_ratio);
    z = lerp(z, z_f, *(p_mask));
  }
}

} // namespace hmap
