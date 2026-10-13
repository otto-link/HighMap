/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <utility>
#include <vector>

#include "highmap/array.hpp"
#include "highmap/carving.hpp"
#include "highmap/geometry/bvh.hpp"
#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/grids.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/geometry/point.hpp"
#include "highmap/internal/validation.hpp"
#include "highmap/math/core.hpp"
#include "highmap/math/profiles.hpp"
#include "highmap/vectors.hpp"

namespace hmap
{

namespace
{

struct PreprocessedPath
{
  Path               path;
  std::vector<float> arc_length;
  std::vector<float> curvature;
  std::vector<float> curv_radius;
  std::vector<float> curv_shape_factor;
};

std::vector<PreprocessedPath> preprocess_paths(
    const std::vector<Path>     &paths,
    float                        elevation_shift,
    float                        shift_ramp_start_ratio,
    float                        shift_ramp_end_ratio,
    ElevationLongitudinalProfile longitudinal_profile,
    float                        min_slope,
    bool                         enable_width_curvature_scaling,
    float                        curvature_radius_min)
{
  std::vector<PreprocessedPath> prep_paths;
  prep_paths.reserve(paths.size());

  for (const auto &path : paths)
  {
    if (path.empty()) continue;

    PreprocessedPath pp;
    pp.path = path;
    size_t npts = pp.path.size();
    pp.arc_length = pp.path.get_arc_length();

    // overall shift
    for (size_t k = 0; k < npts; ++k)
    {
      float t = pp.arc_length[k];

      if (t < shift_ramp_start_ratio)
        t /= shift_ramp_start_ratio;
      else if (t > 1.f - shift_ramp_end_ratio)
        t = (1.f - t) / shift_ramp_end_ratio;
      else
        t = 1.f;

      t = smoothstep3(t);
      pp.path[k].v += t * elevation_shift;
    }

    // monotonicity
    for (size_t k = 1; k < npts; ++k)
    {
      float dz_min = min_slope *
                     glm::length(glm::vec2(pp.path[k].x - pp.path[k - 1].x,
                                           pp.path[k].y - pp.path[k - 1].y));

      switch (longitudinal_profile)
      {
      case ElevationLongitudinalProfile::ELP_FLAT:
        pp.path[k].v = pp.path[0].v;
        break;

      case ElevationLongitudinalProfile::ELP_DECREASING:
        pp.path[k].v = std::min(pp.path[k].v, pp.path[k - 1].v - dz_min);
        break;

      case ElevationLongitudinalProfile::ELP_INCREASING:
        pp.path[k].v = std::max(pp.path[k].v, pp.path[k - 1].v + dz_min);
        break;

      case ElevationLongitudinalProfile::ELP_UNCHANGED: break;
      }
    }

    // curvature scaling
    if (enable_width_curvature_scaling)
    {
      Path path_curv = pp.path;
      pp.curvature = path_curv.get_curvature();
      pp.curv_radius.reserve(npts);
      pp.curv_shape_factor.reserve(npts);

      if (!pp.curvature.empty())
      {
        float curvature_max = 1.f / curvature_radius_min;
        float cmin = *std::min_element(pp.curvature.begin(),
                                       pp.curvature.end());
        float cmax = *std::max_element(pp.curvature.begin(),
                                       pp.curvature.end());
        float cscale = std::min(std::max(std::abs(cmax), std::abs(cmin)),
                                curvature_max);

        for (auto &v : pp.curvature)
        {
          v = std::clamp(v, -curvature_max, curvature_max);
          v = std::copysign(1.f, v) * smoothstep3(std::abs(v / cscale));

          if (v != 0.f)
          {
            float r = 1.f / v;
            r = std::copysign(1.f, r) * std::min(1.f, std::abs(r));
            pp.curv_radius.push_back(r);
          }
          else
          {
            pp.curv_radius.push_back(0.f);
          }
        }

        remap(pp.curv_radius, -1.f, 1.f);

        std::vector<size_t> sign_changes = find_sign_changes(pp.curvature);

        if (!sign_changes.empty())
        {
          std::vector<float> arc_gap(npts);
          float              arc_gap_max = 0.f;

          size_t sign_count = 0;
          size_t k0 = 0;
          size_t k1 = sign_changes[0];

          for (size_t k = 0; k < npts; ++k)
          {
            while (k >= k1 && sign_count < sign_changes.size())
            {
              k0 = k1;
              sign_count++;

              if (sign_count < sign_changes.size())
                k1 = sign_changes[sign_count];
              else
                k1 = npts;
            }

            arc_gap[k] = pp.arc_length[k1] - pp.arc_length[k0];
            arc_gap_max = std::max(arc_gap_max, arc_gap[k]);

            float t = 0.f;
            if (k1 > k0)
              t = float(k - k0) / float(k1 - k0);
            else
              t = 1.f;

            t = 1.f - std::abs(2.f * t - 1.f);
            t = smoothstep3(t);

            pp.curv_shape_factor.push_back(t);
          }

          for (size_t k = 0; k < npts; ++k)
            pp.curv_shape_factor[k] *= arc_gap[k] / arc_gap_max;

          pp.curv_shape_factor = moving_average(pp.curv_shape_factor, 1);
        }
      }
    }

    prep_paths.push_back(std::move(pp));
  }

  return prep_paths;
}

std::vector<Segment2D> build_segments(
    const std::vector<PreprocessedPath> &prep_paths)
{
  std::vector<Segment2D> segments;

  for (size_t p_idx = 0; p_idx < prep_paths.size(); ++p_idx)
  {
    const auto &pp = prep_paths[p_idx];
    size_t      npts = pp.path.size();

    if (npts == 1)
    {
      const auto &p0 = pp.path[0];
      Segment2D   seg;
      seg.p0 = glm::vec2(p0.x, p0.y);
      seg.p1 = glm::vec2(p0.x, p0.y);
      seg.v0 = p0.v;
      seg.v1 = p0.v;
      seg.s0 = 0.f;
      seg.s1 = 0.f;
      seg.k0 = 0;
      seg.k1 = 0;
      seg.path_index = static_cast<uint32_t>(p_idx);
      seg.id = static_cast<uint32_t>(segments.size());
      segments.push_back(seg);
    }
    else
    {
      for (size_t k = 0; k + 1 < npts; ++k)
      {
        const auto &p0 = pp.path[k];
        const auto &p1 = pp.path[k + 1];

        Segment2D seg;
        seg.p0 = glm::vec2(p0.x, p0.y);
        seg.p1 = glm::vec2(p1.x, p1.y);
        seg.v0 = p0.v;
        seg.v1 = p1.v;
        seg.s0 = pp.arc_length[k];
        seg.s1 = pp.arc_length[k + 1];
        seg.k0 = k;
        seg.k1 = k + 1;
        seg.path_index = static_cast<uint32_t>(p_idx);
        seg.id = static_cast<uint32_t>(segments.size());
        seg.d = seg.p1 - seg.p0;
        float len_sq = seg.d.x * seg.d.x + seg.d.y * seg.d.y;
        seg.inv_len_sq = (len_sq > 1e-12f) ? (1.f / len_sq) : 0.f;
        segments.push_back(seg);
      }
    }
  }

  return segments;
}

} // namespace

void trench(Array                       &z,
            const std::vector<Path>     &paths,
            float                        width,
            bool                         enable_width_depth_scaling,
            bool                         enable_width_distance_scaling,
            bool                         enable_width_curvature_scaling,
            float                        curvature_radius_min,
            float                        curv_width_ratio_min,
            float                        curv_width_ratio_max,
            RadialProfile                radial_profile,
            float                        radial_profile_parameter,
            ElevationLongitudinalProfile longitudinal_profile,
            float                        elevation_shift,
            float                        shift_ramp_start_ratio,
            float                        shift_ramp_end_ratio,
            float                        min_slope,
            size_t                       k_neighbors,
            const Array                 *p_noise_r,
            Array                       *p_bending_mask,
            int                          tile_size,
            glm::vec4                    bbox)
{
  (void)k_neighbors;
  if (!validate_non_empty(z)) return;
  if (p_noise_r && !validate_same_shape(z, *p_noise_r)) return;
  if (paths.empty()) return;

  const glm::ivec2 &shape = z.shape;

  auto prep_paths = preprocess_paths(paths,
                                     elevation_shift,
                                     shift_ramp_start_ratio,
                                     shift_ramp_end_ratio,
                                     longitudinal_profile,
                                     min_slope,
                                     enable_width_curvature_scaling,
                                     curvature_radius_min);
  if (prep_paths.empty()) return;

  float max_effective_width = width;
  if (p_noise_r)
    max_effective_width = std::max(0.f, width * (1.f + p_noise_r->max()));
  if (enable_width_curvature_scaling)
    max_effective_width *= std::max(1.f, curv_width_ratio_max);

  if (max_effective_width <= 0.f) return;

  auto profile_fct = get_radial_profile_function(radial_profile,
                                                 radial_profile_parameter);

  float dx = (bbox.y - bbox.x) / float(shape.x);
  float dy = (bbox.w - bbox.z) / float(shape.y);

  Array blending_mask;
  if (p_bending_mask) blending_mask = Array(shape);

  auto all_segments = build_segments(prep_paths);
  if (all_segments.empty()) return;

  // --- Build 2D Bounding Volume Hierarchy (BVH) over path segments

  BVH2D bvh(all_segments, max_effective_width);

  // Check if the path intersects the current heightmap or tile bbox domain
  AABB2D domain_aabb(bbox.x, bbox.z, bbox.y, bbox.w);
  if (!bvh.root_bbox().overlaps(domain_aabb)) return;

  // --- Tile Spatial Partitioning using BVH2D queries

  int actual_tile_size = std::max(1, tile_size);
  int num_tiles_x = (shape.x + actual_tile_size - 1) / actual_tile_size;
  int num_tiles_y = (shape.y + actual_tile_size - 1) / actual_tile_size;
  int total_tiles = num_tiles_x * num_tiles_y;

  std::vector<std::vector<uint32_t>> tile_segments(total_tiles);

#pragma omp parallel for schedule(static)
  for (int tile_idx = 0; tile_idx < total_tiles; ++tile_idx)
  {
    int tx = tile_idx % num_tiles_x;
    int ty = tile_idx / num_tiles_x;

    float tile_xmin = bbox.x + float(tx * actual_tile_size) * dx;
    float tile_xmax = bbox.x +
                      float(std::min((tx + 1) * actual_tile_size, shape.x)) *
                          dx;
    float tile_ymin = bbox.z + float(ty * actual_tile_size) * dy;
    float tile_ymax = bbox.z +
                      float(std::min((ty + 1) * actual_tile_size, shape.y)) *
                          dy;

    AABB2D tile_box(tile_xmin, tile_ymin, tile_xmax, tile_ymax);
    bvh.query_box(tile_box, tile_segments[tile_idx]);
  }

  std::vector<float> xg, yg;
  grid_xy_vector(xg, yg, shape, bbox, false);

  float max_w_sq = max_effective_width * max_effective_width;

#pragma omp parallel for schedule(dynamic, 8)
  for (int tile_idx = 0; tile_idx < total_tiles; ++tile_idx)
  {
    const auto &segs = tile_segments[tile_idx];
    if (segs.empty()) continue;

    int tx = tile_idx % num_tiles_x;
    int ty = tile_idx / num_tiles_x;

    int i_start = tx * actual_tile_size;
    int i_end = std::min(i_start + actual_tile_size, shape.x);
    int j_start = ty * actual_tile_size;
    int j_end = std::min(j_start + actual_tile_size, shape.y);

    for (int j = j_start; j < j_end; ++j)
    {
      float y = yg[j];

      for (int i = i_start; i < i_end; ++i)
      {
        float x = xg[i];

        float min_dist_sq = max_w_sq;
        float best_t = 0.f;
        int   best_seg_idx = -1;

        for (uint32_t seg_idx : segs)
        {
          const auto &seg = all_segments[seg_idx];
          float       qx = x - seg.p0.x;
          float       qy = y - seg.p0.y;

          float t = std::clamp((qx * seg.d.x + qy * seg.d.y) * seg.inv_len_sq,
                               0.f,
                               1.f);

          float proj_x = seg.p0.x + t * seg.d.x;
          float proj_y = seg.p0.y + t * seg.d.y;

          float d_sq = (x - proj_x) * (x - proj_x) +
                       (y - proj_y) * (y - proj_y);

          if (d_sq < min_dist_sq)
          {
            min_dist_sq = d_sq;
            best_t = t;
            best_seg_idx = static_cast<int>(seg_idx);
          }
        }

        if (best_seg_idx < 0) continue;

        const auto &seg = all_segments[best_seg_idx];
        const auto &pp = prep_paths[seg.path_index];
        size_t      k0 = (best_t <= 0.5f) ? seg.k0 : seg.k1;
        float       zref = (1.f - best_t) * seg.v0 + best_t * seg.v1;
        float       arc = (1.f - best_t) * seg.s0 + best_t * seg.s1;

        float dr = p_noise_r ? (*p_noise_r)(i, j) : 0.f;
        float effective_width = std::max(0.f, width * (1.f + dr));

        if (enable_width_distance_scaling) effective_width *= smoothstep3(arc);

        if (enable_width_depth_scaling && std::abs(elevation_shift) > 1e-6f)
        {
          float dz = std::abs((z(i, j) - zref) / elevation_shift);
          effective_width *= std::clamp(dz, 0.f, 1.f);
        }

        if (enable_width_curvature_scaling && !pp.curvature.empty())
        {
          size_t km = (k0 > 0) ? (k0 - 1) : 0;
          size_t kp = std::min(k0 + 1, pp.path.size() - 1);

          float s = -classify_point(pp.path[km],
                                    pp.path[k0],
                                    pp.path[kp],
                                    Point(x, y));

          float t_long = arc;
          t_long = t_long * (1.f - t_long) * 4.f;

          float camp = std::abs(pp.curv_radius[k0]) * t_long;

          effective_width *= lerp(
              1.f,
              std::max(curv_width_ratio_min,
                       1.f + (curv_width_ratio_max - 1.f) * s * camp),
              pp.curv_shape_factor[k0]);
        }

        if (effective_width <= 0.f ||
            min_dist_sq > effective_width * effective_width)
        {
          continue;
        }

        float r = std::sqrt(min_dist_sq) / effective_width;
        if (r >= 0.f && r <= 1.f)
        {
          float t = profile_fct(r);
          z(i, j) = lerp(zref, z(i, j), t);
          if (p_bending_mask) blending_mask(i, j) = 1.f - t;
        }
      }
    }
  }

  // --- Outputs

  if (p_bending_mask) *p_bending_mask = std::move(blending_mask);
}

void trench(Array                       &z,
            const Path                  &path,
            float                        width,
            bool                         enable_width_depth_scaling,
            bool                         enable_width_distance_scaling,
            bool                         enable_width_curvature_scaling,
            float                        curvature_radius_min,
            float                        curv_width_ratio_min,
            float                        curv_width_ratio_max,
            RadialProfile                radial_profile,
            float                        radial_profile_parameter,
            ElevationLongitudinalProfile longitudinal_profile,
            float                        elevation_shift,
            float                        shift_ramp_start_ratio,
            float                        shift_ramp_end_ratio,
            float                        min_slope,
            size_t                       k_neighbors,
            const Array                 *p_noise_r,
            Array                       *p_bending_mask,
            int                          tile_size,
            glm::vec4                    bbox)
{
  if (!validate_non_empty(path, "Path")) return;

  std::vector<Path> paths = {path};
  trench(z,
         paths,
         width,
         enable_width_depth_scaling,
         enable_width_distance_scaling,
         enable_width_curvature_scaling,
         curvature_radius_min,
         curv_width_ratio_min,
         curv_width_ratio_max,
         radial_profile,
         radial_profile_parameter,
         longitudinal_profile,
         elevation_shift,
         shift_ramp_start_ratio,
         shift_ramp_end_ratio,
         min_slope,
         k_neighbors,
         p_noise_r,
         p_bending_mask,
         tile_size,
         bbox);
}

} // namespace hmap
