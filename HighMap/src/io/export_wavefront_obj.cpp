/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cstdint>

#include "WavefrontObjWriter/obj.h"
#include "hmm/src/heightmap.h"
#include "hmm/src/triangulator.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/interpolate.hpp"
#include "highmap/io.hpp"
#include "highmap/noise_function.hpp"
#include "highmap/op.hpp"

namespace hmap
{

std::string extract_raw_filename(std::string fname)
{
  size_t lastindex = fname.find_last_of(".");
  return fname.substr(0, lastindex);
}

void export_wavefront_obj(std::string  fname,
                          const Array &array,
                          MeshType     mesh_type,
                          float        elevation_scaling,
                          std::string  texture_fname,
                          float        max_error)
{
  wow::Obj obj;
  obj.enableTextureCoordinates();

  // mesh fname without extension
  std::string raw_fname = extract_raw_filename(fname);

  // add a material file pointing to the texture file name
  if (texture_fname != "")
  {
    std::fstream f;
    f.open(raw_fname + ".mtl", std::ios::out);
    f << "newmtl hesiod_material" << std::endl;
    f << "Ka 1.000000 1.000000 1.000000" << std::endl;
    f << "Kd 0.640000 0.640000 0.640000" << std::endl;
    f << "Ks 0.500000 0.500000 0.500000" << std::endl;
    f << "Ns 96.078431" << std::endl;
    f << "Ni 1.000000" << std::endl;
    f << "d 1.000000" << std::endl;
    f << "illum 0" << std::endl;
    f << "map_Kd " << texture_fname << std::endl;
    f.close();
  }

  std::vector<float> x = linspace(1.f, 0.f, array.shape.x);
  std::vector<float> y = linspace(0.f, 1.f, array.shape.y);

  switch (mesh_type)
  {

  case hmap::MeshType::quad:
  {
    LOG_DEBUG("exporting OBJ (quad)");

    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        wow::Vertex v = wow::Vertex(x[i],
                                    elevation_scaling * array(i, j),
                                    y[j]);
        v.setTexCoord(1.f - x[i], y[j]);
        obj.appendVertex(v);
      }

    for (int i = 0; i < array.shape.x - 1; i++)
      for (int j = 0; j < array.shape.y - 1; j++)
        obj.appendFace(wow::Face(array.linear_index(i, j),
                                 array.linear_index(i + 1, j),
                                 array.linear_index(i + 1, j + 1),
                                 array.linear_index(i, j + 1)));
  }
  break;

  case hmap::MeshType::tri:
  {
    LOG_DEBUG("exporting OBJ (tri)");

    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        wow::Vertex v = wow::Vertex(x[i],
                                    elevation_scaling * array(i, j),
                                    y[j]);
        v.setTexCoord(1.f - x[i], y[j]);
        obj.appendVertex(v);
      }

    for (int i = 0; i < array.shape.x - 1; i++)
      for (int j = 0; j < array.shape.y - 1; j++)
      {
        obj.appendFace(wow::Face(array.linear_index(i, j),
                                 array.linear_index(i + 1, j),
                                 array.linear_index(i, j + 1)));

        obj.appendFace(wow::Face(array.linear_index(i + 1, j),
                                 array.linear_index(i + 1, j + 1),
                                 array.linear_index(i, j + 1)));
      }
  }
  break;

  case hmap::MeshType::tri_optimized:
  {
    LOG_DEBUG("exporting OBJ (tri optimized)");

    // lots of switch between 'x' and 'y' to deal with row-major vs
    // column-major mode for vector data
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

    // export
    for (size_t k = 0; k < points.size(); k++)
    {
      wow::Vertex v = wow::Vertex(ay * points[k].y,
                                  points[k].z,
                                  ax * points[k].x);
      v.setTexCoord(1.f - ay * points[k].y, ax * points[k].x);
      obj.appendVertex(v);
    }

    for (size_t k = 0; k < triangles.size(); k++)
      obj.appendFace(wow::Face(triangles[k].x, triangles[k].y, triangles[k].z));
  }
  break;
  }

  obj.output(raw_fname);

  // add pointer to material file in obj file
  if (texture_fname != "")
  {
    std::fstream f;
    f.open(fname, std::ios::app);
    f << "mtllib " + raw_fname + ".mtl" << std::endl;
    f << "usemtl hesiod_material" << std::endl;
    f.close();
  }
}

