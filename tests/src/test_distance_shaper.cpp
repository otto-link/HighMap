#include "highmap/filters.hpp"
#include "highmap/math/array.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include <gtest/gtest.h>

using namespace hmap;

TEST(DistanceShaper, EmptyArray)
{
  Array empty;
  Array res = distance_shaper(empty);

  EXPECT_EQ(res.size(), 0);
}

TEST(DistanceShaper, GeneratesSlopesFromFlatShape)
{
  glm::ivec2 shape = {128, 128};
  Array      z(shape, 0.f);

  // create central flat plateau
  for (int j = 48; j < 80; ++j)
  {
    for (int i = 48; i < 80; ++i)
    {
      z(i, j) = 1.f;
    }
  }

  Array sloped = distance_shaper(z,
                                 0.03f,
                                 8,
                                 0.f,
                                 0.f,
                                 DistanceMode::PYRAMID,
                                 DistanceMethod::RT);

  EXPECT_EQ(sloped.shape, shape);

  // plateau center remains at full height
  EXPECT_NEAR(sloped(64, 64), 1.f, 1e-3f);

  // points outside the plateau have received descending slopes
  EXPECT_GT(sloped(64, 90), 0.f);
  EXPECT_LT(sloped(64, 90), 1.f);

  // distance further out should be lower
  EXPECT_LT(sloped(64, 110), sloped(64, 90));
}

TEST(DistanceShaper, SkewControl)
{
  glm::ivec2 shape = {128, 128};
  Array      z(shape, 0.f);

  // create central flat plateau
  for (int j = 48; j < 80; ++j)
  {
    for (int i = 48; i < 80; ++i)
    {
      z(i, j) = 1.f;
    }
  }

  // angle = 0 (points along +x), skew = 0.8
  Array skewed = distance_shaper(z,
                                 0.03f,
                                 8,
                                 0.8f,
                                 0.f,
                                 DistanceMode::PYRAMID,
                                 DistanceMethod::RT);

  // In the +x direction (dip slope), slope is gentler and reaches higher at the same distance
  float val_pos_x = skewed(95, 64);

  // In the -x direction (scarp face), slope drops much faster
  float val_neg_x = skewed(33, 64);

  EXPECT_GT(val_pos_x, val_neg_x);
}

TEST(DistanceShaper, AsteriskCutsBetweenDirections)
{
  glm::ivec2 shape = {128, 128};
  Array      z(shape, 0.f);

  // central flat square
  for (int j = 56; j < 72; ++j)
  {
    for (int i = 56; i < 72; ++i)
    {
      z(i, j) = 1.f;
    }
  }

  Array pyramid = distance_shaper(z,
                                  0.03f,
                                  4,
                                  0.f,
                                  0.f,
                                  DistanceMode::PYRAMID,
                                  DistanceMethod::RT);

  Array asterisk = distance_shaper(z,
                                   0.03f,
                                   4,
                                   0.f,
                                   0.f,
                                   DistanceMode::ASTERISK,
                                   DistanceMethod::RT);

  // Both should preserve the center plateau
  EXPECT_NEAR(pyramid(64, 64), 1.f, 1e-3f);
  EXPECT_NEAR(asterisk(64, 64), 1.f, 1e-3f);

  // Between the 4 cardinal directions (e.g. at 45 degrees diagonal),
  // Asterisk cuts in lower than the flat pyramid facet
  float pyr_diag = pyramid(85, 85);
  float ast_diag = asterisk(85, 85);

  EXPECT_LE(ast_diag, pyr_diag);
}

TEST(DistanceShaper, ClassicMethod)
{
  glm::ivec2 shape = {64, 64};
  Array      z(shape, 0.f);

  for (int j = 24; j < 40; ++j)
  {
    for (int i = 24; i < 40; ++i)
    {
      z(i, j) = 1.f;
    }
  }

  Array classic = distance_shaper(z,
                                  0.05f,
                                  8,
                                  0.f,
                                  0.f,
                                  DistanceMode::PYRAMID,
                                  DistanceMethod::CLASSIC);

  EXPECT_EQ(classic.shape, shape);
  EXPECT_NEAR(classic(32, 32), 1.f, 1e-3f);
  EXPECT_LT(classic(32, 45), 1.f);
}

TEST(DistanceShaper, MaskAndMixRatio)
{
  glm::ivec2 shape = {64, 64};
  Array      z(shape, 0.f);
  z(32, 32) = 1.f;

  Array mask(shape, 0.f);
  // Only allow left half to be affected
  for (int j = 0; j < shape.y; ++j)
  {
    for (int i = 0; i < shape.x / 2; ++i)
    {
      mask(i, j) = 1.f;
    }
  }

  Array masked = distance_shaper(z,
                                 0.05f,
                                 8,
                                 0.f,
                                 0.f,
                                 DistanceMode::PYRAMID,
                                 DistanceMethod::RT,
                                 0.f,
                                 0.f,
                                 0.f,
                                 false,
                                 false,
                                 1.f,
                                 0,
                                 &mask);

  // Right side outside mask should remain 0 (untouched)
  EXPECT_EQ(masked(50, 32), 0.f);
}
