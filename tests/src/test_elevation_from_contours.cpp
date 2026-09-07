#include "highmap.hpp"

#include <gtest/gtest.h>

namespace
{

// axis-aligned closed square contour, centre (cx, cy), half-size half
hmap::Path square(float cx, float cy, float half)
{
  std::vector<float> x = {cx - half, cx + half, cx + half, cx - half};
  std::vector<float> y = {cy - half, cy - half, cy + half, cy + half};
  hmap::Path         p(x, y);
  p.set_closed(true);
  return p;
}

bool is_empty(const hmap::Array &a)
{
  return a.size() == 0;
}

} // namespace

TEST(ElevationFromContours, InvalidInputsReturnEmpty)
{
  glm::ivec2 shape = {32, 32};

  // no contours
  EXPECT_TRUE(is_empty(hmap::elevation_from_contours(shape,
                                                     std::vector<hmap::Path>{},
                                                     std::vector<float>{})));

  // contours / elevations size mismatch
  EXPECT_TRUE(is_empty(hmap::elevation_from_contours(shape,
                                                     {square(0.5f, 0.5f, 0.2f)},
                                                     {0.f, 1.f})));

  // degenerate contour (2 points)
  hmap::Path two({0.f, 1.f}, {0.f, 1.f});
  EXPECT_TRUE(is_empty(hmap::elevation_from_contours(shape, {two}, {0.f})));

  // probability map with the wrong shape
  hmap::Array bad_p({16, 16}, 0.5f);
  EXPECT_TRUE(is_empty(hmap::elevation_from_contours(shape,
                                                     {square(0.5f, 0.5f, 0.2f)},
                                                     {0.f},
                                                     &bad_p)));
}

TEST(ElevationFromContours, ContourPixelsAreExact)
{
  glm::ivec2              shape = {64, 64};
  std::vector<hmap::Path> c = {square(0.5f, 0.5f, 0.4f),
                               square(0.5f, 0.5f, 0.2f)};
  std::vector<float>      h = {0.2f, 0.6f};

  hmap::Array z = hmap::elevation_from_contours(shape, c, h, nullptr, 0.f);
  ASSERT_EQ(z.shape, shape);

  // outline pixel of the outer square: x = 0.1 -> i = round(0.1 * 63) = 6
  EXPECT_NEAR(z(6, 32), 0.2f, 1e-6f);
  // outline pixel of the inner square: x = 0.3 -> i = round(0.3 * 63) = 19
  EXPECT_NEAR(z(19, 32), 0.6f, 1e-6f);
  // top edge of the outer square: y = 0.9 -> j = round(0.9 * 63) = 57
  EXPECT_NEAR(z(32, 57), 0.2f, 1e-6f);
}

TEST(ElevationFromContours, BetweenNestedContoursValuesAreBounded)
{
  glm::ivec2              shape = {64, 64};
  std::vector<hmap::Path> c = {square(0.5f, 0.5f, 0.4f),
                               square(0.5f, 0.5f, 0.2f)};
  std::vector<float>      h = {0.2f, 0.6f};

  for (float r : {0.f, 1.f})
  {
    hmap::Array z = hmap::elevation_from_contours(shape, c, h, nullptr, r, 3);

    // ring between the squares along the middle row (outer outline at i = 6,
    // inner outline at i = 19)
    for (int i = 7; i < 19; ++i)
    {
      EXPECT_GE(z(i, 32), 0.2f - 1e-5f) << "r=" << r << " i=" << i;
      EXPECT_LE(z(i, 32), 0.6f + 1e-5f) << "r=" << r << " i=" << i;
    }

    // strictly inside the ring the terrain must have left the contour levels
    EXPECT_GT(z(12, 32), 0.2f + 1e-3f) << "r=" << r;
    EXPECT_LT(z(12, 32), 0.6f - 1e-3f) << "r=" << r;

    // deterministic case: monotone along the row from outer to inner contour
    if (r == 0.f)
    {
      for (int i = 6; i < 19; ++i)
        EXPECT_LE(z(i, 32), z(i + 1, 32) + 1e-6f) << "i=" << i;
    }
  }
}

