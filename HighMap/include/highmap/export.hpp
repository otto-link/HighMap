/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file export.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file for asset export functionalities.
 *
 * This header file declares functions and types related to exporting assets in
 * various formats. It includes functionality for exporting heightmaps,
 * generating 3D assets in different file formats, and handling export options
 * such as mesh types, file formats, and scaling factors.
 *
 * @version 0.1
 * @date 2023-05-08
 *
 * @copyright Copyright (c) 2023 Otto Link
 */
#pragma once
#include <map>
#include <opencv2/imgcodecs.hpp>
#include <png.h>

#include "highmap/array.hpp"
#include "highmap/tensor.hpp"

namespace hmap
{

enum Cmap : int; // highmap/colormap.hpp

/**
 * @brief Enumeration for different mesh types.
 *
 * This enum defines the various types of mesh representations available. Each
 * type corresponds to a different way of constructing and representing mesh
 * data.
 */
enum MeshType : int
{
  TRI_OPTIMIZED, ///< Triangles with optimized Delaunay triangulation
  TRI,           ///< Triangle elements
};

/**
 * @brief Mapping between `MeshType` enum values and their plain text
 * descriptions.
 *
 * This static map provides a conversion between `MeshType` enum values and
 * their corresponding human-readable descriptions. It is used for displaying or
 * logging the mesh type in a human-friendly format.
 */
static std::map<MeshType, std::string> mesh_type_as_string = {
    {TRI_OPTIMIZED, "triangles (optimized)"},
    {TRI, "triangles"}};

/**
 * @brief Enumeration for asset export formats supported by Assimp.
 *
 * This enum lists the various file formats supported for asset export, as
 * recognized by the Assimp library. Each format is associated with a specific
 * file extension and usage.
 */
enum AssetExportFormat : int
{
  _3DS,    ///< Autodesk 3DS (legacy) - *.3ds
  _3MF,    ///< The 3MF-File-Format - *.3mf
  ASSBIN,  ///< Assimp Binary - *.assbin
  ASSXML,  ///< Assxml Document - *.assxml
  FXBA,    ///< Autodesk FBX (ascii) - *.fbx
  FBX,     ///< Autodesk FBX (binary) - *.fbx
  COLLADA, ///< COLLADA - Digital Asset Exchange Schema - *.dae
  X3D,     ///< Extensible 3D - *.x3d
  GLTF,    ///< GL Transmission Format - *.gltf
  GLB,     ///< GL Transmission Format (binary) - *.glb
  GTLF2,   ///< GL Transmission Format v. 2 - *.gltf
  GLB2,    ///< GL Transmission Format v. 2 (binary) - *.glb
  PLY,     ///< Stanford Polygon Library - *.ply
  PLYB,    ///< Stanford Polygon Library (binary) - *.ply
  STP,     ///< Step Files - *.stp
  STL,     ///< Stereolithography - *.stl
  STLB,    ///< Stereolithography (binary) - *.stl
  OBJ,     ///< Wavefront OBJ format - *.obj
  OBJNOMTL ///< Wavefront OBJ format without material file - *.obj
};

/**
 * @brief Mapping between asset export formats and their plain text
 * representations.
 *
 * This static map provides a mapping between `AssetExportFormat` enumeration
 * values and their corresponding plain text descriptions. Each entry includes a
 * human-readable format description, the format ID used by the Assimp library,
 * and the associated file extension. This mapping is used for converting
 * between enum values and their string representations in various export
 * scenarios.
 *
 * The format is structured as follows:
 * - Human-readable description of the format.
 * - Format ID as recognized by the Assimp library.
 * - File extension commonly used for that format.
 *
 * **Note**: For more details on the Assimp library formats, refer to
 * [Assimp Issue #2481](https://github.com/assimp/assimp/issues/2481).
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
 * @brief Exports a heightmap to various 3D file formats.
 *
 * This function exports the input heightmap array as a 3D asset in the
 * specified format. The export can include different mesh types, elevation
 * scaling, and optional texture and normal maps. The function supports
 * optimized Delaunay triangulation for mesh generation, with a configurable
 * maximum error.
 *
 * @param fname The name of the file to which the 3D asset will be exported.
 * @param array The input heightmap array to be converted into a 3D asset.
 * @param mesh_type The type of mesh to generate (see {@link MeshType}).
 * @param export_format The format in which to export the asset (see {@link
 * AssetExportFormat}).
 * @param elevation_scaling A scaling factor applied to the elevation values of
 * the heightmap. Default is 0.2f.
 * @param texture_fname The name of the texture file to be applied to the asset
 * (optional).
 * @param normal_map_fname The name of the normal map file to be applied to the
 * asset (optional).
 * @param max_error The maximum allowable error for optimized Delaunay
 * triangulation. Default is 5e-4f.
 * @return `true` if the export is successful, `false` otherwise.
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
 * @brief Exports a set of arrays as a banner PNG image file.
 *
 * This function takes a vector of arrays and exports them as a single banner
 * PNG image. The arrays are displayed side by side in the image, using the
 * specified colormap `cmap`. Optionally, hillshading can be applied to
 * enhance the visual representation of the data.
 *
 * @param fname The name of the file to which the banner image will be exported.
 * @param arrays A vector of arrays to be included in the banner image.
 * @param cmap An integer representing the colormap to be applied to the arrays.
 * @param hillshading A boolean flag to activate hillshading for enhanced
 * visual depth. Default is `false`.
 */
void export_banner_png(std::string        fname,
                       std::vector<Array> arrays,
                       int                cmap,
                       bool               hillshading = false);

/**
 * @brief Exports the heightmap normal map as an 8-bit PNG file.
 *
 * This function generates a normal map from the input heightmap array and
 * exports it as an 8-bit PNG image. The normal map can be used in 3D
 * rendering engines to create realistic lighting and shading effects.
 *
 * @param fname The name of the file to which the normal map will be exported.
 * @param array The input heightmap array from which the normal map is derived.
 * @param depth The depth of the PNG image, e.g., `CV_8U` for 8-bit or `CV_16U`
 * for 16-bit. Default is `CV_8U`.
 *
 * **Example**
 * @include ex_export_normal_map.cpp
 */
void export_normal_map_png(std::string  fname,
                           const Array &array,
                           int          depth = CV_8U);

/**
 * @brief Exports four arrays as an RGBA PNG splatmap.
 *
 * This function combines four input arrays, representing the red (R), green
 * (G), blue (B), and alpha (A) channels, into a single RGBA PNG image. The
 * resulting splatmap can be used in applications like terrain texturing. The
 * PNG image is saved to the specified file name `fname`. Channels G, B, and A
 * are optional; if not provided, they will default to zero.
 *
 * @param fname The name of the file to which the RGBA splatmap will be
 * exported.
 * @param p_r Pointer to the array representing the red (R) channel.
 * @param p_g Pointer to the array representing the green (G) channel. Default
 * is `nullptr`.
 * @param p_b Pointer to the array representing the blue (B) channel. Default is
 * `nullptr`.
 * @param p_a Pointer to the array representing the alpha (A) channel. Default
 * is `nullptr`.
 * @param depth The depth of the PNG image, e.g., `CV_8U` for 8-bit or `CV_16U`
 * for 16-bit. Default is `CV_8U`.
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
 * @brief Exports an array to a 16-bit 'raw' file format, commonly used for
 * Unity terrain imports.
 *
 * This function saves the input array to a file in a 16-bit 'raw' format,
 * which is suitable for importing heightmaps into Unity or other applications
 * that support this format. The array values are converted and written to the
 * file specified by `fname`.
 *
 * @param fname The name of the file to which the array will be exported.
 * @param array The input array containing the data to be exported.
 */
void write_raw_16bit(std::string fname, const Array &array);

// helper
Tensor compute_nmap(const Array &array);

} // namespace hmap
