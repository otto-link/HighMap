/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
#include <queue>
#include <random>
#include <utility>
#include <vector>

#include "highmap/terrain_tri_mesh.hpp"

namespace hmap
{

TerrainTriMesh::ShortestPathResult TerrainTriMesh::compute_shortest_paths(
    size_t start,
    bool   use_delta_z,
    float  elevation_weight,
    float  domain_center_weight) const
{
  constexpr size_t INVALID = std::numeric_limits<size_t>::max();

  ShortestPathResult result;

  const size_t n = points.size();

  result.distance.assign(n, std::numeric_limits<float>::infinity());
  result.parent.assign(n, INVALID);

  using QueueNode = std::pair<float, size_t>;

  std::
      priority_queue<QueueNode, std::vector<QueueNode>, std::greater<QueueNode>>
          queue;

  result.distance[start] = 0.f;
  result.parent[start] = start;

  queue.emplace(0.f, start);

  while (!queue.empty())
  {
    auto [dist, v] = queue.top();
    queue.pop();

    if (dist > result.distance[v]) continue;

    for (const Neighbor &n : neighbors.adjacency[v])
    {
      float edge_cost;

      if (use_delta_z)
      {
        float dz = points[v].z - points[n.index].z;
        edge_cost = std::abs(dz) + elevation_weight * points[v].z;
      }
      else
      {
        edge_cost = n.distance2d;
      }

      if (domain_center_weight > 0.f)
      {
        const auto &np = points[n.index];
        float dist_border = std::min({np.x, 1.f - np.x, np.y, 1.f - np.y});
        dist_border = std::clamp(dist_border, 0.f, 0.5f);

        // penalty is highest at boundary (1.0) and 0 at center
        float penalty = 1.f - 2.f * dist_border;
        edge_cost += domain_center_weight * penalty;
      }

      float new_dist = dist + edge_cost;

      if (new_dist < result.distance[n.index])
      {
        result.distance[n.index] = new_dist;
        result.parent[n.index] = v;

        queue.emplace(new_dist, n.index);
      }
    }
  }

  return result;
}

TerrainTriMesh::ShortestPathResult TerrainTriMesh::
    compute_shortest_paths_to_hull(bool  use_delta_z,
                                   float elevation_weight) const
{
  constexpr size_t INVALID = std::numeric_limits<size_t>::max();

  ShortestPathResult result;

  const size_t n = points.size();

  result.distance.assign(n, std::numeric_limits<float>::infinity());
  result.parent.assign(n, INVALID);

  using QueueNode = std::pair<float, size_t>;

  std::
      priority_queue<QueueNode, std::vector<QueueNode>, std::greater<QueueNode>>
          queue;

  for (size_t v : convex_hull)
  {
    result.distance[v] = 0.f;
    result.parent[v] = v;
    queue.emplace(0.f, v);
  }

  while (!queue.empty())
  {
    auto [dist, v] = queue.top();
    queue.pop();

    if (dist > result.distance[v]) continue;

    for (const Neighbor &n : neighbors.adjacency[v])
    {
      float edge_cost;

      if (use_delta_z)
      {
        float dz = points[v].z - points[n.index].z;
        edge_cost = std::abs(dz) + elevation_weight * points[v].z;
      }
      else
      {
        edge_cost = n.distance2d;
      }

      float new_dist = dist + edge_cost;

      if (new_dist < result.distance[n.index])
      {
        result.distance[n.index] = new_dist;
        result.parent[n.index] = v;

        queue.emplace(new_dist, n.index);
      }
    }
  }

  return result;
}

std::vector<size_t> TerrainTriMesh::shortest_path(
    size_t start,
    size_t end,
    bool   use_delta_z,
    float  elevation_weight,
    float  domain_center_weight) const
{
  constexpr size_t INVALID = std::numeric_limits<size_t>::max();

  auto result = compute_shortest_paths(start,
                                       use_delta_z,
                                       elevation_weight,
                                       domain_center_weight);

  std::vector<size_t> path;

  if (result.parent[end] == INVALID) return path;

  const size_t n = points.size();

  for (size_t v = end;; v = result.parent[v])
  {
    path.push_back(v);

    if (v == start) break;

    if (result.parent[v] == INVALID || result.parent[v] == v || path.size() > n)
    {
      return {};
    }
  }

  std::reverse(path.begin(), path.end());

  return path;
}

std::vector<size_t> TerrainTriMesh::path_to_hull(
    size_t                    start,
    const ShortestPathResult &result) const
{
  constexpr size_t INVALID = std::numeric_limits<size_t>::max();

  std::vector<size_t> path;

  if (result.parent[start] == INVALID) return path;

  for (size_t v = start;; v = result.parent[v])
  {
    path.push_back(v);

    if (result.parent[v] == v) break;
  }

  return path;
}

size_t TerrainTriMesh::pick_mesh_boundary_vertex(DomainBoundary boundary,
                                                 std::uint32_t  seed,
                                                 bool favor_boundary_center,
                                                 bool favor_lower_elevation,
                                                 bool favor_sinks) const
{
  if (this->points.empty()) return 0;

  constexpr float     eps = 1e-4f;
  std::vector<size_t> candidates;

  for (size_t i = 0; i < this->points.size(); ++i)
  {
    const auto &p = this->points[i];

    bool match = false;
    switch (boundary)
    {
    case BOUNDARY_LEFT: match = (p.x <= eps); break;

    case BOUNDARY_RIGHT: match = (p.x >= 1.f - eps); break;

    case BOUNDARY_BOTTOM: match = (p.y <= eps); break;

    case BOUNDARY_TOP: match = (p.y >= 1.f - eps); break;
    }

    if (match) candidates.push_back(i);
  }

  // Fallback to convex hull vertices if none strictly matched
  if (candidates.empty())
  {
    if (!this->convex_hull.empty())
    {
      candidates = this->convex_hull;
    }
    else
    {
      candidates.resize(this->points.size());
      std::iota(candidates.begin(), candidates.end(), 0);
    }
  }

  // Sort candidates along the boundary coordinate for consistent ordering
  if (boundary == BOUNDARY_LEFT || boundary == BOUNDARY_RIGHT)
  {
    std::sort(candidates.begin(),
              candidates.end(),
              [&](size_t a, size_t b)
              { return this->points[a].y < this->points[b].y; });
  }
  else
  {
    std::sort(candidates.begin(),
              candidates.end(),
              [&](size_t a, size_t b)
              { return this->points[a].x < this->points[b].x; });
  }

  // Find min and max elevation among candidates
  float min_z = this->points[candidates.front()].z;
  float max_z = min_z;
  for (size_t idx : candidates)
  {
    min_z = std::min(min_z, this->points[idx].z);
    max_z = std::max(max_z, this->points[idx].z);
  }

  const size_t       nc = candidates.size();
  std::vector<float> weights(nc, 1.f);

  for (size_t k = 0; k < nc; ++k)
  {
    size_t idx = candidates[k];

    if (favor_boundary_center && nc > 1)
    {
      float w = float(k) / float(nc - 1);
      weights[k] *= 4.f * w * (1.f - w);
    }

    if (favor_lower_elevation)
    {
      float delta = max_z - min_z;
      float w = (delta > 1e-7f) ? (this->points[idx].z - min_z) / delta : 0.f;
      weights[k] *= (1.f - w) * (1.f - w);
    }

    if (favor_sinks)
    {
      float center_z = this->points[idx].z;
      float min_neigh = center_z;

      for (const auto &nb : this->neighbors.adjacency[idx])
        min_neigh = std::min(min_neigh, this->points[nb.index].z);

      if (center_z <= min_neigh) weights[k] = std::min(1.f, 4.f * weights[k]);
    }
  }

  // Handle case where all weights sum to zero
  float sum_w = 0.f;
  for (float w : weights)
    sum_w += w;

  if (sum_w <= 1e-7f) return candidates[candidates.size() / 2];

  std::mt19937                    gen(seed);
  std::discrete_distribution<int> dist(weights.begin(), weights.end());

  return candidates[dist(gen)];
}

} // namespace hmap
