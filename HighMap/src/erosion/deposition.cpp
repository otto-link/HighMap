/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/erosion.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/kernels.hpp"
#include "highmap/math.hpp"
#include "highmap/range.hpp"
#include "highmap/selector.hpp"

#define SPAWN_LOW_LIMIT 0.1f
#define GRADIENT_MIN 0.0001f
#define SEDIMENT_MIN 0.001f
#define VELOCITY_MIN 0.001f

namespace hmap
{

void sediment_deposition(Array       &z,
                         const Array &talus,
                         Array       *p_deposition_map,
                         float        max_deposition,
                         int          iterations,
                         int          thermal_subiterations)
{
  float deposition_step = max_deposition / (int)iterations;
  Array smap = Array(z.shape); // sediment map

  for (int it = 0; it < iterations; it++)
  {
    smap = smap + deposition_step;
    Array z_tot = z + smap;

    thermal(z_tot, talus, thermal_subiterations, &z);
    smap = z_tot - z;
  }
  z = z + smap;

  if (p_deposition_map) *p_deposition_map = smap;
}

void sediment_deposition(Array       &z,
                         Array       *p_mask,
                         const Array &talus,
                         Array       *p_deposition_map,
                         float        max_deposition,
                         int          iterations,
                         int          thermal_subiterations)
{
  if (!p_mask)
    sediment_deposition(z,
                        talus,
                        p_deposition_map,
                        max_deposition,
                        iterations,
                        thermal_subiterations);
  else
  {
    Array z_f = z;
    sediment_deposition(z_f,
                        talus,
                        p_deposition_map,
                        max_deposition,
                        iterations,
                        thermal_subiterations);
    z = lerp(z, z_f, *(p_mask));
  }
}

void sediment_deposition_particle(Array &z,
                                  int    nparticles,
                                  int    ir,
                                  int    seed,
                                  Array *p_spawning_map,
                                  Array *p_deposition_map,
                                  float  particle_initial_sediment,
                                  float  deposition_velocity_limit,
                                  float  drag_rate)
{
  const int ni = z.shape.x;
  const int nj = z.shape.y;

  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0, 1);

  // keep a backup of the input if the erosion / deposition maps need
  // to be computed
  Array z_bckp = Array();
  if (p_deposition_map != nullptr) z_bckp = z;

  Array kernel = cone(Vec2<int>(2 * ir + 1, 2 * ir + 1));
  kernel.normalize();

  for (int ip = 0; ip < nparticles; ip++)
  {
    float x = 0.f;
    float y = 0.f;
    float vx = 0.f;
    float vy = 0.f;
    float s = particle_initial_sediment; // initial sediment amount

    if (p_spawning_map)
    {
      int count = 0;
      do
      {
        x = dis(gen) * (float)(ni - 3) + 1;
        y = dis(gen) * (float)(nj - 3) + 1;
        count++;
      } while (((*p_spawning_map)((int)x, (int)y) < SPAWN_LOW_LIMIT) &&
               (count < 20));
    }
    else
    {
      x = dis(gen) * (float)(ni - 3) + 1;
      y = dis(gen) * (float)(nj - 3) + 1;
    }

    int count = 0;

    while (s > SEDIMENT_MIN && count < 1000)
    {
      count++; // failsafe to avoid endless loop
      int   i = (int)x;
      int   j = (int)y;
      float u = x - (float)i;
      float v = y - (float)j;

      // surface normal \nabla z = (-dz/dx, -dz/dy, 1), not normalized
      // since its norm is already very close to one, assuming 'z'
      // scales with unity
      float nx = -z.get_gradient_x_bilinear_at(i, j, u, v);
      float ny = -z.get_gradient_y_bilinear_at(i, j, u, v);

      if (approx_hypot(nx, ny) < GRADIENT_MIN) break;

      // classical mechanics (with gravity = 1)
      vx += nx;
      vy += ny;

      // drag
      vx *= (1.f - drag_rate);
      vy *= (1.f - drag_rate);

      float vnorm = approx_hypot(vx, vy);

      if (vnorm < VELOCITY_MIN)
        break;
      else if (vnorm < deposition_velocity_limit)
      {
        if ((i > ir) and (i < ni - ir - 1) and (j > ir) and (j < nj - ir - 1))
          z.depose_amount_kernel_at(i, j, kernel, s);
        break;
      }
    }
  }

  extrapolate_borders(z);

  if (p_deposition_map)
  {
    *p_deposition_map = z - z_bckp;
    clamp_min(*p_deposition_map, 0.f);
  }
}

void sediment_deposition_particle(Array &z,
                                  Array *p_mask,
                                  int    nparticles,
                                  int    ir,
                                  int    seed,
                                  Array *p_spawning_map,
                                  Array *p_deposition_map,
                                  float  particle_initial_sediment,
                                  float  deposition_velocity_limit,
                                  float  drag_rate)
{
  if (!p_mask)
    sediment_deposition_particle(z,
                                 nparticles,
                                 ir,
                                 seed,
                                 p_spawning_map,
                                 p_deposition_map,
                                 particle_initial_sediment,
                                 deposition_velocity_limit,
                                 drag_rate);
  else
  {
    Array z_f = z;
    sediment_deposition_particle(z_f,
                                 nparticles,
                                 ir,
                                 seed,
                                 p_spawning_map,
                                 p_deposition_map,
                                 particle_initial_sediment,
                                 deposition_velocity_limit,
                                 drag_rate);
    z = lerp(z, z_f, *(p_mask));
  }
}

void sediment_layer(Array       &z,
                    const Array &talus_layer,
                    const Array &talus_upper_limit,
                    int          iterations,
                    bool         apply_post_filter,
                    Array       *p_deposition_map)
{
  // backup input
  Array z_bckp = z;

  // prepare talus
  Array g_talus = gradient_talus(z);
  Array talus_ref = talus_layer;
  Array fmask = Array(z.shape, 1.f);

  for (int j = 0; j < z.shape.y; j++)
    for (int i = 0; i < z.shape.x; i++)
      if (g_talus(i, j) > talus_upper_limit(i, j))
      {
        fmask(i, j) = 0.f;
        talus_ref(i, j) = 4.f * (g_talus(i, j) - talus_upper_limit(i, j)) +
                          talus_upper_limit(i, j);
      }

  // apply thermal erosion with prepared inputs
  Array sediment_layer_map(z.shape);

  thermal(z, talus_ref, iterations, nullptr, &sediment_layer_map);

  // smooth transitions
  if (apply_post_filter)
  {
    laplace(sediment_layer_map, 0.2f);
    z = z_bckp + sediment_layer_map;

    // filter also the deposition layer only (and also filter the mask
    // itself to avoid kinky spatial transitions)
    laplace(fmask, 0.2f);
    laplace(z, &fmask, 0.2f);
  }

  // output deposition map
  if (p_deposition_map) *p_deposition_map = sediment_layer_map;
}

} // namespace hmap
