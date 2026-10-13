/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cstddef>
#include <vector>

#include "highmap/terrain_tri_mesh.hpp"

namespace hmap
{

void TerrainTriMesh::flow_breach(float epsilon, float uphill_tolerance)
{
  if (this->points.empty()) return;

  auto paths = this->compute_shortest_paths_to_hull(true);

  // --- Carve depressions along paths to convex hull

  for (size_t start = 0; start < this->points.size(); ++start)
  {
    std::vector<size_t> path = this->path_to_hull(start, paths);
    if (path.size() < 2) continue;

    for (size_t i = 0; i < path.size() - 1; ++i)
    {
      size_t curr = path[i];
      size_t next = path[i + 1];

      // next is closer to the hull (downstream)
      // If next is higher than curr (a sill/barrier blocking outflow from
      // curr), carve next downwards so water can drain towards the hull
      float max_next_z = this->points[curr].z - epsilon + uphill_tolerance;

      if (this->points[next].z > max_next_z)
      {
        this->points[next].z = max_next_z;
      }
    }
  }
}

} // namespace hmap