TEST(ElevationFromContours, DeterministicModeIgnoresSeed)
{
  glm::ivec2              shape = {48, 48};
  std::vector<hmap::Path> c = {square(0.5f, 0.5f, 0.4f),
                               square(0.5f, 0.5f, 0.2f)};
  std::vector<float>      h = {0.f, 1.f};

  hmap::Array z0 = hmap::elevation_from_contours(shape, c, h, nullptr, 0.f, 0);
  hmap::Array z1 = hmap::elevation_from_contours(shape, c, h, nullptr, 0.f, 42);

  for (int k = 0; k < z0.size(); ++k)
    EXPECT_FLOAT_EQ(z0(k), z1(k)) << "k=" << k;
}

TEST(ElevationFromContours, StochasticModeIsSeedReproducible)
{
  glm::ivec2              shape = {48, 48};
  std::vector<hmap::Path> c = {square(0.5f, 0.5f, 0.4f),
                               square(0.5f, 0.5f, 0.2f)};
  std::vector<float>      h = {0.f, 1.f};

  hmap::Array a = hmap::elevation_from_contours(shape, c, h, nullptr, 1.f, 7);
  hmap::Array b = hmap::elevation_from_contours(shape, c, h, nullptr, 1.f, 7);
  hmap::Array d = hmap::elevation_from_contours(shape, c, h, nullptr, 1.f, 8);

  for (int k = 0; k < a.size(); ++k)
    EXPECT_FLOAT_EQ(a(k), b(k)) << "k=" << k;

  int ndiff = 0;
  for (int k = 0; k < a.size(); ++k)
    ndiff += (a(k) != d(k));
  EXPECT_GT(ndiff, 0);
}

TEST(ElevationFromContours, LeafZoneRisesAboveItsContour)
{
  glm::ivec2              shape = {64, 64};
  std::vector<hmap::Path> c = {square(0.5f, 0.5f, 0.4f),
                               square(0.5f, 0.5f, 0.2f)};
  std::vector<float>      h = {0.2f, 0.6f};

  hmap::Array z =
      hmap::elevation_from_contours(shape, c, h, nullptr, 0.f, 0, 0.5f, 1.f);

  // spacing = 0.4, peak = 0.6 + 0.5 * 0.4 = 0.8 at the centre (farthest point)
  EXPECT_NEAR(z(32, 32), 0.8f, 0.05f);
  EXPECT_GT(z(28, 32), 0.6f);
  EXPECT_LT(z(28, 32), 0.8f);
}

TEST(ElevationFromContours, OutsideDropsBelowRootContour)
{
  glm::ivec2              shape = {64, 64};
  std::vector<hmap::Path> c = {square(0.5f, 0.5f, 0.2f)};
  std::vector<float>      h = {0.5f};

  // single contour: spacing falls back to 1 -> outside floor 0.5 - 1.0 * 1
  hmap::Array z =
      hmap::elevation_from_contours(shape, c, h, nullptr, 0.f, 0, 0.5f, 1.f);

  EXPECT_NEAR(z(0, 0), -0.5f, 0.05f); // farthest corner
  EXPECT_LT(z(10, 32), 0.5f);         // outside, near the contour
  EXPECT_GT(z(10, 32), -0.5f);
  EXPECT_NEAR(z(32, 32), 1.0f, 0.05f); // leaf peak: 0.5 + 0.5 * 1
}

