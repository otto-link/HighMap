/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/filters.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/shortest_path.hpp"

namespace hmap
{

void helper_find_up_downslope(const Array     &z,
                              const Vec2<int> &ij,
                              Vec2<int>       &ij_dw,
                              Vec2<int>       &ij_up)
{
  ij_dw = ij;
  ij_up = ij;

  float slope_max_dw = 0.f;
  float slope_max_up = 0.f;

  for (int r = -1; r <= 1; r++)
    for (int s = -1; s <= 1; s++)
    {
      if (r == 0 && s == 0) continue;

      float dz = z(ij.x, ij.y) - z(ij.x + r, ij.y + s);

      if (dz > slope_max_dw)
      {
        slope_max_dw = dz;
        ij_dw = Vec2<int>(ij.x + r, ij.y + s);
      }

      if (-dz > slope_max_up)
      {
        slope_max_up = -dz;
        ij_up = Vec2<int>(ij.x + r, ij.y + s);
      }
    }
}

Path flow_stream(const Array    &z,
                 const Vec2<int> ij_start,
                 const float     elevation_ratio = 0.5f,
                 const float     distance_exponent = 2.f,
                 const float     upward_penalization = 100.f)
{

  // --

  Vec2<int> shape = z.shape;

  // find exit points on the boundaries
  std::vector<Vec2<int>> ij_exits = {};

  for (int i = 1; i < shape.x - 1; ++i)
    for (int j : {0, shape.y - 1})
      if (z(i - 1, j) > z(i, j) && z(i + 1, j) > z(i, j))
        ij_exits.push_back(Vec2<int>(i, j));

  for (int j = 1; j < shape.y - 1; ++j)
    for (int i : {0, shape.x - 1})
      if (z(i, j - 1) > z(i, j) && z(i, j + 1) > z(i, j))
        ij_exits.push_back(Vec2<int>(i, j));

  std::vector<std::vector<int>> i_path_list;
  std::vector<std::vector<int>> j_path_list;

  find_path_dijkstra(z,
                     ij_start,
                     ij_exits,
                     i_path_list,
                     j_path_list,
                     elevation_ratio,
                     distance_exponent,
                     upward_penalization);

  // keep the path with the minimum upward cumulated elevation
  size_t kmin = 0;
  float  dz_cum_min = std::numeric_limits<float>::max();

  for (size_t k = 0; k < i_path_list.size(); k++)
  {
    float dz_cum = 0.f;
    for (size_t r = 0; r < i_path_list[k].size() - 1; r++)
    {
      float dz = z(i_path_list[k][r + 1], j_path_list[k][r + 1]) -
                 z(i_path_list[k][r], j_path_list[k][r]);
      if (dz > 0.f) dz_cum += dz;
    }

    if (dz_cum < dz_cum_min)
    {
      dz_cum_min = dz_cum;
      kmin = k;
    }
  }

  // output as a Path object (assuming a unit square bounding box)
  size_t             npts = i_path_list[kmin].size();
  std::vector<float> x(npts), y(npts), v(npts);

  for (size_t r = 0; r < i_path_list[kmin].size(); r++)
  {
    x[r] = (float)i_path_list[kmin][r] / (shape.x - 1.f);
    y[r] = (float)j_path_list[kmin][r] / (shape.y - 1.f);
    v[r] = z(i_path_list[kmin][r], j_path_list[kmin][r]);
  }

  Path path(x, y, v);

  return path;
}

} // namespace hmap
