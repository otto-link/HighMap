/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

#include <cmath>
#include <vector>

#include <glm/glm.hpp>

#include "highmap/array.hpp"
#include "highmap/features.hpp"

#include <gtest/gtest.h>

using namespace hmap;

namespace
{

Array create_gaussian_peak_map(glm::ivec2 shape,
                               glm::vec2  center,
                               float      sigma,
                               float      height)
{
  Array arr(shape);
  for (int i = 0; i < shape.x; ++i)
  {
    for (int j = 0; j < shape.y; ++j)
    {
      float u = static_cast<float>(i) / (shape.x - 1);
      float v = static_cast<float>(j) / (shape.y - 1);
      float dx = u - center.x;
      float dy = v - center.y;
      float r2 = (dx * dx + dy * dy) / (2.f * sigma * sigma);
      arr(i, j) = height * std::exp(-r2);
    }
  }
  return arr;
}

} // namespace

TEST(PoiTest, ApexDetectionSinglePeak)
{
  glm::ivec2 shape = {128, 128};
  glm::vec2  expected_peak = {0.5f, 0.5f};
  Array      h = create_gaussian_peak_map(shape, expected_peak, 0.1f, 1.0f);

  PoiGenerationOptions opt;
  opt.scale_radius = 8;
  opt.prominence = 0.1f;
  opt.threshold = 0.5f;
  opt.min_distance = 0.1f;
  opt.max_points = 10;

  auto pois = generate_points_of_interest(h, PoiType::APEX, opt);

  ASSERT_FALSE(pois.empty());
  EXPECT_NEAR(pois[0].position.x, expected_peak.x, 0.03f);
  EXPECT_NEAR(pois[0].position.y, expected_peak.y, 0.03f);
  EXPECT_GT(pois[0].score, 0.6f);
  EXPECT_GT(pois[0].elevation, 0.9f);
  EXPECT_EQ(pois[0].type, PoiType::APEX);
}

TEST(PoiTest, SinkDetection)
{
  glm::ivec2 shape = {128, 128};
  glm::vec2  expected_sink = {0.5f, 0.5f};
  Array h = 1.0f - create_gaussian_peak_map(shape, expected_sink, 0.1f, 1.0f);

  PoiGenerationOptions opt;
  opt.scale_radius = 8;
  opt.prominence = 0.1f;
  opt.threshold = 0.5f;
  opt.min_distance = 0.1f;
  opt.max_points = 5;

  auto pois = generate_points_of_interest(h, PoiType::SINK, opt);

  ASSERT_FALSE(pois.empty());
  EXPECT_NEAR(pois[0].position.x, expected_sink.x, 0.03f);
  EXPECT_NEAR(pois[0].position.y, expected_sink.y, 0.03f);
  EXPECT_LT(pois[0].elevation, 0.15f);
  EXPECT_EQ(pois[0].type, PoiType::SINK);
}

TEST(PoiTest, MinDistanceEnforcement)
{
  glm::ivec2 shape = {128, 128};
  Array      h(shape);

  // Add 3 peaks close to each other
  h += create_gaussian_peak_map(shape, {0.50f, 0.50f}, 0.05f, 1.0f);
  h += create_gaussian_peak_map(shape, {0.53f, 0.50f}, 0.05f, 0.9f);
  h += create_gaussian_peak_map(shape, {0.56f, 0.50f}, 0.05f, 0.8f);

  PoiGenerationOptions opt;
  opt.scale_radius = 6;
  opt.prominence = 0.05f;
  opt.threshold = 0.4f;
  opt.min_distance = 0.1f; // min distance larger than peak spacing
  opt.max_points = 10;

  auto pois = generate_points_of_interest(h, PoiType::APEX, opt);

  for (size_t i = 0; i < pois.size(); ++i)
  {
    for (size_t j = i + 1; j < pois.size(); ++j)
    {
      float d = glm::distance(pois[i].position, pois[j].position);
      EXPECT_GE(d, opt.min_distance - 1e-5f);
    }
  }
}

TEST(PoiTest, ProminenceFiltering)
{
  glm::ivec2 shape = {128, 128};
  Array      h(shape);

  // Big peak
  h += create_gaussian_peak_map(shape, {0.3f, 0.3f}, 0.1f, 1.0f);
  // Tiny ripple
  h += create_gaussian_peak_map(shape, {0.7f, 0.7f}, 0.05f, 0.03f);

  PoiGenerationOptions opt;
  opt.scale_radius = 8;
  opt.prominence = 0.15f; // requires higher prominence than the tiny ripple
  opt.threshold = 0.2f;
  opt.min_distance = 0.05f;

  auto pois = generate_points_of_interest(h, PoiType::APEX, opt);

  // Should contain only the main peak
  EXPECT_EQ(pois.size(), 1u);
  if (!pois.empty())
  {
    EXPECT_NEAR(pois[0].position.x, 0.3f, 0.03f);
    EXPECT_NEAR(pois[0].position.y, 0.3f, 0.03f);
  }
}

TEST(PoiTest, MaskExclusion)
{
  glm::ivec2 shape = {128, 128};
  Array      h = create_gaussian_peak_map(shape, {0.5f, 0.5f}, 0.1f, 1.0f);

  Array mask(shape);
  mask = 1.f;
  // Mask out the center peak
  for (int i = 50; i <= 78; ++i)
    for (int j = 50; j <= 78; ++j)
      mask(i, j) = 0.f;

  PoiGenerationOptions opt;
  opt.scale_radius = 8;
  opt.p_mask = &mask;

  auto pois = generate_points_of_interest(h, PoiType::APEX, opt);
  EXPECT_TRUE(pois.empty());
}

TEST(PoiTest, CloudAndPositionsConversion)
{
  glm::ivec2 shape = {128, 128};
  Array      h = create_gaussian_peak_map(shape, {0.5f, 0.5f}, 0.1f, 1.0f);

  PoiGenerationOptions opt;
  opt.scale_radius = 8;
  opt.min_distance = 0.05f;
  opt.threshold = 0.5f;

  auto positions = generate_points_of_interest_positions(h, PoiType::APEX, opt);
  auto cloud = generate_points_of_interest_cloud(h, PoiType::APEX, opt);

  EXPECT_EQ(positions.size(), cloud.size());
  if (!positions.empty())
  {
    EXPECT_NEAR(positions[0].x, cloud[0].x, 1e-4f);
    EXPECT_NEAR(positions[0].y, cloud[0].y, 1e-4f);
  }
}

TEST(PoiTest, DeterministicSeed)
{
  glm::ivec2 shape = {128, 128};
  Array      h = create_gaussian_peak_map(shape, {0.5f, 0.5f}, 0.1f, 1.0f);

  PoiGenerationOptions opt1;
  opt1.jitter = 0.5f;
  opt1.seed = 42;

  PoiGenerationOptions opt2 = opt1;

  auto pois1 = generate_points_of_interest(h, PoiType::APEX, opt1);
  auto pois2 = generate_points_of_interest(h, PoiType::APEX, opt2);

  ASSERT_EQ(pois1.size(), pois2.size());
  for (size_t i = 0; i < pois1.size(); ++i)
  {
    EXPECT_FLOAT_EQ(pois1[i].position.x, pois2[i].position.x);
    EXPECT_FLOAT_EQ(pois1[i].position.y, pois2[i].position.y);
  }
}
