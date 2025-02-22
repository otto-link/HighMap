/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <random>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/math.hpp"
#include "highmap/multiscale/pyramid.hpp"
#include "highmap/range.hpp"

namespace hmap
{

void hydraulic_particle_multiscale(Array &z,
                                   float  particle_density,
                                   int    seed,
                                   Array *p_bedrock,
                                   Array *p_moisture_map,
                                   Array *p_erosion_map,
                                   Array *p_deposition_map,
                                   float  c_capacity,
                                   float  c_erosion,
                                   float  c_deposition,
                                   float  c_inertia,
                                   float  drag_rate,
                                   float  evap_rate,
                                   int    pyramid_finest_level)
{

  // --- 'transform' function
  auto fct =
      [&seed,
       &particle_density,
       p_bedrock,
       p_moisture_map,
       &c_capacity,
       &c_erosion,
       &c_deposition,
       &c_inertia,
       &drag_rate,
       &evap_rate](const hmap::Array &input, const int /* current_level */)
  {
    // LOG_DEBUG("applying erosion to level: %d, shape: {%d, %d}",
    //           current_level,
    //           input.shape.x,
    //           input.shape.y);

    hmap::Array output = input;
    int         nparticles = (int)(particle_density * input.size());
    hydraulic_particle(output,
                       nparticles,
                       ++seed,
                       p_bedrock,
                       p_moisture_map,
                       nullptr, // skip erosion map output
                       nullptr, // idem deposition
                       c_capacity,
                       c_erosion,
                       c_deposition,
                       c_inertia,
                       drag_rate,
                       evap_rate);
    return output;
  };

  // --- pyramid decomposition

  // don't go to deep (== "coarse mesh") in the pyramid levels to
  // avoid arrays with few cells. Settings nlevel to -4 stop the
  // pyramid at arrays of size 32 x 32
  int nlevels = -4;

  PyramidDecomposition pyr = PyramidDecomposition(z, nlevels);
  pyr.decompose();

  // apply the erosion filter and recompose the pyramid
  Array ze = pyr.transform(fct,
                           pyramid_transform_support::FULL,
                           {}, // uniform weights
                           pyramid_finest_level);

  // --- splatmaps
  if (p_erosion_map)
  {
    *p_erosion_map = z - ze;
    clamp_min(*p_erosion_map, 0.f);
  }

  if (p_deposition_map)
  {
    *p_deposition_map = ze - z;
    clamp_min(*p_deposition_map, 0.f);
  }

  z = ze;
}

} // namespace hmap
