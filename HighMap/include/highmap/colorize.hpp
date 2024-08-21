/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file colorize.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file for functions related to applying colorization and
 * hillshading.
 *
 * This file contains the declarations of functions that are used to apply
 * colorization and hillshading effects to images and arrays. The functions
 * allow for creating grayscale, histogram-based, and colored images from
 * elevation data and other input arrays.
 *
 * @version 0.1
 * @date 2023-05-08
 *
 * @copyright Copyright (c) 2023 Otto Link
 *
 */
#pragma once
#include <png.h>

#include "highmap/array.hpp"
#include "highmap/tensor.hpp"

namespace hmap
{

/**
 * @enum Cmap
 * @brief Enumeration for different colormap types.
 *
 * This enumeration is defined in the highmap/colormap.hpp file
 * and is used for selecting the colormap to be applied during colorization.
 */
enum Cmap : int; // highmap/colormap.hpp

/**
 * @brief Apply hillshading to a Tensor image.
 *
 * This function applies a hillshading effect to the provided tensor image
 * using the elevation data in the input array. The effect is controlled
 * by the power exponent and can be scaled by specifying the minimum and
 * maximum values.
 *
 * @param img Input image represented as a Tensor.
 * @param array Elevation data used to generate the hillshading effect.
 * @param vmin Minimum value for scaling the hillshading effect (default is
 * 0.f).
 * @param vmax Maximum value for scaling the hillshading effect (default
 * is 1.f).
 * @param exponent Power exponent applied to the hillshade values (default
 * is 1.f).
 */
void apply_hillshade(Tensor            &img,
                     const hmap::Array &array,
                     float              vmin = 0.f,
                     float              vmax = 1.f,
                     float              exponent = 1.f);

/**
 * @brief Apply hillshading to an 8-bit image.
 *
 * This function applies a hillshading effect to a vector of 8-bit image data.
 * The effect is computed based on the elevation data in the input array.
 * The image can be either RGB or RGBA format depending on the is_img_rgba flag.
 *
 * @param img Input image represented as a vector of 8-bit data.
 * @param array Elevation data used to generate the hillshading effect.
 * @param vmin Minimum value for scaling the hillshading effect (default is
 * 0.f).
 * @param vmax Maximum value for scaling the hillshading effect (default
 * is 1.f).
 * @param exponent Power exponent applied to the hillshade values (default
 * is 1.f).
 * @param is_img_rgba Boolean flag indicating if the input image has an alpha
 * channel (default is false).
 */
void apply_hillshade(std::vector<uint8_t> &img,
                     const hmap::Array    &array,
                     float                 vmin = 0.f,
                     float                 vmax = 1.f,
                     float                 exponent = 1.f,
                     bool                  is_img_rgba = false);

/**
 * @brief Apply colorization to an array.
 *
 * This function applies a colormap to the input array and optionally applies
 * hillshading and noise. The colorization can be reversed, and the function
 * returns a Tensor representing the colorized image.
 *
 * @param array Input array to be colorized.
 * @param vmin Minimum value for scaling the colormap.
 * @param vmax Maximum value for scaling the colormap.
 * @param cmap Colormap to be applied (using the Cmap enum).
 * @param hillshading Boolean flag to apply hillshading.
 * @param reverse Boolean flag to reverse the colormap (default is false).
 * @param p_noise Optional pointer to a noise array (default is nullptr).
 * @return Tensor Colorized Tensor image.
 */
Tensor colorize(Array &array,
                float  vmin,
                float  vmax,
                int    cmap,
                bool   hillshading,
                bool   reverse = false,
                Array *p_noise = nullptr);

/**
 * @brief Convert an array to a grayscale image.
 *
 * This function converts the input array to an 8-bit grayscale Tensor image.
 *
 * @param array Input array.
 * @return Tensor Grayscale Tensor image.
 */
Tensor colorize_grayscale(const Array &array);

/**
 * @brief Convert an array to a histogram-based grayscale image.
 *
 * This function converts the input array to an 8-bit grayscale Tensor image
 * using a histogram-based method for enhanced contrast.
 *
 * @param array Input array.
 * @return Tensor Grayscale Tensor image with histogram-based contrast.
 */
Tensor colorize_histogram(const Array &array);

/**
 * @brief Combine two arrays into a colored image.
 *
 * This function takes two input arrays and combines them into a single
 * 8-bit colored Tensor image. The resulting image uses the data from
 * both arrays to create a composite color representation.
 *
 * @param array1 First input array.
 * @param array2 Second input array.
 * @return Tensor Colorized Tensor image.
 *
 * **Example**
 * @include ex_colorize_vec2.cpp
 *
 * **Result**
 * @image html ex_colorize_vec2.png
 */
Tensor colorize_vec2(const Array &array1, const Array &array2);

} // namespace hmap
