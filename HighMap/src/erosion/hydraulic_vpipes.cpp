/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/math.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

#define EPS 1e-6f

namespace hmap
{

//----------------------------------------------------------------------
// Main operator(s)
//----------------------------------------------------------------------

void hydraulic_vpipes(Array &z,
                      int    iterations,
                      Array *p_bedrock,
                      Array *p_moisture_map,
                      Array *p_erosion_map,
                      Array *p_deposition_map,
                      float  water_height,
                      float  c_capacity,
                      float  c_erosion,
                      float  c_deposition,
                      float  rain_rate,
                      float  evap_rate)
{
  float dt = 0.5f;
  float g = 1.f;
  float pipe_length = 1.f;

  // local
  int ni = z.shape.x;
  int nj = z.shape.y;

  Array rain_map = Array(z.shape);
  if (p_moisture_map)
    rain_map = water_height * (*p_moisture_map);
  else
    rain_map = water_height;

  Array d = rain_map;       // water height
  Array s = Array(z.shape); // sediment height

  Array fL = Array(z.shape);
  Array fR = Array(z.shape);
  Array fT = Array(z.shape);
  Array fB = Array(z.shape);

  float talus_scaling = (float)std::min(z.shape.x, z.shape.y);

  // keep a backup of the input if the erosion / deposition maps need
  // to be computed
  Array z_bckp = Array();
  if ((p_erosion_map != nullptr) | (p_deposition_map != nullptr)) z_bckp = z;

  for (int it = 0; it < iterations; it++)
  {
    if (it % 10 == 0) LOG_DEBUG("iteration: %d", it);

    // --- water increase
    Array d1 = (1.f - dt * rain_rate) * d + dt * rain_rate * rain_map;

    // --- flow simulation
    Array d2 = d1;
    Array u = Array(z.shape); // flow velocities
    Array v = Array(z.shape);

    {
      Array fL_next = Array(z.shape);
      Array fR_next = Array(z.shape);
      Array fT_next = Array(z.shape);
      Array fB_next = Array(z.shape);

      for (int j = 0; j < nj; j++)
        for (int i = 1; i < ni; i++)
        {
          float dh = z(i, j) + d1(i, j) - z(i - 1, j) - d1(i - 1, j);
          fL_next(i, j) = std::max(0.f, fL(i, j) + dt * g * dh / pipe_length);
        }

      for (int j = 0; j < nj; j++)
        for (int i = 0; i < ni - 1; i++)
        {
          float dh = z(i, j) + d1(i, j) - z(i + 1, j) - d1(i + 1, j);
          fR_next(i, j) = std::max(0.f, fR(i, j) + dt * g * dh / pipe_length);
        }

      for (int j = 0; j < nj - 1; j++)
        for (int i = 0; i < ni; i++)
        {
          float dh = z(i, j) + d1(i, j) - z(i, j + 1) - d1(i, j + 1);
          fT_next(i, j) = std::max(0.f, fT(i, j) + dt * g * dh / pipe_length);
        }

      for (int j = 1; j < nj; j++)
        for (int i = 0; i < ni; i++)
        {
          float dh = z(i, j) + d1(i, j) - z(i, j - 1) - d1(i, j - 1);
          fB_next(i, j) = std::max(0.f, fB(i, j) + dt * g * dh / pipe_length);
        }

      fill_borders(fL_next);
      fill_borders(fR_next);
      fill_borders(fT_next);
      fill_borders(fB_next);

      // normalize
      for (int j = 0; j < nj; j++)
        for (int i = 0; i < ni; i++)
        {
          float k = d1(i, j) * pipe_length * pipe_length /
                    (fL_next(i, j) + fR_next(i, j) + fT_next(i, j) +
                     fB_next(i, j) + EPS) /
                    dt;
          k = std::min(1.f, k);

          fL_next(i, j) *= k;
          fR_next(i, j) *= k;
          fT_next(i, j) *= k;
          fB_next(i, j) *= k;
        }

      fL = fL_next;
      fR = fR_next;
      fT = fT_next;
      fB = fB_next;

      // water transport
      for (int j = 1; j < nj - 1; j++)
        for (int i = 1; i < ni - 1; i++)
        {
          float dv = dt *
                     (fR(i - 1, j) + fT(i, j - 1) + fL(i + 1, j) +
                      fB(i, j + 1) - fL(i, j) - fR(i, j) - fT(i, j) - fB(i, j));
          d2(i, j) = d1(i, j) + dv / (pipe_length * pipe_length);
        }

      {
        int i = 0;
        for (int j = 1; j < nj - 1; j++)
        {
          float dv = dt * (fT(i, j - 1) + fL(i + 1, j) + fB(i, j + 1) -
                           fL(i, j) - fR(i, j) - fT(i, j) - fB(i, j));
          d2(i, j) = d1(i, j) + dv / (pipe_length * pipe_length);
        }
      }

      {
        int i = ni - 1;
        for (int j = 1; j < nj - 1; j++)
        {
          float dv = dt * (fR(i - 1, j) + fT(i, j - 1) + fB(i, j + 1) -
                           fL(i, j) - fR(i, j) - fT(i, j) - fB(i, j));
          d2(i, j) = d1(i, j) + dv / (pipe_length * pipe_length);
        }
      }

      {
        int j = 0;
        for (int i = 1; i < ni - 1; i++)
        {
          float dv = dt * (fR(i - 1, j) + fL(i + 1, j) + fB(i, j + 1) -
                           fL(i, j) - fR(i, j) - fT(i, j) - fB(i, j));
          d2(i, j) = d1(i, j) + dv / (pipe_length * pipe_length);
        }
      }

      {
        int j = nj - 1;
        for (int i = 1; i < ni - 1; i++)
        {
          float dv = dt * (fR(i - 1, j) + fT(i, j - 1) + fL(i + 1, j) +
                           -fL(i, j) - fR(i, j) - fT(i, j) - fB(i, j));
          d2(i, j) = d1(i, j) + dv / (pipe_length * pipe_length);
        }
      }

      d2(0, 0) = 0.5f * (d2(1, 0) + d2(0, 1));
      d2(ni - 1, 0) = 0.5f * (d2(ni - 2, 0) + d2(ni - 1, 1));
      d2(ni - 1, nj - 1) = 0.5f * (d2(ni - 1, nj - 2) + d2(ni - 2, nj - 1));
      d2(0, nj - 1) = 0.5f * (d2(0, nj - 2) + d2(1, nj - 1));

      // flow velocities
      for (int j = 1; j < nj - 1; j++)
        for (int i = 1; i < ni - 1; i++)
        {
          u(i, j) = 0.5f * (fR(i - 1, j) - fL(i, j) + fR(i, j) - fL(i + 1, j));
          v(i, j) = 0.5f * (fT(i, j - 1) - fB(i, j) + fT(i, j) - fB(i, j + 1));

          float dmean = std::max(0.5f * water_height * dt,
                                 0.5f * (d1(i, j) + d2(i, j)));
          u(i, j) /= dmean;
          v(i, j) /= dmean;
        }

      fill_borders(u);
      fill_borders(v);

    } // flow sim

    // --- erosion and deposition
    Array s1 = s;
    Array talus = talus_scaling * gradient_norm(z + 0.5f * (d1 + d2));
    laplace(talus, 0.25f, 1);

    // smooth_cpulse(talus, 16);

    for (int j = 1; j < nj - 1; j++)
      for (int i = 1; i < ni - 1; i++)
      {
        // sin(alpha), sin of tilt angle
        float salpha = std::max(0.001f,
                                talus(i, j) / approx_hypot(1.f, talus(i, j)));
        float sc = c_capacity * approx_hypot(u(i, j), v(i, j)) * salpha;

        float delta_sc = dt * (sc - s(i, j));
        float amount;

        if (delta_sc > 0.f)
          amount = c_erosion * delta_sc; // erosion
        else
          amount = c_deposition * delta_sc; // deposition

        s1(i, j) += amount;
        z(i, j) -= amount;
      }

    fill_borders(s1);
    fill_borders(z);

    // bedrock pass
    if (p_bedrock)
      for (int j = 0; j < z.shape.y; j++)
        for (int i = 0; i < z.shape.x; i++)
          z(i, j) = std::max(z(i, j), (*p_bedrock)(i, j));

    // --- sediment transport
    for (int j = 1; j < nj - 1; j++)
      for (int i = 1; i < ni - 1; i++)
      {
        // sediment convection
        float x = (float)i - dt * u(i, j);
        float y = (float)j - dt * v(i, j);

        // bilinear interpolation parameters
        int   ip = (int)x;
        int   jp = (int)y;
        float u = x - (float)ip;
        float v = y - (float)jp;

        s(i, j) = s1.get_value_bilinear_at(ip, jp, u, v);
      }

    fill_borders(s);

    // --- flow evaporation
    d = d2 * (1.f - dt * evap_rate);

    clamp_min(d, 0.f);
    clamp_min(s, 0.f);

  } // it

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

void hydraulic_vpipes(Array &z,
                      Array *p_mask,
                      int    iterations,
                      Array *p_bedrock,
                      Array *p_moisture_map,
                      Array *p_erosion_map,
                      Array *p_deposition_map,
                      float  water_height,
                      float  c_capacity,
                      float  c_erosion,
                      float  c_deposition,
                      float  rain_rate,
                      float  evap_rate)
{
  if (!p_mask)
    hydraulic_vpipes(z,
                     iterations,
                     p_bedrock,
                     p_moisture_map,
                     p_erosion_map,
                     p_deposition_map,
                     water_height,
                     c_capacity,
                     c_erosion,
                     c_deposition,
                     rain_rate,
                     evap_rate);
  else
  {
    Array z_f = z;
    hydraulic_vpipes(z_f,
                     iterations,
                     p_bedrock,
                     p_moisture_map,
                     p_erosion_map,
                     p_deposition_map,
                     water_height,
                     c_capacity,
                     c_erosion,
                     c_deposition,
                     rain_rate,
                     evap_rate);
    z = lerp(z, z_f, *(p_mask));
  }
}

} // namespace hmap
