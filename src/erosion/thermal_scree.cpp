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

  // filling
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

        size_t r = 0;
        if (queue_z.size() > 0)
          r = upperbound_right(queue_z, z(p, q));

        queue_i.insert(queue_i.begin() + r, p);
        queue_j.insert(queue_j.begin() + r, q);
        queue_z.insert(queue_z.begin() + r, z(p, q));
      }
    }
  }
  extrapolate_borders(z, 2);
}

} // namespace hmap
