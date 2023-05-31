#include <cmath>

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

#include "highmap/dbg.hpp"
#include "macrologger.h"

#define EPS 1e-6f

namespace hmap
{

//----------------------------------------------------------------------
// Main operator(s)
//----------------------------------------------------------------------

void hydraulic_vpipes(Array &z)
{
  // mainly Isheden2022
  // Chiba1998 Isheden2022 Mei2007 Stava2008

  // https://www.diva-portal.org/smash/get/diva2:1646074/FULLTEXT01.pdf

  // parameters
  Array rain_map = 0.05f * constant(z.shape, 1.f);

  // Array rain_map = z;
  // remap(rain_map, 0.f, 0.01f);

  float dt = 0.5f;
  int   iterations = 40;

  float evap_rate = 1e-2f;
  float rain_rate = 1e-2f; // 1e-2f;

  // float evap_rate = 0.5f;
  // float rain_rate = 0.f; // 1e-2f;

  float c_capacity = 1.f;
  float c_erosion = 0.2f;
  float c_deposition = 0.2f;

  float g = 1.f;
  float pipe_length = 1.f; // / (float)std::min(z.shape[0], z.shape[1]);

  // local
  int ni = z.shape[0];
  int nj = z.shape[1];

  Array d = rain_map;       // water height
  Array s = Array(z.shape); // sediment height

  Array fL = Array(z.shape);
  Array fR = Array(z.shape);
  Array fT = Array(z.shape);
  Array fB = Array(z.shape);

  float talus_scaling = (float)(4 * std::min(z.shape[0], z.shape[1]));

  Array tmp = Array(z.shape);

  for (int it = 0; it < iterations; it++)
  {
    LOG_DEBUG("iteration: %d", it);

    // LOG_DEBUG("d sum: %g", d.sum());

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

      for (int i = 1; i < ni; i++)
        for (int j = 0; j < nj; j++)
        {
          float dh = z(i, j) + d1(i, j) - z(i - 1, j) - d1(i - 1, j);
          fL_next(i, j) = std::max(0.f, fL(i, j) + dt * g * dh / pipe_length);
        }

      for (int i = 0; i < ni - 1; i++)
        for (int j = 0; j < nj; j++)
        {
          float dh = z(i, j) + d1(i, j) - z(i + 1, j) - d1(i + 1, j);
          fR_next(i, j) = std::max(0.f, fR(i, j) + dt * g * dh / pipe_length);
        }

      for (int i = 0; i < ni; i++)
        for (int j = 0; j < nj - 1; j++)
        {
          float dh = z(i, j) + d1(i, j) - z(i, j + 1) - d1(i, j + 1);
          fT_next(i, j) = std::max(0.f, fT(i, j) + dt * g * dh / pipe_length);
        }

      for (int i = 0; i < ni; i++)
        for (int j = 1; j < nj; j++)
        {
          float dh = z(i, j) + d1(i, j) - z(i, j - 1) - d1(i, j - 1);
          fB_next(i, j) = std::max(0.f, fB(i, j) + dt * g * dh / pipe_length);
        }

      // normalize
      for (int i = 0; i < ni; i++)
        for (int j = 0; j < nj; j++)
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
      for (int i = 1; i < ni - 1; i++)
        for (int j = 1; j < nj - 1; j++)
        {
          float dv =
              dt * (fR(i - 1, j) + fT(i, j - 1) + fL(i + 1, j) + fB(i, j + 1) -
                    fL(i, j) - fR(i, j) - fT(i, j) - fB(i, j));
          d2(i, j) = d1(i, j) + dv / (pipe_length * pipe_length);
        }

      extrapolate_borders(d2);

      // flow velocities
      for (int i = 1; i < ni - 1; i++)
        for (int j = 1; j < nj - 1; j++)
        {
          u(i, j) = 0.5f * (fR(i - 1, j) - fL(i, j) + fR(i, j) - fL(i + 1, j));
          v(i, j) = 0.5f * (fT(i, j - 1) - fB(i, j) + fT(i, j) - fB(i, j + 1));
        }

      extrapolate_borders(u);
      extrapolate_borders(v);

      tmp = v;

    } // flow sim

    // --- erosion and deposition
    Array s1 = s;
    Array talus = talus_scaling * gradient_norm(z + d2);
    laplace(talus, 0.1f, 1);

    for (int i = 1; i < ni - 1; i++)
      for (int j = 1; j < nj - 1; j++)
      {
        // sin(alpha), sin of tilt angle
        float salpha = talus(i, j) / approx_hypot(1.f, talus(i, j));
        float sc = c_capacity * approx_hypot(u(i, j), v(i, j)) * salpha;
        float delta_sc = sc - s(i, j);
        float amount;

        if (delta_sc > 0.f)
          amount = c_erosion * delta_sc; // erosion
        else
          amount = c_deposition * delta_sc; // deposition

        s1(i, j) += amount;
        z(i, j) -= amount;
      }

    extrapolate_borders(s1);
    extrapolate_borders(z);

    // --- sediment transport
    for (int i = 1; i < ni - 1; i++)
      for (int j = 1; j < nj - 1; j++)
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

    extrapolate_borders(s);

    // --- flow evaporation
    d = d2 * (1.f - dt * evap_rate);

    clamp_min(d, 0.f);
    clamp_min(s, 0.f);

  } // it

  extrapolate_borders(z);
}

} // namespace hmap
