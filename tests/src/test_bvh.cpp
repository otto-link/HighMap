#include "highmap/geometry/bvh.hpp"

#include <gtest/gtest.h>

TEST(BVH2DTest, EmptyBVHHandlesGracefully)
{
  hmap::BVH2D bvh;
  EXPECT_TRUE(bvh.empty());

  std::vector<uint32_t> results;
  bvh.query_box(hmap::AABB2D(0.f, 0.f, 1.f, 1.f), results);
  EXPECT_TRUE(results.empty());

  bvh.query_point(glm::vec2(0.5f, 0.5f), results);
  EXPECT_TRUE(results.empty());
}

TEST(BVH2DTest, SingleSegmentOverlapAndContainment)
{
  std::vector<hmap::Segment2D> segs;
  hmap::Segment2D              seg;
  seg.p0 = {0.2f, 0.2f};
  seg.p1 = {0.8f, 0.8f};
  seg.id = 42;
  segs.push_back(seg);

  hmap::BVH2D bvh(segs, 0.1f);
  EXPECT_FALSE(bvh.empty());

  std::vector<uint32_t> results;
  bvh.query_point(glm::vec2(0.5f, 0.5f), results);
  EXPECT_EQ(results.size(), 1u);

  results.clear();
  // Point far away
  bvh.query_point(glm::vec2(0.0f, 1.0f), results);
  EXPECT_TRUE(results.empty());

  // Box overlap
  results.clear();
  bvh.query_box(hmap::AABB2D(0.4f, 0.4f, 0.6f, 0.6f), results);
  EXPECT_EQ(results.size(), 1u);
}
