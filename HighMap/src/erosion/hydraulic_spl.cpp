/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "highmap/array.hpp"
#include "highmap/gradient.hpp"
#include "highmap/hydrology.hpp"
#include "highmap/math.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

void hydraulic_spl(Array &z,
                   float  c_erosion,
                   float  talus_ref,
                   int    iterations,
                   Array *p_bedrock,
                   Array *p_moisture_map,
                   Array *p_erosion_map,
                   int    ir)
{
  Array facc = Array(z.shape);
  Array dz = Array(z.shape);

  // keep a backup of the input if the erosion / deposition maps need
  // to be computed
  Array z_bckp = Array();
  if (p_erosion_map)
    z_bckp = z;

  // default bedrock
  Array z_bedrock = Array();
  if (!p_bedrock)
    z_bedrock = hmap::minimum_local(z, 8 * ir);

  // main loop
  for (int it = 0; it < iterations; it++)
  {
    facc = flow_accumulation_dinf(z, talus_ref);

    // prefilter heightmap for the gradient norm
    {
      auto zf = z;
      smooth_cpulse(zf, ir);
      dz = gradient_norm(zf) * z.shape.x;
    }

    if (p_moisture_map)
      z += -c_erosion * (*p_moisture_map) * pow(facc, 0.8f) * dz * dz;
    else
      z += -c_erosion * pow(facc, 0.8f) * dz * dz;
    
    if (p_bedrock)
      z = maximum(z, *p_bedrock);
    else
      z = maximum(z, z_bedrock);
  }

  // splatmaps
  if (p_erosion_map)
  {
    *p_erosion_map = z_bckp - z;
    clamp_min(*p_erosion_map, 0.f);
  }
}

void hydraulic_spl(Array &z,
                   Array *p_mask,
                   float  c_erosion,
                   float  talus_ref,
                   int    iterations,
                   Array *p_bedrock,
                   Array *p_moisture_map,
                   Array *p_erosion_map,
                   int    ir)
{
  if (!p_mask)
    hydraulic_spl(z,
                  c_erosion,
                  talus_ref,
                  iterations,
                  p_bedrock,
                  p_moisture_map,
                  p_erosion_map,
                  ir);
  else
  {
    Array z_f = z;
    hydraulic_spl(z_f,
                  c_erosion,
                  talus_ref,
                  iterations,
                  p_bedrock,
                  p_moisture_map,
                  p_erosion_map,
                  ir);
    z = lerp(z, z_f, *(p_mask));
  }
}

} // namespace hmap
