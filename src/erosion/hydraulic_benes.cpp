#include <algorithm>

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

#include "macrologger.h"

namespace hmap
{

void hydraulic_benes(Array &z, int iterations)
{
  std::vector<int> di = DI;
  std::vector<int> dj = DJ;
  const uint       nb = di.size();

  float c_erosion = 1.f;
  float c_capacity = 1.f;
  float evap_rate = 0.5f;
  float rain_rate = 0.05f;
  float water_level = 0.005f;

  Array rain_map = z; // constant(z.shape, 1.f);
  remap(rain_map);

  Array z_bedrock = constant(z.shape, -100.f);

  Array w = water_level * rain_map; // water map
  Array w_init = w;                 // backup water initial map
  Array s = constant(z.shape, 0.f); // sediment map (disolved in the water)

  float wmin = w.max() * 0.01f; // minimum water limit

  // main loop
  for (int it = 0; it < iterations; it++)
  {
    // modify neighbor search at each iterations to limit numerical
    // artifacts
    std::rotate(di.begin(), di.begin() + 1, di.end());
    std::rotate(dj.begin(), dj.begin() + 1, dj.end());

    w = (1.f - rain_rate) * w + rain_rate * w_init;

    // erosion
    z = z - c_erosion * w;
    s = s + c_erosion * w;

    for (int i = 1; i < z.shape[0] - 1; i++)
    {
      for (int j = 1; j < z.shape[1] - 1; j++)
      {
        if (z(i, j) > z_bedrock(i, j))
        {
          float              dsum = 0.f;
          float              zsavg = 0.f;
          int                navg = 0;
          std::vector<float> dz(nb);

          for (uint k = 0; k < nb; k++)
          {
            int ia = i + di[k];
            int ja = j + dj[k];
            dz[k] = (z(i, j) + w(i, j) - z(ia, ja) - w(ia, ja));

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

            for (uint k = 0; k < nb; k++)
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

    w = (1.f - evap_rate) * w;

    for (int i = 1; i < z.shape[0] - 1; i++)
    {
      for (int j = 1; j < z.shape[1] - 1; j++)
      {
        if (w(i, j) < wmin)
          w(i, j) = 0.f;
      }
    }

    // deposition
    for (int i = 1; i < z.shape[0] - 1; i++)
    {
      for (int j = 1; j < z.shape[1] - 1; j++)
      {
        float ds = s(i, j) - c_capacity * w(i, j);

        if (ds > 0.f)
        {
          s(i, j) -= ds;
          z(i, j) += ds;
        }
      }
    }

    extrapolate_borders(z);
    fill_borders(w);
    fill_borders(s);

    // laplace(z);
    // z = maximum(z, bedrock);

  } // it
}

} // namespace hmap
