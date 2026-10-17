/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "highmap/geometry/graph.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/geometry/point.hpp"
#include "highmap/hydrology/hydrology.hpp"

#include <unordered_map>
#include <unordered_set>

namespace hmap
{

Graph hydrology_paths_to_graph(const std::vector<Path> &paths,
                               float                    snap_tolerance)
{
  if (paths.empty()) return Graph();

  float tol = std::max(snap_tolerance, 1e-7f);
  float inv_tol = 1.f / tol;

  // spatial grid cell hash for vertex snapping
  struct CellCoord
  {
    int64_t ix;
    int64_t iy;

    bool operator==(const CellCoord &other) const
    {
      return ix == other.ix && iy == other.iy;
    }
  };

  struct CellCoordHash
  {
    std::size_t operator()(const CellCoord &c) const noexcept
    {
      std::size_t h1 = std::hash<int64_t>{}(c.ix);
      std::size_t h2 = std::hash<int64_t>{}(c.iy);
      return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
    }
  };

  std::unordered_map<CellCoord, std::vector<int>, CellCoordHash> grid;
  std::vector<Point>                                             unique_points;

  auto get_or_create_node = [&](const Point &pt) -> int
  {
    int64_t cx = static_cast<int64_t>(std::floor(pt.x * inv_tol));
    int64_t cy = static_cast<int64_t>(std::floor(pt.y * inv_tol));

    float tol2 = tol * tol;
    int   best_idx = -1;
    float best_dist2 = tol2;

    for (int64_t dx = -1; dx <= 1; ++dx)
    {
      for (int64_t dy = -1; dy <= 1; ++dy)
      {
        CellCoord cc{cx + dx, cy + dy};
        auto      it = grid.find(cc);

        if (it != grid.end())
        {
          for (int idx : it->second)
          {
            float ex = unique_points[idx].x - pt.x;
            float ey = unique_points[idx].y - pt.y;
            float d2 = ex * ex + ey * ey;

            if (d2 < best_dist2)
            {
              best_dist2 = d2;
              best_idx = idx;
            }
          }
        }
      }
    }

    if (best_idx != -1) return best_idx;

    int new_idx = static_cast<int>(unique_points.size());
    unique_points.push_back(pt);
    grid[{cx, cy}].push_back(new_idx);
    return new_idx;
  };

  // build unique vertices and sequential path segments
  std::vector<std::pair<int, int>> edges;

  for (const auto &path : paths)
  {
    if (path.size() < 2) continue;

    int prev_node = get_or_create_node(path[0]);

    for (size_t i = 1; i < path.size(); ++i)
    {
      int curr_node = get_or_create_node(path[i]);

      if (curr_node != prev_node)
      {
        edges.push_back({prev_node, curr_node});
        prev_node = curr_node;
      }
    }
  }

  Graph graph(std::move(unique_points));

  // deduplicate edges before adding
  struct EdgeHash
  {
    std::size_t operator()(const std::pair<int, int> &e) const noexcept
    {
      int min_v = std::min(e.first, e.second);
      int max_v = std::max(e.first, e.second);
      return (static_cast<size_t>(min_v) << 32) |
             (static_cast<size_t>(max_v) & 0xffffffffULL);
    }
  };

  struct EdgeEqual
  {
    bool operator()(const std::pair<int, int> &a,
                    const std::pair<int, int> &b) const noexcept
    {
      int min_a = std::min(a.first, a.second);
      int max_a = std::max(a.first, a.second);
      int min_b = std::min(b.first, b.second);
      int max_b = std::max(b.first, b.second);
      return min_a == min_b && max_a == max_b;
    }
  };

  std::unordered_set<std::pair<int, int>, EdgeHash, EdgeEqual> unique_edges;

  for (const auto &edge : edges)
  {
    if (unique_edges.insert(edge).second)
    {
      graph.add_edge(edge.first, edge.second);
    }
  }

  return graph;
}

} // namespace hmap
