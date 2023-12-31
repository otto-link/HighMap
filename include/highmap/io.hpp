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
#include "highmap/vector.hpp"

namespace hmap
{

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

// /**
//  * @brief Convert 3 array element values to a color data (3 channels RGB in
//  [0,
//  * 255]) use a multivariate colormap.
//  *
//  * @param c0 First array.
//  * @param c1 Second array.
//  * @param c2 Third array.
//  * @param clut Colormap Lookup Table object.
//  * @param hillshading Activate hillshading.
//  * @return std::vector<uint8_t> Vector containing colors (size : shape[0] *
//  * shape[1] * 3 channels for RGB).
//  */
// std::vector<uint8_t> colorize_trivariate(const Array &c0,
//                                          const Array &c1,
//                                          const Array &c2,
//                                          Clut3D      &clut,
//                                          bool         hillshading);

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
