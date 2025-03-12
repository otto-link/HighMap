/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cstdint>
#include <memory>

#include "hmm/src/heightmap.h"
#include "hmm/src/triangulator.h"
#include "macrologger.h"
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>

#include "highmap/array.hpp"
#include "highmap/export.hpp"
#include "highmap/interpolate2d.hpp"
#include "highmap/operator.hpp"

namespace hmap
{

std::string extract_raw_filename(std::string fname)
{
  size_t lastindex = fname.find_last_of(".");
  return fname.substr(0, lastindex);
}

bool export_asset(const std::string &fname,
                  const Array       &array,
                  MeshType           mesh_type,
                  AssetExportFormat  export_format,
                  float              elevation_scaling,
                  const std::string &texture_fname,
                  const std::string &normal_map_fname,
                  float              max_error)
{

  LOG_DEBUG("exporting asset, format [%s] aka [%s]",
            asset_export_format_as_string.at(export_format)[0].c_str(),
            asset_export_format_as_string.at(export_format)[1].c_str());

  aiScene *p_scene = new aiScene();
  aiMesh  *p_mesh = new aiMesh();

  // --- generate mesh

  std::vector<float> x = linspace(1.f, 0.f, array.shape.x);
  std::vector<float> y = linspace(0.f, 1.f, array.shape.y);

  aiMaterial *p_material = new aiMaterial();
  aiNode     *p_root = new aiNode();
  p_root->mNumMeshes = 1;
  p_root->mMeshes = new unsigned[]{0};

  p_scene->mNumMeshes = 1;
  p_scene->mMeshes = new aiMesh *[] { p_mesh };
  p_scene->mNumMaterials = 1;
  p_scene->mMaterials = new aiMaterial *[] { p_material };
  p_scene->mRootNode = p_root;
  p_scene->mMetaData = new aiMetadata();

  if (texture_fname != "")
  {
    aiString fname = aiString(texture_fname);
    p_material->AddProperty(&fname, AI_MATKEY_TEXTURE_DIFFUSE(0));
  }

  if (normal_map_fname != "")
  {
    aiString fname = aiString(normal_map_fname);
    p_material->AddProperty(&fname, AI_MATKEY_TEXTURE_NORMALS(0));
  }

  switch (mesh_type)
  {

  case hmap::MeshType::TRI:
  {

    // vertices (and texture coordinates)
    uint n_vertices = array.size();
    uint n_faces = 2 * (array.shape.x - 1) * (array.shape.y - 1);

    p_mesh->mVertices = new aiVector3D[n_vertices];
    p_mesh->mNumVertices = n_vertices;

    p_mesh->mTextureCoords[0] = new aiVector3D[n_vertices];
    p_mesh->mNumUVComponents[0] = 2;

    for (int j = 0; j < array.shape.y; j++)
      for (int i = 0; i < array.shape.x; i++)
      {
        int k = array.linear_index(i, j);
        p_mesh->mVertices[k] = aiVector3D(x[i],
                                          elevation_scaling * array(i, j),
                                          y[j]);
        p_mesh->mTextureCoords[0][k] = aiVector3D(1.f - x[i], y[j], 0.f);
      }

    // faces
    p_mesh->mNumFaces = n_faces;
    p_mesh->mFaces = new aiFace[n_faces];
    p_mesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;

    int k = 0;

    for (int j = 0; j < array.shape.y - 1; j++)
      for (int i = 0; i < array.shape.x - 1; i++)
      {
        p_mesh->mFaces[k].mNumIndices = 3;
        p_mesh->mFaces[k].mIndices = new unsigned[]{
            (uint)array.linear_index(i, j),
            (uint)array.linear_index(i + 1, j),
            (uint)array.linear_index(i, j + 1)};
        k++;

        p_mesh->mFaces[k].mNumIndices = 3;
        p_mesh->mFaces[k].mIndices = new unsigned[]{
            (uint)array.linear_index(i + 1, j),
            (uint)array.linear_index(i + 1, j + 1),
            (uint)array.linear_index(i, j + 1)};
        k++;
      }
  }
  break;

  case hmap::MeshType::TRI_OPTIMIZED:
  {
    // mesh retopology (lots of switch between 'x' and 'y' to deal
    // with row-major vs column-major mode for vector data)
    const auto   p_hmap = std::make_shared<Heightmap>(array.shape.y,
                                                    array.shape.x,
                                                    array.get_vector());
    Triangulator tri(p_hmap);
    const int    max_triangles = 0;
    const int    max_points = 0;

    LOG_DEBUG("remeshing (Delaunay)");
    tri.Run(max_error, max_triangles, max_points);

    auto points = tri.Points(elevation_scaling);
    auto triangles = tri.Triangles();

    LOG_DEBUG("points: %ld (max: %d)", points.size(), array.size());
    LOG_DEBUG("triangles: %ld", triangles.size());

    // x, y normalization coefficients
    float ax = 1.f / (float)array.shape.y;
    float ay = 1.f / (float)array.shape.x;

    // vertices (and texture coordinates)
    uint n_vertices = points.size();
    uint n_faces = triangles.size();

    p_mesh->mVertices = new aiVector3D[n_vertices];
    p_mesh->mNumVertices = n_vertices;

    p_mesh->mTextureCoords[0] = new aiVector3D[n_vertices];
    p_mesh->mNumUVComponents[0] = 2;

    for (size_t k = 0; k < points.size(); k++)
    {
      p_mesh->mVertices[k] = aiVector3D(ay * points[k].y,
                                        points[k].z,
                                        ax * points[k].x);
      p_mesh->mTextureCoords[0][k] = aiVector3D(1.f - ay * points[k].y,
                                                ax * points[k].x,
                                                0.f);
    }

    // faces
    p_mesh->mNumFaces = n_faces;
    p_mesh->mFaces = new aiFace[n_faces];
    p_mesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;

    for (size_t k = 0; k < triangles.size(); k++)
    {
      p_mesh->mFaces[k].mNumIndices = 3;
      p_mesh->mFaces[k].mIndices = new unsigned[]{(uint)triangles[k].x,
                                                  (uint)triangles[k].y,
                                                  (uint)triangles[k].z};
    }
  }
  break;

  default: LOG_ERROR("unknown mesh type");
  }

  // --- export

  Assimp::Exporter exporter;

  std::string format_id = asset_export_format_as_string.at(export_format)[1];
  std::string format_extension = "." + asset_export_format_as_string.at(
                                           export_format)[2];

  aiReturn err = exporter.Export(p_scene,
                                 format_id,
                                 extract_raw_filename(fname) + format_extension,
                                 0);

  delete p_scene;

  if (err == aiReturn_SUCCESS)
    return true;
  else
  {
    LOG_ERROR("failed to export asset");
    return false;
  }
}

} // namespace hmap
