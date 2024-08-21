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
#include <map>
#include <opencv2/imgcodecs.hpp>
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
 * @brief Export the heightmap normal map to a 8 bit png file.
 *
 * @param fname File name.
 * @param array Input array.
 *
 * **Example**
 * @include ex_export_normal_map.cpp
 */
void export_normal_map_png(std::string  fname,
                           const Array &array,
                           int          depth = CV_8U);

/**
 * @brief Export 4 arrays as a RGBA png splatmap.
 *
 * @param fname File name.
 * @param p_r Reference to array for channel R.
 * @param p_g Reference to array for channel G.
 * @param p_b Reference to array for channel B.
 * @param p_a Reference to array for channel A.
 *
 * **Example**
 * @include ex_export_splatmap_png_16bit.cpp
 *
 * **Result**
 * @image html ex_export_splatmap_png_16bit0.png
 * @image html ex_export_splatmap_png_16bit1.png
 */
void export_splatmap_png(std::string fname,
                         Array      *p_r,
                         Array      *p_g = nullptr,
                         Array      *p_b = nullptr,
                         Array      *p_a = nullptr,
                         int         depth = CV_8U);

/**
 * @brief Reads an image file and converts it to a 2D array.
 *
 * This function uses the OpenCV `imread` function to load an image from the
 * specified file. The supported file formats are those recognized by OpenCV's
 * `imread` function, such as JPEG, PNG, BMP, and others. If the image is in
 * color, it is automatically converted to grayscale using the built-in OpenCV
 * codec converter. This conversion process may introduce artifacts depending on
 * the image's original format and content.
 *
 * @param fname The name of the image file to be read.
 * @return Array A 2D array containing the pixel values of the grayscale image.
 */
Array read_to_array(std::string fname);

/**
 * @brief Export an array to a 16bit 'raw' file (Unity import terrain format).
 *
 * @param fname Filename.
 * @param array Input array.
 */
void write_raw_16bit(std::string fname, const Array &array);

} // namespace hmap
