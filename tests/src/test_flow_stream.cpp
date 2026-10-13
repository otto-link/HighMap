#include <cmath>
#include <vector>

#include "highmap.hpp"

#include <gtest/gtest.h>

TEST(FlowStreamMesh, StreamToBoundaryVertex)
{
  // inclined plane from left (high) to right (low)
  std::vector<glm::vec3> pts = {{0.0f, 0.0f, 1.0f},
                                {0.0f, 1.0f, 1.0f},
                                {1.0f, 0.0f, 0.0f},
                                {1.0f, 1.0f, 0.0f},
                                {0.5f, 0.5f, 0.5f}};

  hmap::TerrainTriMesh mesh(pts);

  // start from top-left (vertex 1)
  hmap::Path path = hmap::flow_stream(mesh, size_t(1));

  EXPECT_GE(path.size(), 2u);

  // path must end at a boundary outlet
  auto last_pt = path[path.size() - 1];
  EXPECT_NEAR(last_pt.x, 1.0f, 1e-4f);
  EXPECT_NEAR(last_pt.v, 0.0f, 1e-4f);
}

TEST(FlowStreamMesh, StreamFromCoordinates)
{
  std::vector<glm::vec3> pts = {{0.0f, 0.0f, 1.0f},
                                {0.0f, 1.0f, 1.0f},
                                {1.0f, 0.0f, 0.0f},
                                {1.0f, 1.0f, 0.0f},
                                {0.5f, 0.5f, 0.5f}};

  hmap::TerrainTriMesh mesh(pts);

  // start near (0.05, 0.95)
  hmap::Path path = hmap::flow_stream(mesh, glm::vec2(0.05f, 0.95f));

  EXPECT_GE(path.size(), 2u);

  auto last_pt = path[path.size() - 1];
  EXPECT_NEAR(last_pt.x, 1.0f, 1e-4f);
}

TEST(FlowStreamMesh, StreamFromHeightmapMesh)
{
  glm::ivec2  shape = {64, 64};
  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                  shape,
                                  {2.f, 2.f},
                                  42);
  hmap::remap(z);

  hmap::TerrainTriMesh mesh =
      hmap::generate_terrain_tri_mesh_from_heightmap_random(z, 256, 42);

  hmap::Path path = hmap::flow_stream(mesh, glm::vec2(0.5f, 0.5f));

  EXPECT_GE(path.size(), 2u);

  for (size_t i = 0; i < path.size(); ++i)
  {
    const auto &pt = path[i];
    EXPECT_GE(pt.x, 0.f);
    EXPECT_LE(pt.x, 1.f);
    EXPECT_GE(pt.y, 0.f);
    EXPECT_LE(pt.y, 1.f);
  }
}

TEST(FlowStreamVirtualArray, StreamOnVirtualArray)
{
  glm::ivec2        shape = {64, 64};
  glm::ivec2        tile_shape = {32, 32};
  int               halo = 8;
  hmap::ComputeMode cm;

  hmap::Array z_arr = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                      shape,
                                      {2.f, 2.f},
                                      42);
  hmap::remap(z_arr);

  hmap::VirtualArray z_va(shape, tile_shape, halo, hmap::StorageMode::VA_RAM);
  z_va.from_array(z_arr, cm);

  hmap::Path path = hmap::va::flow_stream(cm,
                                          z_va,
                                          glm::vec2(0.4f, 0.6f),
                                          42,
                                          256);

  EXPECT_GE(path.size(), 2u);

  for (size_t i = 0; i < path.size(); ++i)
  {
    const auto &pt = path[i];
    EXPECT_GE(pt.x, 0.f);
    EXPECT_LE(pt.x, 1.f);
    EXPECT_GE(pt.y, 0.f);
    EXPECT_LE(pt.y, 1.f);
  }
}
