/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <random>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/erosion.hpp"
#include "highmap/filters.hpp"
#include "highmap/geometry/grids.hpp"
#include "highmap/kernels.hpp"
#include "highmap/math.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

#include "highmap/geometry/cloud.hpp"

#include "highmap/internal/particles.hpp"

#define HMAP_EROSION_DT 1.f
#define HMAP_EROSION_VOLUME_MIN 0.01f

namespace hmap
{

//----------------------------------------------------------------------
// Main operator(s)
//----------------------------------------------------------------------

void hydraulic_particle(Array &z,
                        int    nparticles,
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
                        bool   post_filtering)
{
  const int ni = z.shape.x;
  const int nj = z.shape.y;
  float     dt = HMAP_EROSION_DT;

  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0, 1);

  // --- initialization

  // keep a backup of the input if the erosion / deposition maps need
  // to be computed
  Array z_bckp = Array();
  if ((p_erosion_map != nullptr) | (p_deposition_map != nullptr)) z_bckp = z;

  // particles spawning positions defined using the moisture map as a density
  // map
  std::vector<float> x0(nparticles);
  std::vector<float> y0(nparticles);
  Array       density = p_moisture_map ? *p_moisture_map : Array(z.shape, 1.f);
  Vec4<float> bbox(1.f, (float)z.shape.x - 2.f, 1.f, (float)z.shape.y - 2.f);

  random_grid_density(x0, y0, density, seed, bbox);

  // spawn particles
  std::vector<Particle> particles;
  particles.reserve(nparticles);

  for (int ip = 0; ip < nparticles; ip++)
  {
    Particle p(c_capacity, c_erosion, c_deposition, c_inertia, drag_rate);
    p.set_xy(x0[ip], y0[ip]);
    p.volume = density(p.pos.i, p.pos.j);

    particles.push_back(p);
  }

  // --- main loop

  // keep track of the number of active particles
  int n_active_particles = nparticles;

  // loop over particles
  while (n_active_particles > 0)
  {
    for (auto &particle : particles)
    {
      if (particle.is_active)
      {
        float z_prev = z.get_value_bilinear_at(particle.pos.i,
                                               particle.pos.j,
                                               particle.pos.u,
                                               particle.pos.v);
        Pos   pos_prev = particle.pos;

        particle.move(z, dt);

        if ((particle.pos.i < 1) or (particle.pos.i > ni - 2) or
            (particle.pos.j < 1) or (particle.pos.j > nj - 2))
        {
          particle.is_active = false;
        }
        else
        {
          float z_next = z.get_value_bilinear_at(particle.pos.i,
                                                 particle.pos.j,
                                                 particle.pos.u,
                                                 particle.pos.v);

          // particle sediment capacity
          float dz = z_prev - z_next;
          float sc = particle.c_capacity * particle.volume * particle.vnorm *
                     dz;
          float delta_sc = dt * (sc - particle.sediment);
          float amount;

          if (delta_sc > 0.f)
            amount = particle.c_erosion * delta_sc; // erosion
          else
            amount = particle.c_deposition * delta_sc; // deposition

          particle.sediment += amount;

          z.depose_amount_bilinear_at(pos_prev.i,
                                      pos_prev.j,
                                      pos_prev.u,
                                      pos_prev.v,
                                      -amount);

          // make sure bedrock is not eroded
          if (p_bedrock)
            z(pos_prev.i,
              pos_prev.j) = std::max(z(pos_prev.i, pos_prev.j),
                                     (*p_bedrock)(pos_prev.i, pos_prev.j));

          particle.volume *= (1 - dt * evap_rate);

          if (particle.volume < HMAP_EROSION_VOLUME_MIN)
            particle.is_active = false;
        }

        if (!particle.is_active) n_active_particles--;
      }
    }
  }

  extrapolate_borders(z);

  // last pass for bedrock
  if (p_bedrock)
    for (int j = 0; j < z.shape.y; j++)
      for (int i = 0; i < z.shape.x; i++)
        z(i, j) = std::max(z(i, j), (*p_bedrock)(i, j));

  // post-filter
  if (post_filtering)
  {
    float sigma = 0.25f;
    int   iterations = 1;
    laplace(z, sigma, iterations);
  }

  // splatmaps
  if (p_erosion_map)
  {
    *p_erosion_map = z_bckp - z;
    clamp_min(*p_erosion_map, 0.f);
  }

  if (p_deposition_map)
  {
    *p_deposition_map = z - z_bckp;
    clamp_min(*p_deposition_map, 0.f);
  }
}

void hydraulic_particle(Array &z,
                        Array *p_mask,
                        int    nparticles,
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
                        bool   post_filtering)
{
  if (!p_mask)
    hydraulic_particle(z,
                       nparticles,
                       seed,
                       p_bedrock,
                       p_moisture_map,
                       p_erosion_map,
                       p_deposition_map,
                       c_capacity,
                       c_erosion,
                       c_deposition,
                       c_inertia,
                       drag_rate,
                       evap_rate,
                       post_filtering);
  else
  {
    Array z_f = z;
    hydraulic_particle(z_f,
                       nparticles,
                       seed,
                       p_bedrock,
                       p_moisture_map,
                       p_erosion_map,
                       p_deposition_map,
                       c_capacity,
                       c_erosion,
                       c_deposition,
                       c_inertia,
                       drag_rate,
                       evap_rate,
                       post_filtering);
    z = lerp(z, z_f, *(p_mask));
  }
}

} // namespace hmap
