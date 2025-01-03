/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>
#include <random>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/erosion.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/math.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

#include "highmap/internal/vector_utils.hpp"

namespace hmap
{

//----------------------------------------------------------------------
// Main operator
//----------------------------------------------------------------------

void thermal_scree(Array &z,
                   float  talus,
                   uint   seed,
                   float  zmax,
                   float  zmin,
                   float  noise_ratio,
                   Array *p_deposition_map,
                   float  landing_talus_ratio,
                   float  landing_width_ratio,
                   bool   talus_constraint)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(1.f - noise_ratio,
                                            1.f + noise_ratio);

  std::vector<int>   di = DI;
  std::vector<int>   dj = DJ;
  std::vector<float> c = CD;
  const uint         nb = di.size();

  // keep a backup of the input if the erosion / deposition maps need
  // to be computed
  Array z_bckp = Array();
  if (p_deposition_map != nullptr) z_bckp = z;

  // populate queue
  std::vector<int>   queue_i = {};
  std::vector<int>   queue_j = {};
  std::vector<float> queue_z = {};

  queue_i.reserve(z.shape.x * z.shape.y);
  queue_j.reserve(z.shape.x * z.shape.y);
  queue_z.reserve(z.shape.x * z.shape.y);

  // trick to exclude border cells, to avoid checking out of bounds
  // indices
  set_borders(z, 10.f * z.max(), 2);

  if (talus_constraint)
  {
    Array tz = gradient_talus(z);
    for (int j = 2; j < z.shape.y - 2; j++)
      for (int i = 2; i < z.shape.x - 2; i++)
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
    for (int j = 2; j < z.shape.y - 2; j++)
      for (int i = 2; i < z.shape.x - 2; i++)
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
  std::vector<size_t> idx = argsort(queue_z);
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

  if (p_deposition_map)
  {
    *p_deposition_map = z - z_bckp;
    clamp_min(*p_deposition_map, 0.f);
  }
}

void thermal_scree_fast(Array    &z,
                        Vec2<int> shape_coarse,
                        float     talus,
                        uint      seed,
                        float     zmax,
                        float     zmin,
                        float     noise_ratio,
                        float     landing_talus_ratio,
                        float     landing_width_ratio,
                        bool      talus_constraint)
{
  // apply the algorithm on the coarser mesh (and ajust the talus
  // value)
  int   step = std::max(z.shape.x / shape_coarse.x, z.shape.y / shape_coarse.y);
  float talus_coarse = talus * step;

  // add maximum filter to avoid loosing data (for instance those
  // defined at only one cell)
  Array z_coarse = Array(shape_coarse);
  {
    Array z_filtered = maximum_local(z, (int)std::ceil(0.5f * step));
    z_coarse = z_filtered.resample_to_shape(shape_coarse);
  }

  thermal_scree(z_coarse,
                talus_coarse,
                seed,
                zmax,
                zmin,
                noise_ratio,
                nullptr,
                landing_talus_ratio,
                landing_width_ratio,
                talus_constraint);

  // revert back to the original resolution but keep initial
  // smallscale details
  z_coarse = z_coarse.resample_to_shape(z.shape);

  clamp_min(z, z_coarse);
}

//----------------------------------------------------------------------
// Overloading
//----------------------------------------------------------------------

void thermal_scree(Array &z,
                   Array *p_mask,
                   float  talus,
                   uint   seed,
                   float  zmax,
                   float  zmin,
                   float  noise_ratio,
                   Array *p_deposition_map,
                   float  landing_talus_ratio,
                   float  landing_width_ratio,
                   bool   talus_constraint)
{
  if (!p_mask)
    thermal_scree(z,
                  talus,
                  seed,
                  zmax,
                  zmin,
                  noise_ratio,
                  p_deposition_map,
                  landing_talus_ratio,
                  landing_width_ratio,
                  talus_constraint);
  else
  {
    Array z_f = z;
    thermal_scree(z_f,
                  talus,
                  seed,
                  zmax,
                  zmin,
                  noise_ratio,
                  p_deposition_map,
                  landing_talus_ratio,
                  landing_width_ratio,
                  talus_constraint);
    z = lerp(z, z_f, *(p_mask));
  }
}

void thermal_scree(Array &z,
                   float  talus,
                   uint   seed,
                   float  zmax,
                   float  noise_ratio,
                   Array *p_deposition_map)
{
  thermal_scree(z,
                talus,
                seed,
                zmax,
                z.min(),
                noise_ratio,
                p_deposition_map,
                1.f, // landing talus = talus => deactivate smooth landing
                0.f,
                false);
}

void thermal_scree_fast(Array    &z,
                        Vec2<int> shape_coarse,
                        float     talus,
                        uint      seed,
                        float     zmax,
                        float     noise_ratio)
{
  thermal_scree_fast(z,
                     shape_coarse,
                     talus,
                     seed,
                     zmax,
                     z.min(),
                     noise_ratio,
                     1.f, // landing talus = talus => deactivate smooth landing
                     0.f,
                     false);
}

} // namespace hmap
