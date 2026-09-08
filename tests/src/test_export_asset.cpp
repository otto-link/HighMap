#include <cstdio>
#include <filesystem>

#include "highmap.hpp"

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <gtest/gtest.h>

using namespace hmap;

// Regression: the TRI_OPTIMIZED (Delaunay) mesh path wrote texture
// coordinates into an unallocated aiMesh::mTextureCoords[0] -> SIGSEGV.
TEST(ExportAsset, TriOptimizedExportsWithoutCrash)
{
  glm::ivec2 shape = {64, 64};
  Array      z = noise(NoiseType::SIMPLEX2, shape, {2.f, 2.f}, 1);
  remap(z);

  std::filesystem::path fname = std::filesystem::temp_directory_path() /
                                "hmap_test_tri_optimized";

  // export_asset appends the format extension itself
  std::filesystem::path out = fname.string() + ".glb";

  bool ok = export_asset(fname.string(),
                         z,
                         MeshType::TRI_OPTIMIZED,
                         AssetExportFormat::GLB2,
                         0.2f,
                         "",
                         "",
                         1e-2f);

  EXPECT_TRUE(ok);
  EXPECT_TRUE(std::filesystem::exists(out));
  std::filesystem::remove(out);
}

TEST(ExportAsset, TriExportsWithoutCrash)
{
  glm::ivec2 shape = {64, 64};
  Array      z = noise(NoiseType::SIMPLEX2, shape, {2.f, 2.f}, 1);
  remap(z);

  std::filesystem::path fname = std::filesystem::temp_directory_path() /
                                "hmap_test_tri";

  // export_asset appends the format extension itself
  std::filesystem::path out = fname.string() + ".glb";

  bool ok = export_asset(fname.string(),
                         z,
                         MeshType::TRI,
                         AssetExportFormat::GLB2,
                         0.2f,
                         "",
                         "",
                         1e-2f);

  EXPECT_TRUE(ok);
  EXPECT_TRUE(std::filesystem::exists(out));
  std::filesystem::remove(out);
}

TEST(ExportAsset, TriFitBoundariesCoordinates)
{
  glm::ivec2 shape = {64, 64};
  Array      z(shape, 0.5f);

  std::filesystem::path fname_fit = std::filesystem::temp_directory_path() /
                                    "hmap_test_fit";
  std::filesystem::path out_fit = fname_fit.string() + ".glb";

  bool ok_fit = export_asset(fname_fit.string(),
                             z,
                             MeshType::TRI,
                             AssetExportFormat::GLB2,
                             0.2f,
                             "",
                             "",
                             1e-2f,
                             true);
  EXPECT_TRUE(ok_fit);
  EXPECT_TRUE(std::filesystem::exists(out_fit));

  std::filesystem::path fname_nofit = std::filesystem::temp_directory_path() /
                                      "hmap_test_nofit";
  std::filesystem::path out_nofit = fname_nofit.string() + ".glb";

  bool ok_nofit = export_asset(fname_nofit.string(),
                               z,
                               MeshType::TRI,
                               AssetExportFormat::GLB2,
                               0.2f,
                               "",
                               "",
                               1e-2f,
                               false);
  EXPECT_TRUE(ok_nofit);
  EXPECT_TRUE(std::filesystem::exists(out_nofit));

  Assimp::Importer importer;
  const aiScene   *scene_fit = importer.ReadFile(out_fit.string(), 0);
  ASSERT_NE(scene_fit, nullptr);
  ASSERT_GT(scene_fit->mNumMeshes, 0u);
  aiMesh *mesh_fit = scene_fit->mMeshes[0];

  float max_x_fit = 0.f;
  float max_z_fit = 0.f;
  for (unsigned int i = 0; i < mesh_fit->mNumVertices; ++i)
  {
    max_x_fit = std::max(max_x_fit, mesh_fit->mVertices[i].x);
    max_z_fit = std::max(max_z_fit, mesh_fit->mVertices[i].z);
  }
  EXPECT_NEAR(max_x_fit, 1.0f, 1e-5f);
  EXPECT_NEAR(max_z_fit, 1.0f, 1e-5f);

  const aiScene *scene_nofit = importer.ReadFile(out_nofit.string(), 0);
  ASSERT_NE(scene_nofit, nullptr);
  ASSERT_GT(scene_nofit->mNumMeshes, 0u);
  aiMesh *mesh_nofit = scene_nofit->mMeshes[0];

  float max_x_nofit = 0.f;
  float max_z_nofit = 0.f;
  for (unsigned int i = 0; i < mesh_nofit->mNumVertices; ++i)
  {
    max_x_nofit = std::max(max_x_nofit, mesh_nofit->mVertices[i].x);
    max_z_nofit = std::max(max_z_nofit, mesh_nofit->mVertices[i].z);
  }
  EXPECT_NEAR(max_x_nofit, 1.0f - 1.0f / shape.y, 1e-5f);
  EXPECT_NEAR(max_z_nofit, 1.0f - 1.0f / shape.x, 1e-5f);

  std::filesystem::remove(out_fit);
  std::filesystem::remove(out_nofit);
}

