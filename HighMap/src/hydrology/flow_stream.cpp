/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

#include "highmap/array.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/geometry/point.hpp"
#include "highmap/hydrology/hydrology.hpp"
#include "highmap/internal/validation.hpp"
#include "highmap/shortest_path.hpp"
#include "highmap/terrain_tri_mesh.hpp"
#include "highmap/virtual_array/virtual_array.hpp"

namespace hmap
{

Path flow_stream(const Array             &z,
                 const glm::ivec2         ij_start,
                 const float              elevation_ratio,
                 const float              distance_exponent,
                 const float              upward_penalization,
                 const std::vector<Path> &existing_streams)
{
  if (!validate_non_empty(z)) return Path();

  glm::ivec2 shape = z.shape;

  // find exit points on the boundaries
  std::vector<glm::ivec2> ij_exits = {};

  for (int i = 1; i < shape.x - 1; ++i)
    for (int j : {0, shape.y - 1})
      if (z(i - 1, j) > z(i, j) && z(i + 1, j) > z(i, j))
        ij_exits.push_back(glm::ivec2(i, j));

  for (int j = 1; j < shape.y - 1; ++j)
    for (int i : {0, shape.x - 1})
      if (z(i, j - 1) > z(i, j) && z(i, j + 1) > z(i, j))
        ij_exits.push_back(glm::ivec2(i, j));

  // ensure exit points of existing streams are considered
  for (const auto &stream : existing_streams)
  {
    if (!stream.empty())
    {
      const auto &last_pt = stream.back();
      glm::ivec2  ij_exit(
          std::clamp(static_cast<int>(std::round(last_pt.x * (shape.x - 1.f))),
                     0,
                     shape.x - 1),
          std::clamp(static_cast<int>(std::round(last_pt.y * (shape.y - 1.f))),
                     0,
                     shape.y - 1));

      if (std::find(ij_exits.begin(), ij_exits.end(), ij_exit) ==
          ij_exits.end())
      {
        ij_exits.push_back(ij_exit);
      }
    }
  }

  // fallback to all boundary cells if no exit was detected
  if (ij_exits.empty())
  {
    for (int i = 0; i < shape.x; ++i)
    {
      ij_exits.push_back(glm::ivec2(i, 0));
      ij_exits.push_back(glm::ivec2(i, shape.y - 1));
    }
    for (int j = 1; j < shape.y - 1; ++j)
    {
      ij_exits.push_back(glm::ivec2(0, j));
      ij_exits.push_back(glm::ivec2(shape.x - 1, j));
    }
  }

  // if favoring existing streams, adjust effective elevation along streams
  // to attract new flow paths into existing beds
  Array z_work = z;

  if (!existing_streams.empty())
  {
    float z_min = z.min();
    float z_max = z.max();
    float z_range = std::max(1e-4f, z_max - z_min);
    float base_depression = 0.15f * z_range;

    for (const auto &stream : existing_streams)
    {
      if (stream.empty()) continue;

      // propagate monotonic downstream depression along stream
      float curr_dep = base_depression;

      for (size_t k = 0; k < stream.size(); ++k)
      {
        int pi = std::clamp(
            static_cast<int>(std::round(stream[k].x * (shape.x - 1.f))),
            0,
            shape.x - 1);
        int pj = std::clamp(
            static_cast<int>(std::round(stream[k].y * (shape.y - 1.f))),
            0,
            shape.y - 1);

        // widen the valley (radius 2) so approaching paths feel the gradient
        for (int di = -2; di <= 2; ++di)
        {
          for (int dj = -2; dj <= 2; ++dj)
          {
            int ci = pi + di;
            int cj = pj + dj;

            if (ci >= 0 && ci < shape.x && cj >= 0 && cj < shape.y)
            {
              float d = std::sqrt(static_cast<float>(di * di + dj * dj));
              if (d <= 2.f)
              {
                float factor = 1.f - 0.35f * d;
                z_work(ci, cj) = std::min(z_work(ci, cj),
                                          z(ci, cj) - curr_dep * factor);
              }
            }
          }
        }

        if (k + 1 < stream.size())
        {
          int qi = std::clamp(
              static_cast<int>(std::round(stream[k + 1].x * (shape.x - 1.f))),
              0,
              shape.x - 1);
          int qj = std::clamp(
              static_cast<int>(std::round(stream[k + 1].y * (shape.y - 1.f))),
              0,
              shape.y - 1);

          int di = std::abs(qi - pi);
          int dj = std::abs(qj - pj);
          int n_steps = std::max(di, dj);

          for (int s = 1; s <= n_steps; ++s)
          {
            float t = static_cast<float>(s) / static_cast<float>(n_steps);
            int   ci = std::clamp(
                static_cast<int>(std::round(pi + t * (qi - pi))),
                0,
                shape.x - 1);
            int cj = std::clamp(
                static_cast<int>(std::round(pj + t * (qj - pj))),
                0,
                shape.y - 1);

            for (int rdi = -2; rdi <= 2; ++rdi)
            {
              for (int rdj = -2; rdj <= 2; ++rdj)
              {
                int ni = ci + rdi;
                int nj = cj + rdj;

                if (ni >= 0 && ni < shape.x && nj >= 0 && nj < shape.y)
                {
                  float d = std::sqrt(
                      static_cast<float>(rdi * rdi + rdj * rdj));
                  if (d <= 2.f)
                  {
                    float factor = 1.f - 0.35f * d;
                    z_work(ni, nj) = std::min(z_work(ni, nj),
                                              z(ni, nj) - curr_dep * factor);
                  }
                }
              }
            }
          }
        }

        // downstream points are progressively deeper
        curr_dep += 0.002f * z_range;
      }
    }
  }

  std::vector<std::vector<int>> i_path_list;
  std::vector<std::vector<int>> j_path_list;

  find_path_dijkstra(z_work,
                     ij_start,
                     ij_exits,
                     i_path_list,
                     j_path_list,
                     elevation_ratio,
                     distance_exponent,
                     upward_penalization);

  // keep the path with the minimum upward cumulated elevation
  size_t       kmin = 0;
  float        dz_cum_min = std::numeric_limits<float>::max();
  const Array &z_eval = existing_streams.empty() ? z : z_work;

  for (size_t k = 0; k < i_path_list.size(); k++)
  {
    float dz_cum = 0.f;
    for (size_t r = 0; r < i_path_list[k].size() - 1; r++)
    {
      float dz = z_eval(i_path_list[k][r + 1], j_path_list[k][r + 1]) -
                 z_eval(i_path_list[k][r], j_path_list[k][r]);
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

Path flow_stream(const TerrainTriMesh    &mesh,
                 glm::vec2                xy_start,
                 float                    elevation_ratio,
                 float                    distance_exponent,
                 float                    upward_penalization,
                 bool                     favor_domain_center,
                 const std::vector<Path> &existing_streams)
{
  if (mesh.size() == 0) return Path();

  const auto &pts = mesh.get_points();

  // find nearest vertex to starting position
  size_t start_idx = 0;
  float  min_dist2 = std::numeric_limits<float>::max();

  for (size_t i = 0; i < pts.size(); ++i)
  {
    float dx = pts[i].x - xy_start.x;
    float dy = pts[i].y - xy_start.y;
    float d2 = dx * dx + dy * dy;

    if (d2 < min_dist2)
    {
      min_dist2 = d2;
      start_idx = i;
    }
  }

  return flow_stream(mesh,
                     start_idx,
                     elevation_ratio,
                     distance_exponent,
                     upward_penalization,
                     favor_domain_center,
                     existing_streams);
}

Path flow_stream(const TerrainTriMesh    &mesh,
                 size_t                   start_vertex_index,
                 float                    elevation_ratio,
                 float                    distance_exponent,
                 float                    upward_penalization,
                 bool                     favor_domain_center,
                 const std::vector<Path> &existing_streams)
{
  if (mesh.size() == 0 || start_vertex_index >= mesh.size()) return Path();

  const auto &points = mesh.get_points();
  const auto &convex_hull = mesh.get_convex_hull();

  if (convex_hull.empty()) return Path();

  // find exit points along the boundary (convex hull)
  std::vector<size_t> exits;
  const size_t        hull_size = convex_hull.size();

  for (size_t k = 0; k < hull_size; ++k)
  {
    size_t prev_k = (k + hull_size - 1) % hull_size;
    size_t next_k = (k + 1) % hull_size;

    size_t curr_idx = convex_hull[k];
    size_t prev_idx = convex_hull[prev_k];
    size_t next_idx = convex_hull[next_k];

    float z_curr = points[curr_idx].z;
    float z_prev = points[prev_idx].z;
    float z_next = points[next_idx].z;

    // local minimum along the boundary
    if (z_curr <= z_prev && z_curr <= z_next)
    {
      exits.push_back(curr_idx);
    }
  }

  // ensure boundary exits of existing streams are considered
  for (const auto &stream : existing_streams)
  {
    if (!stream.empty())
    {
      const auto &last_pt = stream.back();
      float       min_d2 = std::numeric_limits<float>::max();
      size_t      closest_hull_idx = convex_hull[0];

      for (size_t h_idx : convex_hull)
      {
        float dx = points[h_idx].x - last_pt.x;
        float dy = points[h_idx].y - last_pt.y;
        float d2 = dx * dx + dy * dy;

        if (d2 < min_d2)
        {
          min_d2 = d2;
          closest_hull_idx = h_idx;
        }
      }

      if (std::find(exits.begin(), exits.end(), closest_hull_idx) ==
          exits.end())
      {
        exits.push_back(closest_hull_idx);
      }
    }
  }

  // fallback to all boundary vertices if no local dip was detected
  if (exits.empty())
  {
    exits = convex_hull;
  }

  // if favoring existing streams, adjust mesh vertex heights along existing
  // streams
  TerrainTriMesh mesh_work = mesh;

  if (!existing_streams.empty())
  {
    std::vector<glm::vec3> pts_work = points;
    float                  z_min = std::numeric_limits<float>::max();
    float                  z_max = std::numeric_limits<float>::lowest();

    for (const auto &p : points)
    {
      z_min = std::min(z_min, p.z);
      z_max = std::max(z_max, p.z);
    }

    float z_range = std::max(1e-4f, z_max - z_min);
    float depression = 0.05f * z_range;

    for (const auto &stream : existing_streams)
    {
      if (stream.empty()) continue;

      for (size_t i = 0; i < pts_work.size(); ++i)
      {
        float min_d2 = std::numeric_limits<float>::max();

        for (size_t k = 0; k < stream.size(); ++k)
        {
          float dx = pts_work[i].x - stream[k].x;
          float dy = pts_work[i].y - stream[k].y;
          float d2 = dx * dx + dy * dy;

          if (d2 < min_d2) min_d2 = d2;

          if (k + 1 < stream.size())
          {
            float sx = stream[k + 1].x - stream[k].x;
            float sy = stream[k + 1].y - stream[k].y;
            float seg_len2 = sx * sx + sy * sy;

            if (seg_len2 > 1e-12f)
            {
              float t = ((pts_work[i].x - stream[k].x) * sx +
                         (pts_work[i].y - stream[k].y) * sy) /
                        seg_len2;
              t = std::clamp(t, 0.f, 1.f);
              float px = stream[k].x + t * sx;
              float py = stream[k].y + t * sy;
              float dist2 = (pts_work[i].x - px) * (pts_work[i].x - px) +
                            (pts_work[i].y - py) * (pts_work[i].y - py);

              if (dist2 < min_d2) min_d2 = dist2;
            }
          }
        }

        // if vertex is within proximity of stream (approx 2% domain width)
        float thresh = 0.02f;
        if (min_d2 < thresh * thresh)
        {
          float w = 1.f - std::sqrt(min_d2) / thresh;
          pts_work[i].z -= depression * w;
        }
      }
    }

    mesh_work.get_points() = pts_work;
  }

  // --- Compute shortest paths using TerrainTriMesh hydrological metric

  TerrainTriMesh::MeshPathCostConfig config;
  config.metric = TerrainTriMesh::MeshPathCostConfig::Metric::HYDROLOGICAL;
  config.elevation_ratio = elevation_ratio;
  config.distance_exponent = distance_exponent;
  config.upward_penalization = upward_penalization;
  config.domain_center_weight = favor_domain_center ? 1.f : 0.f;

  TerrainTriMesh::ShortestPathResult result = mesh_work.compute_shortest_paths(
      start_vertex_index,
      config);

  // reconstruct path to each exit and pick the one with minimal upward
  // cumulated elevation
  float               min_cum_upward = std::numeric_limits<float>::max();
  std::vector<size_t> best_path;

  for (size_t exit_idx : exits)
  {
    std::vector<size_t> path_indices = mesh_work.reconstruct_path(
        start_vertex_index,
        exit_idx,
        result);

    if (path_indices.empty()) continue;

    // compute cumulative upward elevation along the path (using evaluated
    // points)
    const auto &eval_points = mesh_work.get_points();
    float       cum_upward = 0.f;

    for (size_t i = 0; i < path_indices.size() - 1; ++i)
    {
      float dz = eval_points[path_indices[i + 1]].z -
                 eval_points[path_indices[i]].z;

      if (dz > 0.f) cum_upward += dz;
    }

    if (cum_upward < min_cum_upward)
    {
      min_cum_upward = cum_upward;
      best_path = std::move(path_indices);
    }
  }

  if (best_path.empty()) return Path();

  std::vector<Point> path_points;
  path_points.reserve(best_path.size());

  for (size_t v_idx : best_path)
  {
    const auto &p = points[v_idx];
    path_points.emplace_back(p.x, p.y, p.z);
  }

  return Path(path_points);
}

} // namespace hmap

namespace hmap::va
{

Path flow_stream(const ComputeMode       &cm,
                 const VirtualArray      &z,
                 glm::vec2                start,
                 std::uint32_t            seed,
                 size_t                   control_points_count,
                 float                    elevation_ratio,
                 float                    distance_exponent,
                 float                    upward_penalization,
                 bool                     favor_domain_center,
                 const std::vector<Path> &existing_streams)
{
  TerrainTriMesh mesh = va::generate_terrain_tri_mesh_from_heightmap_random(
      cm,
      z,
      control_points_count,
      seed);

  return flow_stream(mesh,
                     start,
                     elevation_ratio,
                     distance_exponent,
                     upward_penalization,
                     favor_domain_center,
                     existing_streams);
}

} // namespace hmap::va
