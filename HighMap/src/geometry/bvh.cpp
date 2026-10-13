/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <stack>

#include "highmap/geometry/bvh.hpp"

namespace hmap
{

BVH2D::BVH2D(const std::vector<Segment2D> &segments, float padding)
{
  this->build(segments, padding);
}

void BVH2D::build(const std::vector<Segment2D> &input_segments, float padding)
{
  this->clear();
  if (input_segments.empty()) return;

  this->segments = input_segments;
  this->nodes.reserve(input_segments.size() * 2);

  this->build_recursive(0,
                        static_cast<uint32_t>(this->segments.size()),
                        padding);
}

void BVH2D::clear() noexcept
{
  this->nodes.clear();
  this->segments.clear();
}

int32_t BVH2D::build_recursive(uint32_t start, uint32_t count, float padding)
{
  Node node;
  node.seg_start = start;
  node.seg_count = count;

  // compute bounding box of all segments in this node
  AABB2D node_bbox;
  AABB2D centroid_bbox;

  for (uint32_t i = 0; i < count; ++i)
  {
    const auto &seg = this->segments[start + i];
    node_bbox.expand(seg.bounds(padding));

    glm::vec2 centroid = 0.5f * (seg.p0 + seg.p1);
    centroid_bbox.expand(centroid);
  }

  node.bbox = node_bbox;

  // leaf condition: small number of segments
  if (count <= 4)
  {
    int32_t node_idx = static_cast<int32_t>(this->nodes.size());
    this->nodes.push_back(node);
    return node_idx;
  }

  // split along longest axis of centroids
  glm::vec2 ext = centroid_bbox.extent();
  int       axis = (ext.x >= ext.y) ? 0 : 1;

  auto begin_it = this->segments.begin() + start;
  auto mid_it = begin_it + (count / 2);
  auto end_it = begin_it + count;

  if (axis == 0)
  {
    std::nth_element(begin_it,
                     mid_it,
                     end_it,
                     [](const Segment2D &a, const Segment2D &b)
                     { return (a.p0.x + a.p1.x) < (b.p0.x + b.p1.x); });
  }
  else
  {
    std::nth_element(begin_it,
                     mid_it,
                     end_it,
                     [](const Segment2D &a, const Segment2D &b)
                     { return (a.p0.y + a.p1.y) < (b.p0.y + b.p1.y); });
  }

  uint32_t left_count = count / 2;
  uint32_t right_count = count - left_count;

  // allocate node slot
  int32_t node_idx = static_cast<int32_t>(this->nodes.size());
  this->nodes.push_back(node);

  int32_t left_child = this->build_recursive(start, left_count, padding);
  int32_t right_child = this->build_recursive(start + left_count,
                                              right_count,
                                              padding);

  this->nodes[node_idx].left = left_child;
  this->nodes[node_idx].right = right_child;

  return node_idx;
}

void BVH2D::query_box(const AABB2D          &query_box,
                      std::vector<uint32_t> &out_seg_indices) const
{
  if (this->empty()) return;

  // iterative traversal using stack
  int32_t stack[64];
  int32_t stack_ptr = 0;
  stack[stack_ptr++] = 0;

  while (stack_ptr > 0)
  {
    int32_t     node_idx = stack[--stack_ptr];
    const auto &node = this->nodes[node_idx];

    if (!node.bbox.overlaps(query_box)) continue;

    if (node.is_leaf())
    {
      for (uint32_t i = 0; i < node.seg_count; ++i)
      {
        out_seg_indices.push_back(this->segments[node.seg_start + i].id);
      }
    }
    else
    {
      if (node.left >= 0) stack[stack_ptr++] = node.left;
      if (node.right >= 0) stack[stack_ptr++] = node.right;
    }
  }
}

void BVH2D::query_point(const glm::vec2       &p,
                        std::vector<uint32_t> &out_seg_indices) const
{
  if (this->empty()) return;

  int32_t stack[64];
  int32_t stack_ptr = 0;
  stack[stack_ptr++] = 0;

  while (stack_ptr > 0)
  {
    int32_t     node_idx = stack[--stack_ptr];
    const auto &node = this->nodes[node_idx];

    if (!node.bbox.contains(p)) continue;

    if (node.is_leaf())
    {
      for (uint32_t i = 0; i < node.seg_count; ++i)
      {
        out_seg_indices.push_back(this->segments[node.seg_start + i].id);
      }
    }
    else
    {
      if (node.left >= 0) stack[stack_ptr++] = node.left;
      if (node.right >= 0) stack[stack_ptr++] = node.right;
    }
  }
}

} // namespace hmap