TEST(ExportAsset, TriOptimizedFitBoundariesCoordinates)
{
  glm::ivec2 shape = {64, 64};
  Array      z = noise(NoiseType::SIMPLEX2, shape, {2.f, 2.f}, 1);
  remap(z);

  std::filesystem::path fname_fit = std::filesystem::temp_directory_path() /
                                    "hmap_test_opt_fit";
  std::filesystem::path out_fit = fname_fit.string() + ".glb";

  bool ok_fit = export_asset(fname_fit.string(),
                             z,
                             MeshType::TRI_OPTIMIZED,
                             AssetExportFormat::GLB2,
                             0.2f,
                             "",
                             "",
                             1e-2f,
                             true);
  EXPECT_TRUE(ok_fit);
  EXPECT_TRUE(std::filesystem::exists(out_fit));

  std::filesystem::path fname_nofit = std::filesystem::temp_directory_path() /
                                      "hmap_test_opt_nofit";
  std::filesystem::path out_nofit = fname_nofit.string() + ".glb";

  bool ok_nofit = export_asset(fname_nofit.string(),
                               z,
                               MeshType::TRI_OPTIMIZED,
                               AssetExportFormat::GLB2,
                               0.2f,
                               "",
                               "",
                               1e-2f,
                               false);
  EXPECT_TRUE(ok_nofit);
  EXPECT_TRUE(std::filesystem::exists(out_nofit));

  Assimp::Importer importer;
  const aiScene   *scene_fit = importer.ReadFile(out_fit.string(), 0);
  ASSERT_NE(scene_fit, nullptr);
  ASSERT_GT(scene_fit->mNumMeshes, 0u);
  aiMesh *mesh_fit = scene_fit->mMeshes[0];

  float max_x_fit = 0.f;
  float max_z_fit = 0.f;
  for (unsigned int i = 0; i < mesh_fit->mNumVertices; ++i)
  {
    max_x_fit = std::max(max_x_fit, mesh_fit->mVertices[i].x);
    max_z_fit = std::max(max_z_fit, mesh_fit->mVertices[i].z);
  }
  EXPECT_NEAR(max_x_fit, 1.0f, 1e-5f);
  EXPECT_NEAR(max_z_fit, 1.0f, 1e-5f);

  const aiScene *scene_nofit = importer.ReadFile(out_nofit.string(), 0);
  ASSERT_NE(scene_nofit, nullptr);
  ASSERT_GT(scene_nofit->mNumMeshes, 0u);
  aiMesh *mesh_nofit = scene_nofit->mMeshes[0];

  float max_x_nofit = 0.f;
  float max_z_nofit = 0.f;
  for (unsigned int i = 0; i < mesh_nofit->mNumVertices; ++i)
  {
    max_x_nofit = std::max(max_x_nofit, mesh_nofit->mVertices[i].x);
    max_z_nofit = std::max(max_z_nofit, mesh_nofit->mVertices[i].z);
  }
  EXPECT_NEAR(max_x_nofit, 1.0f - 1.0f / shape.x, 1e-5f);
  EXPECT_NEAR(max_z_nofit, 1.0f - 1.0f / shape.y, 1e-5f);

  std::filesystem::remove(out_fit);
  std::filesystem::remove(out_nofit);
}

TEST(ExportAsset, MaskedFitBoundariesCoordinates)
{
  glm::ivec2 shape = {64, 64};
  Array      z(shape, 0.5f);
  Array      mask(shape, 1.0f);

  std::filesystem::path fname_fit = std::filesystem::temp_directory_path() /
                                    "hmap_test_mask_fit";
  std::filesystem::path out_fit = fname_fit.string() + ".glb";

  bool ok_fit = export_asset(fname_fit.string(),
                             z,
                             mask,
                             AssetExportFormat::GLB2,
                             0.2f,
                             "",
                             "",
                             true);
  EXPECT_TRUE(ok_fit);
  EXPECT_TRUE(std::filesystem::exists(out_fit));

  std::filesystem::path fname_nofit = std::filesystem::temp_directory_path() /
                                      "hmap_test_mask_nofit";
  std::filesystem::path out_nofit = fname_nofit.string() + ".glb";

  bool ok_nofit = export_asset(fname_nofit.string(),
                               z,
                               mask,
                               AssetExportFormat::GLB2,
                               0.2f,
                               "",
                               "",
                               false);
  EXPECT_TRUE(ok_nofit);
  EXPECT_TRUE(std::filesystem::exists(out_nofit));

  Assimp::Importer importer;
  const aiScene   *scene_fit = importer.ReadFile(out_fit.string(), 0);
  ASSERT_NE(scene_fit, nullptr);
  ASSERT_GT(scene_fit->mNumMeshes, 0u);
  aiMesh *mesh_fit = scene_fit->mMeshes[0];

  float max_x_fit = 0.f;
  float max_z_fit = 0.f;
  for (unsigned int i = 0; i < mesh_fit->mNumVertices; ++i)
  {
    max_x_fit = std::max(max_x_fit, mesh_fit->mVertices[i].x);
    max_z_fit = std::max(max_z_fit, mesh_fit->mVertices[i].z);
  }
  EXPECT_NEAR(max_x_fit, 1.0f, 1e-5f);
  EXPECT_NEAR(max_z_fit, 1.0f, 1e-5f);

  const aiScene *scene_nofit = importer.ReadFile(out_nofit.string(), 0);
  ASSERT_NE(scene_nofit, nullptr);
  ASSERT_GT(scene_nofit->mNumMeshes, 0u);
  aiMesh *mesh_nofit = scene_nofit->mMeshes[0];

  float max_x_nofit = 0.f;
  float max_z_nofit = 0.f;
  for (unsigned int i = 0; i < mesh_nofit->mNumVertices; ++i)
  {
    max_x_nofit = std::max(max_x_nofit, mesh_nofit->mVertices[i].x);
    max_z_nofit = std::max(max_z_nofit, mesh_nofit->mVertices[i].z);
  }
  EXPECT_NEAR(max_x_nofit, 1.0f - 1.0f / shape.y, 1e-5f);
  EXPECT_NEAR(max_z_nofit, 1.0f - 1.0f / shape.x, 1e-5f);

  std::filesystem::remove(out_fit);
  std::filesystem::remove(out_nofit);
}
