#include <cmath>
#include <random>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

#include "op/vector_utils.hpp"

namespace hmap
{

void thermal_scree(Array &z,
                   float  talus,
                   uint   seed,
                   float  noise_ratio,
                   float  zmin,
                   float  zmax,
                   float  landing_talus_ratio,
                   float  landing_width_ratio,
                   bool   talus_constraint)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(1.f - noise_ratio,
                                            1.f + noise_ratio);

  std::vector<int>   di = DI;
  std::vector<int>   dj = DJ;
  std::vector<float> c = C;
  const uint         nb = di.size();

  // populate queue
  std::vector<int>   queue_i = {};
  std::vector<int>   queue_j = {};
  std::vector<float> queue_z = {};

  // trick to exclude border cells, to avoid checking out of bounds
  // indices
  set_borders(z, 10.f * z.max(), 2);

  if (talus_constraint)
  {
    Array tz = gradient_talus(z);
    for (int i = 2; i < z.shape[0] - 2; i++)
      for (int j = 2; j < z.shape[1] - 2; j++)
      {
        float rd = dis(gen);
        if ((z(i, j) > zmin) and (z(i, j) < zmax * rd) and (tz(i, j) <= talus))
        {
          queue_i.push_back(i);
          queue_j.push_back(j);
          queue_z.push_back(z(i, j));
        }
      }
  }
  else
  {
    for (int i = 2; i < z.shape[0] - 2; i++)
      for (int j = 2; j < z.shape[1] - 2; j++)
      {
        float rd = dis(gen);
        if ((z(i, j) > zmin) and (z(i, j) < zmax * rd))
        {
          queue_i.push_back(i);
          queue_j.push_back(j);
          queue_z.push_back(z(i, j));
        }
      }
  }

  // sort queue by elevation
  std::vector<size_t> idx = sort_indexes(queue_z);
  reindex_vector(queue_i, idx);
  reindex_vector(queue_j, idx);
  reindex_vector(queue_z, idx);

  // fill
  if (landing_talus_ratio == 1.f) // --- filling / without soft-landing
  {
    while (queue_i.size() > 0)
    {
      int i = queue_i.back();
      int j = queue_j.back();

      queue_i.pop_back();
      queue_j.pop_back();
      queue_z.pop_back();

      for (uint k = 0; k < nb; k++) // loop over neighbors
      {
        int   p = i + di[k];
        int   q = j + dj[k];
        float rd = dis(gen);
        float h = z(i, j) - c[k] * talus * rd;

        if (h > z(p, q))
        {
          z(p, q) = h;

          // sorting should be performed to insert this new cells at the
          // right position but it is much faster to put it at the end
          // (and does not change much the result)
          queue_i.push_back(p);
          queue_j.push_back(q);
          queue_z.push_back(z(p, q));
        }
      }
    }
  }
  else // --- filling / with soft-landing
  {
    Array z0 = z;

    while (queue_i.size() > 0)
    {
      int i = queue_i.back();
      int j = queue_j.back();

      queue_i.pop_back();
      queue_j.pop_back();
      queue_z.pop_back();

      for (uint k = 0; k < nb; k++) // loop over neighbors
      {
        int   p = i + di[k];
        int   q = j + dj[k];
        float rd = dis(gen);

        float dz = c[k] * talus * rd;
        dz *= landing_talus_ratio +
              (1.f - landing_talus_ratio) *
                  std::min(1.f,
                           landing_width_ratio * std::abs(z(i, j) - z0(p, q)) /
                               talus);
        float h = z(i, j) - dz;

        if (h > z(p, q))
        {
          z(p, q) = h;

          // sorting should be performed to insert this new cells at the
          // right position but it is much faster to put it at the end
          // (and does not change much the result)
          queue_i.push_back(p);
          queue_j.push_back(q);
          queue_z.push_back(z(p, q));
        }
      }
    }
  }

  // clean-up boundaries
  extrapolate_borders(z, 2);
}

} // namespace hmap