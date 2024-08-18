/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file export.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-05-08
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <png.h>

#include "highmap/array.hpp"

namespace hmap
{

enum Cmap : int; // highmap/colormap.hpp

/**
 * @brief Mesh type.
 */
enum MeshType : int
{
  TRI_OPTIMIZED, ///< Triangles with optimized Delaunay triangulation
  TRI,           ///< Triangle elements
};

/**
 * @brief Mapping between the enum and plain text.
 */
static std::map<MeshType, std::string> mesh_type_as_string = {
    {TRI_OPTIMIZED, "triangles (optimized)"},
    {TRI, "triangles"}};

/**
 * @brief Asset export format (nomemclature of formats supported by assimp)
 */
enum AssetExportFormat : int
{
  _3DS,     ///< Autodesk 3DS (legacy) - *.3ds
  _3MF,     ///< The 3MF-File-Format - *.3mf
  ASSBIN,   ///<   Assimp Binary - *.assbin
  ASSXML,   ///< Assxml Document - *.assxml
  FXBA,     ///< Autodesk FBX (ascii) - *.fbx
  FBX,      ///< Autodesk FBX (binary) - *.fbx
  COLLADA,  ///< COLLADA - Digital Asset Exchange Schema - *.dae
  X3D,      ///< Extensible 3D - *.x3d
  GLTF,     ///< GL Transmission Format - *.gltf
  GLB,      ///< GL Transmission Format (binary) - *.glb
  GTLF2,    ///< GL Transmission Format v. 2 - *.gltf
  GLB2,     ///< GL Transmission Format v. 2 (binary) - *.glb
  PLY,      ///< Stanford Polygon Library - *.ply
  PLYB,     ///< Stanford Polygon Library (binary) - *.ply
  STP,      ///< Step Files - *.stp
  STL,      ///< Stereolithography - *.stl
  STLB,     ///< Stereolithography (binary) - *.stl
  OBJ,      ///< Wavefront OBJ format - *.obj
  OBJNOMTL, ///< Wavefront OBJ format without material file - *.obj
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
        {_3DS, {"Autodesk 3DS (legacy) - *.3ds", "3ds", "3ds"}},
        {_3MF, {"The 3MF-File-Format - *.3mf", "3mf", "3mf"}},
        {ASSBIN, {"Assimp Binary - *.assbin", "assbin", "assbin"}},
        {ASSXML, {"Assxml Document - *.assxml", "assxml", "assxml"}},
        {FXBA, {"Autodesk FBX (ascii) - *.fbx", "fbxa", "fbx"}},
        {FBX, {"Autodesk FBX (binary) - *.fbx", "fbx", "fbx"}},
        {COLLADA, {"COLLADA - Digital Asset Exchange Schema - *.dae", "collada", "dae"}},
        {X3D, {"Extensible 3D - *.x3d", "x3d", "x3d"}},
        {GLTF, {"GL Transmission Format - *.gltf", "gltf", "gltf"}},
        {GLB, {"GL Transmission Format (binary) - *.glb", "glb", "glb"}},
        {GTLF2, {"GL Transmission Format v. 2 - *.gltf", "gltf2", "gltf"}},
        {GLB2, {"GL Transmission Format v. 2 (binary) - *.glb", "glb2", "glb"}},
        {PLY, {"Stanford Polygon Library - *.ply", "ply", "ply"}},
        {PLYB, {"Stanford Polygon Library (binary) - *.ply", "plyb", "ply"}},
        {STP, {"Step Files - *.stp", "stp", "stp"}},
        {STL, {"Stereolithography - *.stl", "stl", "stl"}},
        {STLB, {"Stereolithography (binary) - *.stl", "stlb", "stl"}},
        {OBJ, {"Wavefront OBJ format - *.obj", "obj", "obj"}},
        {OBJNOMTL, {"Wavefront OBJ format without material file - *.obj", "objnomtl", "obj"}},
};
// clang-format on

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
                  MeshType          mesh_type = MeshType::TRI,
                  AssetExportFormat export_format = AssetExportFormat::GLB2,
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
