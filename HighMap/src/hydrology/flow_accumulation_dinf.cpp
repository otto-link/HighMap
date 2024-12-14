/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <list>

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/hydrology.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

// neighbor pattern search based on D8 flow direction neighborhood
// coding

// 5 1 7
// 0 . 3
// 4 2 6
// clang-format off
#define DI {-1, 0, 0, 1, -1, -1, 1, 1}
#define DJ {0, 1, -1, 0, -1, 1, -1, 1}
#define KP {3, 2, 1, 0, 7, 6, 5, 4}  // reverse neighbor index
#define C  {1.f, 1.f, 1.f, 1.f, M_SQRT1_2, M_SQRT1_2, M_SQRT1_2, M_SQRT1_2}
  
// the "effective contour length" of pixel i. The value of L i is 0.5
// for pixels in cardinal directions and 0.354 for pixels in diagonal
// directions (Quinn et al., 1991)
#define ECL {0.5, 0.5, 0.5, 0.5 , 0.354, 0.354, 0.354, 0.354}
// clang-format on

namespace hmap
{

Array flow_accumulation_dinf(const Array &z, float talus_ref)
{
  const std::vector<int> di = DI;
  const std::vector<int> dj = DJ;
  const std::vector<int> kp = KP;
  const uint             nb = di.size();

  Array facc = constant(z.shape, 1.f);
  Array nidp = Array(z.shape);

  // smooth small wavelenghts before computing flow directions to
  // avoid artifacts
  std::vector<Array> dinf(nb, {z.shape});
  {
    Array zf = z;
    laplace(zf);
    dinf = flow_direction_dinf(zf, talus_ref);
  }

  // --- compute number of input drainage paths for each cell
  for (int j = 1; j < z.shape.y - 1; j++)
    for (int i = 1; i < z.shape.x - 1; i++)
      for (uint k = 0; k < nb; k++)
        // count the number of neighbors with flow directions pointing
        // to the current cell
        if (dinf[kp[k]](i + di[k], j + dj[k]) > 0.f) nidp(i, j) += 1.f;

  // --- flow accumulation (same as D8 with minor adjustements)

  // populate queue
  std::list<int> i_queue = {};
  std::list<int> j_queue = {};

  for (int j = 1; j < z.shape.y - 1; j++)
    for (int i = 1; i < z.shape.x - 1; i++)
      if (nidp(i, j) == 0)
      {
        i_queue.push_back(i);
        j_queue.push_back(j);
      }

  // main loop
  while (i_queue.size() > 0)
  {
    int i = i_queue.back();
    int j = j_queue.back();
    i_queue.pop_back();
    j_queue.pop_back();

    for (uint k = 0; k < nb; k++)
    {
      int p = i + di[k];
      int q = j + dj[k];

      if (dinf[k](i, j) > 0.f)
      {
        facc(p, q) += facc(i, j) * dinf[k](i, j);
        nidp(p, q) -= 1.f;
        if (nidp(p, q) == 0.f)
        {
          i_queue.push_back(p);
          j_queue.push_back(q);
        }
      }
    }
  }

  fill_borders(facc);
  return facc;
}

std::vector<Array> flow_direction_dinf(const Array &z, float talus_ref)
{
  const std::vector<int>   di = DI;
  const std::vector<int>   dj = DJ;
  const std::vector<float> c = C;
  const std::vector<float> ecl = ECL;
  const uint               nb = di.size();

  // the flow-partition exponent is defined locally based on the local
  // talus in [1, 10] (Qin et al 2007)
  Array p = Array(z.shape);
  {
    Array talus = gradient_talus(z) / talus_ref;
    clamp_max(talus, 1.f);
    p = 10.f * talus + 1.f;
  }

  // memory consuming... every 8 direction needs a full array
  std::vector<Array> dinf(nb, {z.shape});

  for (int j = 1; j < z.shape.y - 1; j++)
    for (int i = 1; i < z.shape.x - 1; i++)
    {
      for (uint k = 0; k < nb; k++)
      {
        float dz = z(i, j) - z(i + di[k], j + dj[k]);
        if (dz > 0) dinf[k](i, j) = std::pow(dz * c[k], p(i, j)) * ecl[k];
      }

      // normalize
      float sum = 0.f;
      for (uint k = 0; k < nb; k++)
        sum += dinf[k](i, j);

      if (sum > 0.f)
        for (uint k = 0; k < nb; k++)
          dinf[k](i, j) /= sum;
    }

  return dinf;
}

} // namespace hmap
