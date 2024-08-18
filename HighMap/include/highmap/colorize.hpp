/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file colorize.hpp
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

enum cmap : int; // highmap/colormap.hpp

/**
 * @brief Apply hillshading to an image.
 *
 * @param img Input image.
 * @param array Elevation array.
 * @param exponent Power exponent applied to the hillshade values.
 * @param is_img_rgba Whether the input image as an alpha channel or not.
 */
void apply_hillshade(std::vector<uint8_t> &img,
                     const hmap::Array    &array,
                     float                 vmin = 0.f,
                     float                 vmax = 1.f,
                     float                 exponent = 1.f,
                     bool                  is_img_rgba = false);

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

} // namespace hmap