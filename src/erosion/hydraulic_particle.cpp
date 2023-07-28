/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <random>

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

#include "highmap/dbg.hpp"
#include "macrologger.h"

#define DT 1.f
#define VOLUME_INIT 1.f
#define VOLUME_MIN 0.01f
#define SPAWN_MOISTURE_LOW_LIMIT 0.1f
#define VELOCITY_INIT 0.f
#define VELOCITY_MIN 0.001f
#define GRADIENT_MIN 0.0001f

namespace hmap
{

//----------------------------------------------------------------------
// Main operator(s)
//----------------------------------------------------------------------

void hydraulic_particle(Array &z,
                        Array &moisture_map,
                        int    nparticles,
                        int    seed,
                        int    c_radius,
                        float  c_capacity,
                        float  c_erosion,
                        float  c_deposition,
                        float  drag_rate,
                        float  evap_rate)
{
  const int ni = z.shape[0];
  const int nj = z.shape[1];
  float     dt = DT;

  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0, 1);

  // --- initialization

  // define erosion kernel
  const int ir = c_radius;
  const int nk = 2 * ir + 1;
  Array     kernel = tricube({nk, nk});

  kernel.normalize();

  // --- main loop

#pragma omp parallel for
  for (int ip = 0; ip < nparticles; ip++)
  {
    float x = 0.f;
    float y = 0.f;
    float vx = VELOCITY_INIT * (2.f * dis(gen) - 1.f);
    float vy = VELOCITY_INIT * (2.f * dis(gen) - 1.f);
    float s = 0.f; // sediment

    // keep spawning new particle in [1, shape[...] - 2] until the
    // initial volume is large enough (to avoid using particle from
    // dry regions)
    do
    {
      x = dis(gen) * (float)(ni - 3) + 1;
      y = dis(gen) * (float)(nj - 3) + 1;
    } while (moisture_map((int)x, (int)y) < SPAWN_MOISTURE_LOW_LIMIT);

    float volume = VOLUME_INIT * moisture_map((int)x, (int)y);

    int i_next = (int)x;
    int j_next = (int)y;

    while (volume > VOLUME_MIN)
    {
      int   i = i_next;
      int   j = j_next;
      float u = x - (float)i_next;
      float v = y - (float)j_next;
      float z_old = z.get_value_bilinear_at(i, j, u, v);

      // surface normal \nabla z = (-dz/dx, -dz/dy, 1), not normalized
      // since its norm is already very close to one, assuming 'z'
      // scales with unity
      float nx = -z.get_gradient_x_bilinear_at(i, j, u, v);
      float ny = -z.get_gradient_y_bilinear_at(i, j, u, v);

      if (approx_hypot(nx, ny) < GRADIENT_MIN)
        break;

      // classical mechanics (with gravity = 1)
      vx += dt * nx;
      vy += dt * ny;

      // drag
      vx *= (1.f - dt * drag_rate);
      vy *= (1.f - dt * drag_rate);

      float vnorm = approx_hypot(vx, vy);

      if (vnorm < VELOCITY_MIN)
        break;

      // move particle
      x += dt * vx;
      y += dt * vy;

      // perform erosion / deposition
      i_next = (int)x;
      j_next = (int)y;

      float u_next = x - (float)i_next;
      float v_next = y - (float)j_next;

      if ((i_next < 1) or (i_next > ni - 2) or (j_next < 1) or
          (j_next > nj - 2))
        break;

      float z_next = z.get_value_bilinear_at(i_next, j_next, u_next, v_next);

      // particle sediment capacity
      float dz = z_old - z_next;
      float sc = c_capacity * volume * vnorm * dz;
      float delta_sc = dt * (sc - s);
      float amount;

      if (delta_sc > 0.f)
        amount = c_erosion * delta_sc; // erosion
      else
        amount = c_deposition * delta_sc; // deposition

      s += amount;

      if (ir == 0)
        z.depose_amount_bilinear_at(i, j, u, v, -amount);
      else if ((i > ir) and (i < ni - ir - 1) and (j > ir) and
               (j < nj - ir - 1))
        // kernel-based - VERY SLOW
        z.depose_amount_kernel_bilinear_at(i, j, u, v, ir, -amount);

      volume *= (1 - dt * evap_rate);
    }
  }

  extrapolate_borders(z);
}

//----------------------------------------------------------------------
// Overloading
//----------------------------------------------------------------------

// uniform moisture map
void hydraulic_particle(Array &z,
                        int    nparticles,
                        int    seed,
                        int    c_radius,
                        float  c_capacity,
                        float  c_erosion,
                        float  c_deposition,
                        float  drag_rate,
                        float  evap_rate)
{
  Array mmap = constant(z.shape, 1.f);
  hydraulic_particle(z,
                     mmap,
                     nparticles,
                     seed,
                     c_radius,
                     c_capacity,
                     c_erosion,
                     c_deposition,
                     drag_rate,
                     evap_rate);
}

//----------------------------------------------------------------------
// Macros
//----------------------------------------------------------------------

void hydraulic_particle_multiscale(Array             &z,
                                   Array             &moisture_map,
                                   float              particle_density,
                                   std::vector<float> scales,
                                   int                seed,
                                   int                c_radius,
                                   float              c_capacity,
                                   float              c_erosion,
                                   float              c_deposition,
                                   float              drag_rate,
                                   float              evap_rate)
{
  const uint ns = scales.size();

  for (uint k = 0; k < ns; k++)
  {
    // "large scales" are isolated using resampling
    int              ni = (int)std::ceil((float)z.shape[0] / scales[k]);
    int              nj = (int)std::ceil((float)z.shape[1] / scales[k]);
    std::vector<int> shape_c = {ni, nj};
    int              nparticles = (int)(particle_density * ni * nj);

    LOG_DEBUG("resampling: {%d, %d}", ni, nj);

    Array z_c = z.resample_to_shape(shape_c);
    Array mmap_c = moisture_map.resample_to_shape(shape_c);

    // backup "small scales"
    Array z_c0 = z - z_c.resample_to_shape(z.shape);

    // perform erosion and resample back to initial shape
    hydraulic_particle(z_c,
                       mmap_c,
                       nparticles,
                       seed,
                       c_radius,
                       c_capacity,
                       c_erosion,
                       c_deposition,
                       drag_rate,
                       evap_rate);

    z_c = z_c.resample_to_shape(z.shape);
    z = z_c + z_c0;
  }
}

// void hydraulic_thermal(Array &z)
// {
// }

} // namespace hmap