TEST(ElevationFromContours, BasinLeafSinksBelowItsContour)
{
  glm::ivec2              shape = {64, 64};
  std::vector<hmap::Path> c = {square(0.5f, 0.5f, 0.4f),
                               square(0.5f, 0.5f, 0.2f)};
  std::vector<float>      h = {0.6f, 0.2f};

  hmap::Array z =
      hmap::elevation_from_contours(shape, c, h, nullptr, 0.f, 0, 0.5f, 1.f);

  // inner contour lower than its parent: interior sinks to 0.2 - 0.5 * 0.4
  EXPECT_NEAR(z(32, 32), 0.0f, 0.05f);
  EXPECT_LT(z(28, 32), 0.2f);

  // ring between the contours descends from 0.6 to 0.2
  EXPECT_GT(z(12, 32), 0.2f);
  EXPECT_LT(z(12, 32), 0.6f);
}

TEST(ElevationFromContours, HighProbabilityKeepsTerrainLower)
{
  glm::ivec2              shape = {64, 64};
  std::vector<hmap::Path> c = {square(0.5f, 0.5f, 0.45f),
                               square(0.5f, 0.5f, 0.1f)};
  std::vector<float>      h = {0.f, 1.f};

  // high probability in the lower half of the domain
  hmap::Array p(shape, 0.5f);
  for (int j = 0; j < 32; ++j)
    for (int i = 0; i < 64; ++i)
      p(i, j) = 0.9f;

  hmap::Array z = hmap::elevation_from_contours(shape, c, h, &p, 0.f, 0);

  // mirrored mid-ring samples: the high-probability half must be lower
  float lo = 0.f, hi = 0.f;
  int   n = 0;
  for (int i = 20; i < 44; ++i)
  {
    lo += z(i, 14);
    hi += z(i, 49);
    ++n;
  }
  EXPECT_LT(lo / n, hi / n);
}

TEST(ElevationFromContours, RisingZoneHasNoPits)
{
  glm::ivec2              shape = {64, 64};
  std::vector<hmap::Path> c = {square(0.5f, 0.5f, 0.4f),
                               square(0.5f, 0.5f, 0.15f)};
  std::vector<float>      h = {0.f, 1.f};

  hmap::Array z = hmap::elevation_from_contours(shape, c, h, nullptr, 1.f, 11);

  // every pixel of the ring between the contours (outer outline at i,j = 6
  // and 57, inner outline at 23 and 41) must have an 8-neighbour that is not
  // higher than itself
  int pits = 0;
  for (int j = 7; j < 57; ++j)
    for (int i = 7; i < 57; ++i)
    {
      const bool ring = (i < 23 || i > 41 || j < 23 || j > 41);
      if (!ring) continue;

      const float v = z(i, j);
      bool        has_lower = false;
      for (int dj = -1; dj <= 1; ++dj)
        for (int di = -1; di <= 1; ++di)
          if ((di || dj) && z(i + di, j + dj) <= v) has_lower = true;
      pits += !has_lower;
    }
  EXPECT_EQ(pits, 0);
}

namespace
{

// largest absolute difference between horizontally adjacent pixels in a row
float max_row_jump(const hmap::Array &z, int j, int i0, int i1)
{
  float jump = 0.f;
  for (int i = i0; i < i1; ++i)
    jump = std::max(jump, std::abs(z(i + 1, j) - z(i, j)));
  return jump;
}

} // namespace

TEST(ElevationFromContours, OutsideZoneIsContinuousBetweenRoots)
{
  glm::ivec2              shape = {96, 64};
  std::vector<hmap::Path> c = {square(0.2f, 0.5f, 0.1f),
                               square(0.8f, 0.5f, 0.1f)};
  std::vector<float>      h = {0.2f, 0.5f};

  hmap::Array z = hmap::elevation_from_contours(shape, c, h, nullptr, 0.f);

  // row through both contours, between them (outlines at i = 29 and 66)
  // the elevation must vary smoothly: no jump larger than a few percent of
  // the elevation gap per pixel
  EXPECT_LT(max_row_jump(z, 32, 30, 65), 0.03f);
}

