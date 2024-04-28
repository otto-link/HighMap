/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file io.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-05-08
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <cstdint>

#include "highmap/colormaps.hpp"
#include "highmap/geometry.hpp"
#include "highmap/vector.hpp"

namespace hmap
{

/**
 * @brief Mesh type.
 */
enum MeshType : int
{
  tri_optimized, ///< Triangles with optimized Delaunay triangulation
  tri,           ///< Triangle elements
};

/**
 * @brief Mapping between the enum and plain text.
 */
static std::map<MeshType, std::string> mesh_type_as_string = {
    {tri_optimized, "triangles (optimized)"},
    {tri, "triangles"}};

/**
 * @brief Asset export format (nomemclature of formats supported by assimp)
 */
enum AssetExportFormat : int
{
  _3ds,     ///< Autodesk 3DS (legacy) - *.3ds
  _3mf,     ///< The 3MF-File-Format - *.3mf
  assbin,   ///<   Assimp Binary - *.assbin
  assxml,   ///< Assxml Document - *.assxml
  fbxa,     ///< Autodesk FBX (ascii) - *.fbx
  fbx,      ///< Autodesk FBX (binary) - *.fbx
  collada,  ///< COLLADA - Digital Asset Exchange Schema - *.dae
  x3d,      ///< Extensible 3D - *.x3d
  gltf,     ///< GL Transmission Format - *.gltf
  glb,      ///< GL Transmission Format (binary) - *.glb
  gltf2,    ///< GL Transmission Format v. 2 - *.gltf
  glb2,     ///< GL Transmission Format v. 2 (binary) - *.glb
  ply,      ///< Stanford Polygon Library - *.ply
  plyb,     ///< Stanford Polygon Library (binary) - *.ply
  stp,      ///< Step Files - *.stp
  stl,      ///< Stereolithography - *.stl
  stlb,     ///< Stereolithography (binary) - *.stl
  obj,      ///< Wavefront OBJ format - *.obj
  objnomtl, ///< Wavefront OBJ format without material file - *.obj
  x,        ///< X Files - *.x
};

/**
 * @brief Mapping between the enum and plain text. Plain has 3 components: a
 * human-readable plain text format, the corresponding format id for the assimp
 * library and the corresponding file extension.
 *
 *  https://github.com/assimp/assimp/issues/2481
 */
// clang-format off
static std::map<AssetExportFormat, std::vector<std::string>>
    asset_export_format_as_string = {
        {_3ds, {"Autodesk 3DS (legacy) - *.3ds", "3ds", "3ds"}},
        {_3mf, {"The 3MF-File-Format - *.3mf", "3mf", "3mf"}},
        {assbin, {"Assimp Binary - *.assbin", "assbin", "assbin"}},
        {assxml, {"Assxml Document - *.assxml", "assxml", "assxml"}},
        {fbxa, {"Autodesk FBX (ascii) - *.fbx", "fbxa", "fbx"}},
        {fbx, {"Autodesk FBX (binary) - *.fbx", "fbx", "fbx"}},
        {collada, {"COLLADA - Digital Asset Exchange Schema - *.dae", "collada", "dae"}},
        {x3d, {"Extensible 3D - *.x3d", "x3d", "x3d"}},
        {gltf, {"GL Transmission Format - *.gltf", "gltf", "gltf"}},
        {glb, {"GL Transmission Format (binary) - *.glb", "glb", "glb"}},
        {gltf2, {"GL Transmission Format v. 2 - *.gltf", "gltf2", "gltf"}},
        {glb2, {"GL Transmission Format v. 2 (binary) - *.glb", "glb2", "glb"}},
        {ply, {"Stanford Polygon Library - *.ply", "ply", "ply"}},
        {plyb, {"Stanford Polygon Library (binary) - *.ply", "plyb", "ply"}},
        {stp, {"Step Files - *.stp", "stp", "stp"}},
        {stl, {"Stereolithography - *.stl", "stl", "stl"}},
        {stlb, {"Stereolithography (binary) - *.stl", "stlb", "stl"}},
        {obj, {"Wavefront OBJ format - *.obj", "obj", "obj"}},
        {objnomtl, {"Wavefront OBJ format without material file - *.obj", "objnomtl", "obj"}},
        {x, {"X Files - *.x", "x", "x"}},
};
// clang-format on

/**
 * @brief Convert array element values to a color data (3 channels RGB in [0,
 * 255]).
 *
 * @param array Input array.
 * @param vmin Lower bound for scaling to array [0, 1].
 * @param vmax Upper bound for scaling to array [0, 1]
 * @param cmap Colormap (see {@link cmap}).
 * @param hillshading Activate hillshading.
 * @param reverse Reverse colormap.
 * @return std::vector<uint8_t> Vector containing colors (size : shape[0] *
 * shape[1] * 3 channels for RGB).
 */
std::vector<uint8_t> colorize(Array &array,
                              float  vmin,
                              float  vmax,
                              int    cmap,
                              bool   hillshading,
                              bool   reverse = false);

/**
 * @brief Export array values to a 8 bit grayscale image.
 *
 * @param array Input array.
 * @param step Step for each direction (to skip data).
 * @return std::vector<uint8_t> Output image.
 */
std::vector<uint8_t> colorize_grayscale(const Array &array,
                                        Vec2<int>    step = {1, 1});

/**
 * @brief Export array values to a 8 bit grayscale histogram image.
 *
 * @param array Input array.
 * @param step Step for each direction (to skip data).
 * @return std::vector<uint8_t> Output image.
 */
std::vector<uint8_t> colorize_histogram(const Array &array,
                                        Vec2<int>    step = {1, 1});

/**
 * @brief Export a pair of arrays to a 8 bit colored image.
 * @param array1 Input array.
 * @param array2 Input array.
 * @return Output image.
 *
 * **Example**
 * @include ex_colorize_vec2.cpp
 *
 * **Result**
 * @image html ex_colorize_vec2.png
 */
std::vector<uint8_t> colorize_vec2(const Array &array1, const Array &array2);

void convert_rgb_to_ryb(Array &r,
                        Array &g,
                        Array &b,
                        Array &r_out,
                        Array &y_out,
                        Array &b_out);

void convert_ryb_to_rgb(Array &r,
                        Array &y,
                        Array &b,
                        Array &r_out,
                        Array &g_out,
                        Array &b_out);

/**
 * @brief Export the heightmap to various 3d file formats.
 * @param fname File name.
 * @param array Input array.
 * @param mesh_type Mesh type (see {@link mesh_type}).
 * @param export_format Export format (see {@link AssetExportFormat}).
 * @param elevation_scaling Elevation scaling factor.
 * @param texture_fname Texture file name (not mandatory).
 * @param normal_map_fname Normal file name (not mandatory).
 * @param max_error Max error (only used for optimized Delaunay triangulation).
 * @return Success.
 */
bool export_asset(std::string       fname,
                  const Array      &array,
                  MeshType          mesh_type = MeshType::tri,
                  AssetExportFormat export_format = AssetExportFormat::glb2,
                  float             elevation_scaling = 0.2f,
                  std::string       texture_fname = "",
                  std::string       normal_map_fname = "",
                  float             max_error = 5e-4f);

/**
 * @brief Export a set of arrays as banner png image file.
 *
 * @param fname File name.
 * @param arrays Arrays.
 * @param cmap Colormap.
 * @param hillshading Activate hillshading.
 */
void export_banner_png(std::string        fname,
                       std::vector<Array> arrays,
                       int                cmap,
                       bool               hillshading = false);

/**
 * @brief Export a 'bird view' (top view) of the heightmap as a 16 bit png file.
 *
 * @param fname File name.
 * @param array Input array.
 */
void export_birdview_png_16bit(std::string  fname,
                               const Array &array,
                               const float  gamma = 1.f);

/**
 * @brief Export the hillshade map to a 8 bit png file.
 *
 * @param fname File name.
 * @param array Input array.
 */
void export_hillshade_png_8bit(std::string fname, const Array &array);

/**
 * @brief Export the hillshade map to a 16 bit png file.
 *
 * @param fname File name.
 * @param array Input array.
 */
void export_hillshade_png_16bit(std::string fname, const Array &array);

/**
 * @brief Export the heightmap normal map to a 8 bit png file.
 *
 * @param fname File name.
 * @param array Input array.
 *
 * **Example**
 * @include ex_export_normal_map.cpp
 */
void export_normal_map_png_8bit(std::string fname, const Array &array);

/**
 * @brief Export the heightmap normal map to a 16 bit png file.
 *
 * @param fname File name.
 * @param array Input array.
 *
 * **Example**
 * @include ex_export_normal_map.cpp
 */
void export_normal_map_png_16bit(std::string fname, const Array &array);

/**
 * @brief Export 4 arrays as a RGBA png splatmap.
 *
 * @param fname File name.
 * @param p_r Reference to array for channel R.
 * @param p_g Reference to array for channel G.
 * @param p_b Reference to array for channel B.
 * @param p_a Reference to array for channel A.
 */
void export_splatmap_png_16bit(std::string fname,
                               Array      *p_r,
                               Array      *p_g = nullptr,
                               Array      *p_b = nullptr,
                               Array      *p_a = nullptr);

/**
 * @brief Export a pair of heightmaps as a glyph vector field representation to
 * a 8 bit png file.
 *
 * @param fname File name.
 * @param array_u Vector component.
 * @param array_v Vector component.
 * @param density Glyph density.
 * @param scale Glyph scale.
 * @param seed Random seed number.
 *
 * **Example**
 * @include ex_export_vector_glyph.cpp
 *
 * **Result**
 * @image html ex_export_vector_glyph.png
 */
void export_vector_glyph_png_8bit(const std::string fname,
                                  const Array      &array_u,
                                  const Array      &array_v,
                                  const float       density = 0.05f,
                                  const float       scale = 0.05f,
                                  const uint        seed = 0);

/**
 * @brief Export a pair of heightmaps as a glyph vector field representation to
 * a 16 bit png file.
 *
 * @param fname File name.
 * @param array_u Vector component.
 * @param array_v Vector component.
 * @param density Glyph density.
 * @param scale Glyph scale.
 * @param seed Random seed number.
 *
 * **Example**
 * @include ex_export_vector_glyph.cpp
 *
 * **Result**
 * @image html ex_export_vector_glyph.png
 */
void export_vector_glyph_png_16bit(const std::string fname,
                                   const Array      &array_u,
                                   const Array      &array_v,
                                   const float       density = 0.05f,
                                   const float       scale = 0.05f,
                                   const uint        seed = 0);

/**
 * @brief Read an 8bit grayscale image to a png file.
 *
 * @param fname File name.
 * @param shape Image shape.
 */
std::vector<uint8_t> read_png_grayscale_8bit(std::string fname);

/**
 * @brief Read an 16bit grayscale image to a png file.
 *
 * @param fname File name.
 * @param shape Image shape.
 */
std::vector<uint16_t> read_png_grayscale_16bit(std::string fname);

/**
 * @brief
 *
 * @param fname File name.
 * @param width Image width.
 * @param height Image width.
 * @param color_type Color type.
 * @param bit_depth Bit depth.
 */
void read_png_header(std::string fname,
                     int        &width,
                     int        &height,
                     png_byte   &color_type,
                     png_byte   &bit_depth);

/**
 * @brief Export an 8bit grayscale image to a png file.
 *
 * @param fname File name.
 * @param img Image data.
 * @param shape Image shape.
 */
void write_png_grayscale_8bit(std::string           fname,
                              std::vector<uint8_t> &img,
                              Vec2<int>             shape);

/**
 * @brief Export an 16bit grayscale image to a png file.
 *
 * @param fname File name.
 * @param img Image data.
 * @param shape Image shape.
 */
void write_png_grayscale_16bit(std::string            fname,
                               std::vector<uint16_t> &img,
                               Vec2<int>              shape);

/**
 * @brief Export an 8bit RGB image to a png file.
 *
 * @param fname File name.
 * @param img Image data.
 * @param shape Image shape.
 */
void write_png_rgb_8bit(std::string           fname,
                        std::vector<uint8_t> &img,
                        Vec2<int>             shape);

/**
 * @brief Export an 16bit RGB image to a png file.
 *
 * @param fname File name.
 * @param img Image data.
 * @param shape Image shape.
 */
void write_png_rgb_16bit(std::string            fname,
                         std::vector<uint16_t> &img,
                         Vec2<int>              shape);

/**
 * @brief Export an 8bit RGBA image to a png file.
 *
 * @param fname File name.
 * @param img Image data.
 * @param shape Image shape.
 */
void write_png_rgba_8bit(std::string           fname,
                         std::vector<uint8_t> &img,
                         Vec2<int>             shape);

/**
 * @brief Export an 16bit RGBA image to a png file.
 *
 * @param fname File name.
 * @param img Image data.
 * @param shape Image shape.
 */
void write_png_rgba_16bit(std::string            fname,
                          std::vector<uint16_t> &img,
                          Vec2<int>              shape);

/**
 * @brief Export an array to a 16bit 'raw' file (Unity import terrain format).
 *
 * @param fname Filename.
 * @param array Input array.
 */
void write_raw_16bit(std::string fname, const Array &array);

} // namespace hmap
