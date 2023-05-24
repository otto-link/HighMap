#include <algorithm>

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

#include "macrologger.h"

namespace hmap
{

//----------------------------------------------------------------------
// Main operator(s)
//----------------------------------------------------------------------

void hydraulic_benes(Array &z,
                     Array &rain_map,
                     int    iterations,
                     float  c_capacity,
                     float  c_erosion,
                     float  c_deposition,
                     float  water_level,
                     float  evap_rate,
                     float  rain_rate)
{
  std::vector<int> di = DI;
  std::vector<int> dj = DJ;
  const uint       nb = di.size();

  Array w = water_level * rain_map; // water map
  Array w_init = w;                 // backup water initial map
  Array s = constant(z.shape, 0.f); // sediment map (disolved in the water)

  float wmin = w.max() * 0.01f; // minimum water limit

  Array vel = Array(z.shape);

  // main loop
  for (int it = 0; it < iterations; it++)
  {
    // modify neighbor search at each iterations to limit numerical
    // artifacts
    std::rotate(di.begin(), di.begin() + 1, di.end());
    std::rotate(dj.begin(), dj.begin() + 1, dj.end());

    w = (1.f - rain_rate) * w + rain_rate * w_init;

    // --- water flow dynamic and sediment transport

    for (int i = 1; i < z.shape[0] - 1; i++)
    {
      for (int j = 1; j < z.shape[1] - 1; j++)
      {
        float              dsum = 0.f;
        float              zsavg = 0.f;
        int                navg = 0;
        std::vector<float> dz(nb);

        for (uint k = 0; k < nb; k++)
        {
          int ia = i + di[k];
          int ja = j + dj[k];
          dz[k] = z(i, j) + w(i, j) - z(ia, ja) - w(ia, ja);

          if (dz[k] > 0.f)
          {
            dsum += dz[k];
            zsavg += z(ia, ja) + w(ia, ja);
            navg++;
          }
          else
            dz[k] = 0.f;
        }

        if (dsum > 0.f and w(i, j) > wmin)
        {
          zsavg /= (float)navg;

          float dw_tot = std::min(w(i, j), z(i, j) + w(i, j) - zsavg);
          float ds_tot;

          if (w(i, j) > 0.f)
            ds_tot = s(i, j) * dw_tot / w(i, j);
          else
            ds_tot = 0.f;

          w(i, j) -= dw_tot;
          s(i, j) -= ds_tot;

          vel(i, j) = dw_tot;

          for (uint k = 0; k < nb; k++)
          {
            if (dz[k] > 0.f)
            {
              int   ia = i + di[k];
              int   ja = j + dj[k];
              float r = dz[k] / dsum;

              w(ia, ja) += dw_tot * r;
              s(ia, ja) += ds_tot * r;
            }
          }
        }
      } // j
    }   // i

    // --- erosion and deposition
    for (int i = 1; i < z.shape[0] - 1; i++)
    {
      for (int j = 1; j < z.shape[1] - 1; j++)
      {
        float              dsum = 0.f;
        std::vector<float> dz(nb);

        for (uint k = 0; k < nb; k++)
        {
          int ia = i + di[k];
          int ja = j + dj[k];
          dz[k] = z(i, j) + w(i, j) - z(ia, ja) - w(ia, ja); // no water

          if (dz[k] > 0.f)
            dsum += dz[k];
          else
            dz[k] = 0.f;
        }

        if (dsum > 0.f)
        {
          float ds_tot = c_capacity * w(i, j) * vel(i, j) - s(i, j);
          float amount;

          if (ds_tot > 0.f)
            amount = c_erosion * ds_tot;
          else
            amount = c_deposition * ds_tot;

          z(i, j) -= amount;
          s(i, j) += amount;

          for (uint k = 0; k < nb; k++)
          {
            if (dz[k] > 0.f)
            {
              int   ia = i + di[k];
              int   ja = j + dj[k];
              float r = dz[k] / dsum;

              z(ia, ja) -= amount * r;
              s(ia, ja) += amount * r;
            }
          }
        }

      } // j
    }   // i

    w = (1.f - evap_rate) * w;
    chop(w, wmin);

    extrapolate_borders(z);
    fill_borders(w);
    fill_borders(s);

    laplace(w);
    laplace(s);
  } // it
}

//----------------------------------------------------------------------
// Overloading
//----------------------------------------------------------------------

// uniform moisture map
void hydraulic_benes(Array &z,
                     int    iterations,
                     float  c_capacity,
                     float  c_erosion,
                     float  c_deposition,
                     float  water_level,
                     float  evap_rate,
                     float  rain_rate)
{
  Array rain_map = constant(z.shape, 1.f);

  hydraulic_benes(z,
                  rain_map,
                  iterations,
                  c_capacity,
                  c_erosion,
                  c_deposition,
                  water_level,
                  evap_rate,
                  rain_rate);
}

} // namespace hmap