void export_wavefront_obj(std::string  fname,
                          const Array &array,
                          int          ir,
                          float        elevation_scaling,
                          std::string  texture_fname,
                          float        max_error)
{
  wow::Obj obj;
  obj.enableTextureCoordinates();
  obj.enableNormal();

  // mesh fname without extension
  std::string raw_fname = extract_raw_filename(fname);

  // add a material file
  {
    std::fstream f;
    f.open(raw_fname + ".mtl", std::ios::out);
    f << "newmtl hesiod_material" << std::endl;
    f << "Ka 1.000000 1.000000 1.000000" << std::endl;
    f << "Kd 0.640000 0.640000 0.640000" << std::endl;
    f << "Ks 0.500000 0.500000 0.500000" << std::endl;
    f << "Ns 96.078431" << std::endl;
    f << "Ni 1.000000" << std::endl;
    f << "d 1.000000" << std::endl;
    f << "illum 0" << std::endl;
    f << "map_Kd " << texture_fname << std::endl;
    if (texture_fname != "")
      f << "map_bump " << raw_fname + "_nmap.png" << std::endl;
    f.close();
  }

  std::vector<float> x = linspace(1.f, 0.f, array.shape.x);
  std::vector<float> y = linspace(0.f, 1.f, array.shape.y);

  LOG_DEBUG("exporting OBJ (tri optimized)");

  // lots of switch between 'x' and 'y' to deal with row-major vs
  // column-major mode for vector data
  Array array_f = array;
  if (ir > 0)
    smooth_cone(array_f, ir);

  const auto   p_hmap = std::make_shared<Heightmap>(array_f.shape.y,
                                                  array_f.shape.x,
                                                  array_f.get_vector());
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

  // generate sampling functions for the normal vector
  Array nx = Array(array.shape);
  Array ny = Array(array.shape);
  Array nz = Array(array.shape);

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      hmap::Vec3<float> vec_norm = array_f.get_normal_at(i, j);
      nx(i, j) = vec_norm.x;
      ny(i, j) = vec_norm.y;
      nz(i, j) = vec_norm.z;
    }

  ArrayFunction fnx = ArrayFunction(nx, {1.f, 1.f}, false);
  ArrayFunction fny = ArrayFunction(ny, {1.f, 1.f}, false);
  ArrayFunction fnz = ArrayFunction(nz, {1.f, 1.f}, false);

  // export the normal of the difference between the original array
  // and the array based on the triangulated mesh
  export_normal_map_png_16bit(raw_fname + "_nmap.png", array - array_f);

  hmap::export_banner_png("diff.png", {array, array_f}, hmap::cmap::jet);

  // --- export

  for (size_t k = 0; k < points.size(); k++)
  {
    wow::Vertex v = wow::Vertex(ay * points[k].y,
                                points[k].z,
                                ax * points[k].x);
    v.setTexCoord(1.f - ay * points[k].y, ax * points[k].x);
    v.setNormal(
        fny.get_function()(1.f - ay * points[k].y, ax * points[k].x, 0.f),
        fnz.get_function()(1.f - ay * points[k].y, ax * points[k].x, 0.f),
        fnx.get_function()(1.f - ay * points[k].y, ax * points[k].x, 0.f));

    obj.appendVertex(v);
  }

  for (size_t k = 0; k < triangles.size(); k++)
    obj.appendFace(wow::Face(triangles[k].x, triangles[k].y, triangles[k].z));

  obj.output(raw_fname);

  // add pointer to material file
  {
    std::fstream f;
    f.open(fname, std::ios::app);
    f << "mtllib " + raw_fname + ".mtl" << std::endl;
    f << "usemtl hesiod_material" << std::endl;
    f.close();
  }
}

void export_wavefront_obj(std::string fname,
                          const Path &path,
                          float       elevation_scaling)
{
  LOG_DEBUG("exporting OBJ (lines for path)");

  wow::LineObj obj;

  for (auto &p : path.points)
    obj.appendVertex(p.x, elevation_scaling * p.v, p.y);

  for (int k = 0; k < (int)path.points.size() - 1; k++)
    obj.appendLine(wow::Line(k, k + 1));

  obj.output(extract_raw_filename(fname));
}

} // namespace hmap
