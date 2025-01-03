/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <list>

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/hydrology.hpp"
#include "highmap/primitives.hpp"

// neighbor pattern search based on D8 flow direction neighborhood
// coding

// 5 6 7
// 4 . 0
// 3 2 1
// clang-format off
#define DI {1, 1, 0, -1, -1, -1, 0, 1}
#define DJ {0, -1, -1, -1, 0, 1, 1, 1}
#define C  {1.f, M_SQRT1_2, 1.f, M_SQRT1_2, 1.f, M_SQRT1_2, 1.f, M_SQRT1_2}
// clang-format on

namespace hmap
{

Array d8_compute_ndip(const Array &d8)
{
  Array nidp = Array(d8.shape); // output

  const std::vector<int> di = DI;
  const std::vector<int> dj = DJ;
  const uint             nb = di.size();

  // correspond between current neighbor index and neighbor flow
  // direction pointing to the current cell
  const std::vector<int> kp = {4, 5, 6, 7, 0, 1, 2, 3};

  for (int j = 1; j < d8.shape.y - 1; j++)
    for (int i = 1; i < d8.shape.x - 1; i++)
      // count the number of neighbors with flow directions pointing
      // to the current cell
      for (uint k = 0; k < nb; k++)
        if (d8(i + di[k], j + dj[k]) == kp[k]) nidp(i, j) += 1;

  return nidp;
}

Array flow_accumulation_d8(const Array &z)
{
  Array facc = constant(z.shape, 1.f);
  Array d8 = flow_direction_d8(z);
  Array nidp = d8_compute_ndip(d8);

  // --- flow accumulation
  const std::vector<int> di = DI;
  const std::vector<int> dj = DJ;

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

    // next cell (according to flow direction)
    int k = d8(i, j);
    int p = i + di[k];
    int q = j + dj[k];

    if ((i != p) or (j != q))
    {
      facc(p, q) += facc(i, j);
      nidp(p, q) -= 1.f;
      if (nidp(p, q) == 0)
      {
        i_queue.push_back(p);
        j_queue.push_back(q);
      }
    }
  }

  fill_borders(facc);
  return facc;
}

Array flow_direction_d8(const Array &z)
{
  Array d8 = Array(z.shape);

  const std::vector<int>   di = DI;
  const std::vector<int>   dj = DJ;
  const std::vector<float> c = C;
  const uint               nb = di.size();

  for (int j = 1; j < z.shape.y - 1; j++)
    for (int i = 1; i < z.shape.x - 1; i++)
    {
      float dmax = 0.f;
      int   kn = 0;

      for (uint k = 0; k < nb; k++)
      {
        // elevation difference between the two cells
        float delta = (z(i, j) - z(i + di[k], j + dj[k])) * c[k];

        if (delta > dmax)
        {
          dmax = delta;
          kn = k;
        }
      }
      d8(i, j) = (float)kn;
    }
  fill_borders(d8);
  return d8;
}

} // namespace hmap
