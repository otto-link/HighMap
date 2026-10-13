/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <cstdint>
#include <numeric>
#include <random>
#include <vector>

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/internal/validation.hpp"
#include "highmap/shortest_path.hpp"
#include "highmap/terrain_tri_mesh.hpp"
#include "highmap/virtual_array.hpp"

namespace hmap
{

Path find_cut_path_dijkstra(const Array   &z,
                            DomainBoundary start,
                            DomainBoundary end,
                            float          dijk_elevation_ratio,
                            float          dijk_distance_exponent,
                            float          dijk_upward_penalization,
                            std::uint32_t  seed,
                            bool           favor_boundary_center,
                            bool           favor_lower_elevation,
                            bool           favor_sinks)
{
  if (!validate_non_empty(z)) return Path();

  // --- pick start and end cells

  glm::ivec2 start_pt = pick_boundary_cell(z,
                                           start,
                                           seed,
                                           favor_boundary_center,
                                           favor_lower_elevation,
                                           favor_sinks);
  glm::ivec2 end_pt = pick_boundary_cell(z,
                                         end,
                                         seed,
                                         favor_boundary_center,
                                         favor_lower_elevation,
                                         favor_sinks);

  // --- find cut path

  return find_path_dijkstra(z,
                            start_pt,
                            end_pt,
                            glm::vec4(0.f, 1.f, 0.f, 1.f),
                            dijk_elevation_ratio,
                            dijk_distance_exponent,
                            dijk_upward_penalization);
}

Path find_cut_path_midpoint(const Array   &z,
                            DomainBoundary start,
                            DomainBoundary end,
                            std::uint32_t  seed,
                            float          offset_ratio,
                            int            steps,
                            bool           favor_boundary_center,
                            bool           favor_lower_elevation,
                            bool           favor_sinks)
{
  if (!validate_non_empty(z)) return Path();

  // --- pick start and end cells

  glm::ivec2 start_pt = pick_boundary_cell(z,
                                           start,
                                           seed,
                                           favor_boundary_center,
                                           favor_lower_elevation,
                                           favor_sinks);
  glm::ivec2 end_pt = pick_boundary_cell(z,
                                         end,
                                         seed,
                                         favor_boundary_center,
                                         favor_lower_elevation,
                                         favor_sinks);

  // --- find cut path

  const int max_it = 0; // => autoset by algo

  return find_path_midpoint(z,
                            start_pt,
                            end_pt,
                            glm::vec4(0.f, 1.f, 0.f, 1.f),
                            offset_ratio,
                            max_it,
                            steps);
}

Path find_cut_path_multiscale(const Array   &z,
                              DomainBoundary start,
                              DomainBoundary end,
                              int            n_levels,
                              int            corridor_radius,
                              float          corridor_decay,
                              float          dijk_elevation_ratio,
                              float          dijk_distance_exponent,
                              float          dijk_upward_penalization,
                              std::uint32_t  seed,
                              bool           favor_boundary_center,
                              bool           favor_lower_elevation,
                              bool           favor_sinks,
                              bool           use_astar,
                              bool           smooth_path)
{
  if (!validate_non_empty(z)) return Path();

  // --- pick start and end cells

  glm::ivec2 start_pt = pick_boundary_cell(z,
                                           start,
                                           seed,
                                           favor_boundary_center,
                                           favor_lower_elevation,
                                           favor_sinks);
  glm::ivec2 end_pt = pick_boundary_cell(z,
                                         end,
                                         seed,
                                         favor_boundary_center,
                                         favor_lower_elevation,
                                         favor_sinks);

  // --- find multiscale cut path

  return find_path_multiscale(z,
                              start_pt,
                              end_pt,
                              glm::vec4(0.f, 1.f, 0.f, 1.f),
                              n_levels,
                              corridor_radius,
                              corridor_decay,
                              dijk_elevation_ratio,
                              dijk_distance_exponent,
                              dijk_upward_penalization,
                              nullptr,
                              use_astar,
                              smooth_path);
}

} // namespace hmap

