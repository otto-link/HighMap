/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file bvh.hpp
 * @brief 2D Bounding Volume Hierarchy (BVH / AABB Tree) for spatial line
 * segment queries.
 */
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#include <glm/glm.hpp>

namespace hmap
{

/**
 * @brief 2D Axis-Aligned Bounding Box (AABB).
 */
struct AABB2D
{
  glm::vec2 min{std::numeric_limits<float>::infinity()};
  glm::vec2 max{-std::numeric_limits<float>::infinity()};

  constexpr AABB2D() noexcept = default;

  constexpr AABB2D(const glm::vec2 &min, const glm::vec2 &max) noexcept
      : min(min), max(max)
  {
  }

  constexpr AABB2D(float min_x, float min_y, float max_x, float max_y) noexcept
      : min(min_x, min_y), max(max_x, max_y)
  {
  }

  constexpr bool contains(const glm::vec2 &p) const noexcept
  {
    return p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y;
  }

  constexpr bool overlaps(const AABB2D &other) const noexcept
  {
    return min.x <= other.max.x && max.x >= other.min.x &&
           min.y <= other.max.y && max.y >= other.min.y;
  }

  constexpr void expand(const glm::vec2 &p) noexcept
  {
    min = glm::min(min, p);
    max = glm::max(max, p);
  }

  constexpr void expand(const AABB2D &other) noexcept
  {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
  }

  constexpr glm::vec2 center() const noexcept
  {
    return 0.5f * (min + max);
  }

  constexpr glm::vec2 extent() const noexcept
  {
    return max - min;
  }
};

/**
 * @brief 2D Directed line segment with metadata.
 */
struct Segment2D
{
  glm::vec2 p0{0.f};
  glm::vec2 p1{0.f};
  float     v0{0.f};
  float     v1{0.f};
  float     s0{0.f};
  float     s1{0.f};
  size_t    k0{0};
  size_t    k1{0};
  uint32_t  path_index{0};
  uint32_t  id{0};
  glm::vec2 d{0.f};
  float     inv_len_sq{0.f};

  constexpr AABB2D bounds(float padding = 0.f) const noexcept
  {
    return AABB2D(std::min(p0.x, p1.x) - padding,
                  std::min(p0.y, p1.y) - padding,
                  std::max(p0.x, p1.x) + padding,
                  std::max(p0.y, p1.y) + padding);
  }

  constexpr glm::vec2 dir() const noexcept
  {
    return d;
  }

  constexpr float len_sq() const noexcept
  {
    return d.x * d.x + d.y * d.y;
  }
};

/**
 * @brief 2D Linear Bounding Volume Hierarchy for line segments.
 */
class BVH2D
{
public:
  struct Node
  {
    AABB2D   bbox;
    int32_t  left{-1};  ///< Left child index or -1 if leaf
    int32_t  right{-1}; ///< Right child index or -1 if leaf
    uint32_t seg_start{0};
    uint32_t seg_count{0};

    constexpr bool is_leaf() const noexcept
    {
      return left < 0;
    }
  };

  BVH2D() = default;

  /**
   * @brief Build BVH over line segments with capsule expansion padding.
   */
  explicit BVH2D(const std::vector<Segment2D> &segments, float padding = 0.f);

  /**
   * @brief Rebuild BVH with given segments and padding.
   */
  void build(const std::vector<Segment2D> &segments, float padding = 0.f);

  /**
   * @brief Clear BVH.
   */
  void clear() noexcept;

  bool empty() const noexcept
  {
    return nodes.empty();
  }

  const AABB2D &root_bbox() const noexcept
  {
    return nodes[0].bbox;
  }

  /**
   * @brief Query all segments whose padded bounding boxes overlap with @p
   * query_box.
   */
  void query_box(const AABB2D          &query_box,
                 std::vector<uint32_t> &out_seg_indices) const;

  /**
   * @brief Query all segments whose padded bounding boxes contain point @p p.
   */
  void query_point(const glm::vec2       &p,
                   std::vector<uint32_t> &out_seg_indices) const;

  const std::vector<Segment2D> &get_segments() const noexcept
  {
    return segments;
  }

  const std::vector<Node> &get_nodes() const noexcept
  {
    return nodes;
  }

private:
  std::vector<Node>      nodes;
  std::vector<Segment2D> segments;

  int32_t build_recursive(uint32_t start, uint32_t count, float padding);
};

} // namespace hmap