TEST(ElevationFromContours, SiblingsAtDifferentElevationsAreContinuous)
{
  glm::ivec2 shape = {96, 64};
  // enclosing contour with two children: a hill and a basin
  std::vector<hmap::Path> c = {square(0.5f, 0.5f, 0.45f),
                               square(0.25f, 0.5f, 0.08f),
                               square(0.75f, 0.5f, 0.08f)};
  std::vector<float>      h = {0.2f, 0.6f, 0.0f};

  hmap::Array z = hmap::elevation_from_contours(shape, c, h, nullptr, 0.f);

  // row through both children, in the ring between them (outlines at
  // i = 31 and 64)
  EXPECT_LT(max_row_jump(z, 32, 32, 63), 0.03f);
}

TEST(ElevationFromContours, RasterContoursOverloadWorks)
{
  glm::ivec2  shape = {64, 64};
  hmap::Array raster_contours(shape, 0.f);

  // Draw outer square contour at elevation 0.2
  for (int i = 10; i <= 54; ++i)
  {
    raster_contours(i, 10) = 0.2f;
    raster_contours(i, 54) = 0.2f;
    raster_contours(10, i) = 0.2f;
    raster_contours(54, i) = 0.2f;
  }

  // Draw inner square contour at elevation 0.6
  for (int i = 24; i <= 40; ++i)
  {
    raster_contours(i, 24) = 0.6f;
    raster_contours(i, 40) = 0.6f;
    raster_contours(24, i) = 0.6f;
    raster_contours(40, i) = 0.6f;
  }

  hmap::Array z = hmap::elevation_from_contours(raster_contours, nullptr, 0.f);
  ASSERT_EQ(z.shape, shape);

  // Contour pixels should match exactly
  EXPECT_NEAR(z(10, 32), 0.2f, 1e-6f);
  EXPECT_NEAR(z(24, 32), 0.6f, 1e-6f);

  // Point between nested contours (e.g. at (17, 32)) should be strictly between
  // 0.2 and 0.6
  EXPECT_GT(z(17, 32), 0.2f);
  EXPECT_LT(z(17, 32), 0.6f);

  // Point inside innermost contour (leaf peak) should rise above 0.6
  EXPECT_GT(z(32, 32), 0.6f);

  // Point outside outermost contour should drop below 0.2
  EXPECT_LT(z(2, 32), 0.2f);
}

TEST(ElevationFromContours, RasterContoursEmptyInput)
{
  hmap::Array empty_arr;
  EXPECT_TRUE(is_empty(hmap::elevation_from_contours(empty_arr)));

  // Array with all zeros (no contour)
  hmap::Array zero_arr({32, 32}, 0.f);
  EXPECT_TRUE(is_empty(hmap::elevation_from_contours(zero_arr)));
}

TEST(ElevationFromContours, SmoothstepInterpolation)
{
  glm::ivec2              shape = {64, 64};
  std::vector<hmap::Path> c = {square(0.5f, 0.5f, 0.4f),
                               square(0.5f, 0.5f, 0.2f)};
  std::vector<float>      h = {0.0f, 1.0f};

  hmap::Array z_linear =
      hmap::elevation_from_contours(shape, c, h, nullptr, 0.f, 0, 0.5f, 1.f, false);
  hmap::Array z_smooth =
      hmap::elevation_from_contours(shape, c, h, nullptr, 0.f, 0, 0.5f, 1.f, true);

  ASSERT_EQ(z_smooth.shape, shape);

  // Near the midpoint, both should be close to 0.5 (since smoothstep(0.5) == 0.5)
  // For points in the first quarter (e.g. u = 0.25), smoothstep(0.25) = 0.15625 < 0.25
  // For points in the third quarter (e.g. u = 0.75), smoothstep(0.75) = 0.84375 > 0.75
  // Check between outer contour (x=0.1) and inner contour (x=0.3):
  // i=6 is x~0.1 (h=0), i=19 is x~0.3 (h=1)
  // i=9 is closer to 0: smoothstep elevation should be lower than linear
  // i=16 is closer to 1: smoothstep elevation should be higher than linear
  EXPECT_LT(z_smooth(9, 32), z_linear(9, 32));
  EXPECT_GT(z_smooth(16, 32), z_linear(16, 32));
}