namespace hmap::va
{

namespace
{

size_t pick_mesh_boundary_vertex(const TerrainTriMesh &mesh,
                                 DomainBoundary        boundary,
                                 std::uint32_t         seed,
                                 bool                  favor_boundary_center,
                                 bool                  favor_lower_elevation,
                                 bool                  favor_sinks)
{
  const auto &points = mesh.get_points();
  if (points.empty()) return 0;

  const auto &nbrs = mesh.get_neighbors();

  constexpr float     eps = 1e-4f;
  std::vector<size_t> candidates;

  for (size_t i = 0; i < points.size(); ++i)
  {
    const auto &p = points[i];

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
    const auto &hull = mesh.get_convex_hull();
    if (!hull.empty())
    {
      candidates = hull;
    }
    else
    {
      candidates.resize(points.size());
      std::iota(candidates.begin(), candidates.end(), 0);
    }
  }

  // Sort candidates along the boundary coordinate for consistent ordering
  if (boundary == BOUNDARY_LEFT || boundary == BOUNDARY_RIGHT)
  {
    std::sort(candidates.begin(),
              candidates.end(),
              [&](size_t a, size_t b) { return points[a].y < points[b].y; });
  }
  else
  {
    std::sort(candidates.begin(),
              candidates.end(),
              [&](size_t a, size_t b) { return points[a].x < points[b].x; });
  }

  // Find min and max elevation among candidates
  float min_z = points[candidates.front()].z;
  float max_z = min_z;
  for (size_t idx : candidates)
  {
    min_z = std::min(min_z, points[idx].z);
    max_z = std::max(max_z, points[idx].z);
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
      float w = (delta > 1e-7f) ? (points[idx].z - min_z) / delta : 0.f;
      weights[k] *= (1.f - w) * (1.f - w);
    }

    if (favor_sinks)
    {
      float center_z = points[idx].z;
      float min_neigh = center_z;

      for (const auto &nb : nbrs.adjacency[idx])
        min_neigh = std::min(min_neigh, points[nb.index].z);

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

} // namespace

Path find_cut_path_midpoint(const ComputeMode  &cm,
                            const VirtualArray &z,
                            DomainBoundary      start,
                            DomainBoundary      end,
                            std::uint32_t       seed,
                            size_t              control_points_count,
                            float /* offset_ratio */,
                            int /* steps */,
                            bool favor_boundary_center,
                            bool favor_lower_elevation,
                            bool favor_sinks)
{
  TerrainTriMesh mesh = va::generate_terrain_tri_mesh_from_heightmap_random(
      cm,
      z,
      control_points_count,
      seed);

  if (mesh.size() == 0) return Path();

  size_t start_idx = pick_mesh_boundary_vertex(mesh,
                                               start,
                                               seed,
                                               favor_boundary_center,
                                               favor_lower_elevation,
                                               favor_sinks);

  // use seed + 1 for selecting end vertex to avoid identical index selection on
  // same distributions
  size_t end_idx = pick_mesh_boundary_vertex(mesh,
                                             end,
                                             seed + 1,
                                             favor_boundary_center,
                                             favor_lower_elevation,
                                             favor_sinks);

  std::vector<size_t> vertex_indices = mesh.shortest_path(start_idx,
                                                          end_idx,
                                                          true,
                                                          1.f);

  if (vertex_indices.empty())
  {
    vertex_indices = {start_idx, end_idx};
  }

  const auto        &pts = mesh.get_points();
  std::vector<Point> path_points;
  path_points.reserve(vertex_indices.size());

  for (size_t v : vertex_indices)
  {
    const auto &p = pts[v];
    path_points.emplace_back(p.x, p.y, p.z);
  }

  return Path(path_points);
}

} // namespace hmap::va
