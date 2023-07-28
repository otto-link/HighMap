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
#define IMG_CHANNELS 3

#include "highmap/colormaps.hpp"

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
 * @return std::vector<uint8_t> Vector containing colors (size : shape[0] *
 * shape[1] * 3 channels for RGB).
 */
std::vector<uint8_t> colorize(const Array &array,
                              float        vmin,
                              float        vmax,
                              int          cmap,
                              bool         hillshading);

/**
 * @brief Convert array element values to a grayscale image data (1 channels in
 * [0, 255]).
 *
 * @param array Input array.
 * @param step Steps for i and j indices, to take every "step" data.
 * @return std::vector<uint8_t> Vector containing colors (size : shape[0] *
 * shape[1] * 1 channel).
 */

/**
 * @brief
 *
 * @param array
 * @param step
 * @return std::vector<uint8_t>
 */
std::vector<uint8_t> colorize_grayscale(const Array     &array,
                                        std::vector<int> step = {1, 1});

std::vector<uint8_t> colorize_histogram(const Array     &array,
                                        std::vector<int> step = {1, 1});

/**
 * @brief Convert 3 array element values to a color data (3 channels RGB in [0,
 * 255]) use a multivariate colormap.
 *
 * @param c0 First array.
 * @param c1 Second array.
 * @param c2 Third array.
 * @param clut Colormap Lookup Table object.
 * @param hillshading Activate hillshading.
 * @return std::vector<uint8_t> Vector containing colors (size : shape[0] *
 * shape[1] * 3 channels for RGB).
 */
std::vector<uint8_t> colorize_trivariate(const Array &c0,
                                         const Array &c1,
                                         const Array &c2,
                                         Clut3D      &clut,
                                         bool         hillshading);

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
 * @brief Export an 8bit image to a png file.
 *
 * @param fname File name.
 * @param img Image data.
 * @param shape Image shape.
 */
void write_png_8bit(std::string           fname,
                    std::vector<uint8_t> &img,
                    std::vector<int>      shape);

} // namespace hmap
