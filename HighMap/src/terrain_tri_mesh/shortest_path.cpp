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
    const std::vector<std::pair<size_t, float>> &sources,
    const MeshPathCostConfig                    &config) const
{
  constexpr size_t INVALID = std::numeric_limits<size_t>::max();

  ShortestPathResult result;

  const size_t n = points.size();

  float min_z = 0.f;
  if (config.metric == MeshPathCostConfig::Metric::SYMMETRIC_Z)
  {
    min_z = this->get_range_z().x;
  }

  result.distance.assign(n, std::numeric_limits<float>::infinity());
  result.parent.assign(n, INVALID);

  using QueueNode = std::pair<float, size_t>;

  std::
      priority_queue<QueueNode, std::vector<QueueNode>, std::greater<QueueNode>>
          queue;

  for (const auto &[src, init_dist] : sources)
  {
    if (src >= n) continue;

    result.distance[src] = init_dist;
    result.parent[src] = src;
    queue.emplace(init_dist, src);
  }

  while (!queue.empty())
  {
    auto [dist, v] = queue.top();
    queue.pop();

    if (dist > result.distance[v]) continue;

    for (const Neighbor &nb : neighbors.adjacency[v])
    {
      size_t nb_idx = nb.index;
      float  edge_cost = 0.f;

      switch (config.metric)
      {
      case MeshPathCostConfig::Metric::EUCLIDEAN_2D:
      {
        edge_cost = nb.distance2d;
        break;
      }

      case MeshPathCostConfig::Metric::SYMMETRIC_Z:
      {
        float dz = points[v].z - points[nb_idx].z;
        edge_cost = std::abs(dz) +
                    config.elevation_weight * (points[v].z - min_z);
        break;
      }

      case MeshPathCostConfig::Metric::HYDROLOGICAL:
      {
        float dz = points[nb_idx].z - points[v].z;
        edge_cost = (1.f - config.elevation_ratio) * nb.distance2d;

        if (dz > 0.f)
          edge_cost += config.upward_penalization *
                       std::pow(dz, config.distance_exponent);
        else
          edge_cost += std::abs(dz);

        edge_cost += config.elevation_ratio * std::max(0.f, dz);
        break;
      }
      }

      if (config.domain_center_weight > 0.f)
      {
        const auto &np = points[nb_idx];
        float dist_border = std::min({np.x, 1.f - np.x, np.y, 1.f - np.y});
        dist_border = std::clamp(dist_border, 0.f, 0.5f);

        // penalty is highest at boundary (1.0) and 0 at center
        float penalty = 1.f - 2.f * dist_border;
        edge_cost += config.domain_center_weight * penalty;
      }

      if (!config.vertex_cost_multipliers.empty() &&
          nb_idx < config.vertex_cost_multipliers.size())
      {
        edge_cost *= config.vertex_cost_multipliers[nb_idx];
      }

      float new_dist = dist + edge_cost;

      if (new_dist < result.distance[nb_idx])
      {
        result.distance[nb_idx] = new_dist;
        result.parent[nb_idx] = v;

        queue.emplace(new_dist, nb_idx);
      }
    }
  }

  return result;
}

TerrainTriMesh::ShortestPathResult TerrainTriMesh::compute_shortest_paths(
    size_t                    start,
    const MeshPathCostConfig &config) const
{
  return compute_shortest_paths({{start, 0.f}}, config);
}

TerrainTriMesh::ShortestPathResult TerrainTriMesh::compute_shortest_paths(
    size_t start,
    bool   use_delta_z,
    float  elevation_weight,
    float  domain_center_weight) const
{
  MeshPathCostConfig config;
  config.metric = use_delta_z ? MeshPathCostConfig::Metric::SYMMETRIC_Z
                              : MeshPathCostConfig::Metric::EUCLIDEAN_2D;
  config.elevation_weight = elevation_weight;
  config.domain_center_weight = domain_center_weight;

  return compute_shortest_paths(start, config);
}

TerrainTriMesh::ShortestPathResult TerrainTriMesh::
    compute_shortest_paths_to_hull(bool  use_delta_z,
                                   float elevation_weight) const
{
  MeshPathCostConfig config;
  config.metric = use_delta_z ? MeshPathCostConfig::Metric::SYMMETRIC_Z
                              : MeshPathCostConfig::Metric::EUCLIDEAN_2D;
  config.elevation_weight = elevation_weight;

  std::vector<std::pair<size_t, float>> sources;
  sources.reserve(convex_hull.size());

  for (size_t v : convex_hull)
  {
    sources.emplace_back(v, 0.f);
  }

  return compute_shortest_paths(sources, config);
}

std::vector<size_t> TerrainTriMesh::reconstruct_path(
    size_t                    start,
    size_t                    end,
    const ShortestPathResult &result) const
{
  constexpr size_t INVALID = std::numeric_limits<size_t>::max();

  std::vector<size_t> path;

  if (end >= result.parent.size() || result.parent[end] == INVALID) return path;

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

std::vector<size_t> TerrainTriMesh::shortest_path(
    size_t                    start,
    size_t                    end,
    const MeshPathCostConfig &config) const
{
  auto result = compute_shortest_paths(start, config);
  return reconstruct_path(start, end, result);
}

std::vector<size_t> TerrainTriMesh::shortest_path(
    size_t start,
    size_t end,
    bool   use_delta_z,
    float  elevation_weight,
    float  domain_center_weight) const
{
  MeshPathCostConfig config;
  config.metric = use_delta_z ? MeshPathCostConfig::Metric::SYMMETRIC_Z
                              : MeshPathCostConfig::Metric::EUCLIDEAN_2D;
  config.elevation_weight = elevation_weight;
  config.domain_center_weight = domain_center_weight;

  return shortest_path(start, end, config);
}

std::vector<size_t> TerrainTriMesh::path_to_hull(
    size_t                    start,
    const ShortestPathResult &result) const
{
  constexpr size_t INVALID = std::numeric_limits<size_t>::max();

  std::vector<size_t> path;

  if (start >= result.parent.size() || result.parent[start] == INVALID)
    return path;

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
