/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file synthesis.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Provides functions for synthesizing new heightmaps from input arrays.
 *
 * This header file defines functions for synthesizing new heightmaps by
 * using various techniques such as non-parametric sampling, patch quilting,
 * and blending. These functions allow for the generation of larger or
 * modified heightmaps based on input data, using methods like patch stitching
 * and random sampling.
 *
 * Functions provided:
 * - `non_parametric_sampling`: Generates a new heightmap using non-parametric
 * sampling.
 * - `quilting`: Synthesizes a new heightmap by stitching patches from a set of
 * input heightmaps.
 * - `quilting_blend`: Similar to `quilting`, but includes blending of patches.
 * - `quilting_expand`: Synthesizes a larger heightmap by expanding and
 * stitching patches from an input heightmap.
 * - `quilting_shuffle`: Similar to `quilting_expand`, but reshuffles patches to
 * generate a new heightmap with the same shape.
 *
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023 Otto Link
 */

#pragma once

#include "highmap/array.hpp"
#include "macrologger.h"

namespace hmap
{

/**
 * @brief Synthesize a new heightmap based on an input array using a
 * non-parametric sampling method.
 *
 * This method generates a new heightmap by sampling patches from the input
 * array non-parametrically. It is a slow process and is based on the technique
 * described in @cite Efros1999.
 *
 * @param array Input array from which patches are sampled.
 * @param patch_shape Shape of the patches used for sampling.
 * @param seed Random seed number for patch selection.
 * @param error_threshold Threshold for patch selection based on error.
 * @return Array Resulting synthesized heightmap.
 *
 * **Example**
 * @include non_parametric_sampling.cpp
 *
 * **Result**
 * @image html non_parametric_sampling.png
 */
Array non_parametric_sampling(const Array    &array,
                              hmap::Vec2<int> patch_shape,
                              uint            seed,
                              float           error_threshold = 0.1f);

/**
 * @brief Synthesize a new heightmap by stitching together small patches from
 * input heightmaps.
 *
 * This function creates a new heightmap by stitching patches from a set of
 * input heightmaps. The stitching process allows for flipping, rotating, and
 * transposing patches, and includes options for smoothing based on the overlap
 * between patches. This technique is based on @cite Efros2001.
 *
 * @param p_arrays Vector of pointers to input heightmaps.
 * @param patch_base_shape Shape of the patches to be used.
 * @param tiling Number of patches in each direction (x, y).
 * @param overlap Overlap between patches, in the range ]0, 1[.
 * @param seed Random seed number for patch placement.
 * @param patch_flip Allow flipping of patches (up-down, left-right).
 * @param patch_rotate Allow 90-degree rotation of patches (for square patches).
 * @param patch_transpose Allow transposition of patches (for square patches).
 * @param filter_width_ratio Ratio of filter width to overlap length for
 * smoothing.
 * @return Array Resulting synthesized heightmap.
 *
 * **Example**
 * @include ex_quilting.cpp
 *
 * **Result**
 * @image html ex_quilting0.png
 * @image html ex_quilting1.png
 * @image html ex_quilting2.png
 * @image html ex_quilting3.png
 * @image html ex_quilting4.png
 */
Array quilting(const std::vector<const Array *> &p_arrays,
               hmap::Vec2<int>                   patch_base_shape,
               hmap::Vec2<int>                   tiling,
               float                             overlap,
               uint                              seed,
               std::vector<Array *>              secondary_arrays = {},
               bool                              patch_flip = true,
               bool                              patch_rotate = true,
               bool                              patch_transpose = true,
               float                             filter_width_ratio = 0.25f);

/**
 * @brief Synthesize a new heightmap by stitching together small patches from a
 * list of input heightmaps.
 *
 * This function creates a new heightmap by stitching patches from a set of
 * input heightmaps, similar to `quilting`, but the patches are blended
 * together. The blending options include flipping, rotating, and transposing
 * patches, with smoothing based on the overlap between patches.
 *
 * @param p_arrays Vector of pointers to input heightmaps.
 * @param patch_base_shape Shape of the patches to be used.
 * @param overlap Overlap between patches, in the range ]0, 1[.
 * @param seed Random seed number for patch placement.
 * @param patch_flip Allow flipping of patches (up-down, left-right).
 * @param patch_rotate Allow 90-degree rotation of patches (for square patches).
 * @param patch_transpose Allow transposition of patches (for square patches).
 * @param filter_width_ratio Ratio of filter width to overlap length for
 * smoothing.
 * @return Array Resulting synthesized heightmap.
 *
 * **Example**
 * @include ex_quilting.cpp
 *
 * **Result**
 * @image html ex_quilting0.png
 * @image html ex_quilting1.png
 * @image html ex_quilting2.png
 * @image html ex_quilting3.png
 * @image html ex_quilting4.png
 */
Array quilting_blend(const std::vector<const Array *> &p_arrays,
                     hmap::Vec2<int>                   patch_base_shape,
                     float                             overlap,
                     uint                              seed,
                     bool                              patch_flip = true,
                     bool                              patch_rotate = true,
                     bool                              patch_transpose = true,
                     float filter_width_ratio = 0.25f);

/**
 * @brief Synthesize a new heightmap by expanding the input heightmap and
 * stitching patches.
 *
 * This function generates a larger heightmap by expanding the input heightmap
 * and stitching patches based on the given parameters. The expansion ratio
 * determines the new size of the heightmap, and the function includes options
 * for flipping, rotating, and transposing patches. The output can be either the
 * same shape as the input or expanded based on the given ratio.
 *
 * @param array Input heightmap to be expanded.
 * @param expansion_ratio Ratio by which to expand the heightmap (e.g., 2 for
 * doubling the size).
 * @param patch_base_shape Shape of the patches to be used.
 * @param overlap Overlap between patches, in the range ]0, 1[.
 * @param seed Random seed number for patch placement.
 * @param keep_input_shape If true, the output heightmap retains the input
 * shape.
 * @param patch_flip Allow flipping of patches (up-down, left-right).
 * @param patch_rotate Allow 90-degree rotation of patches (for square patches).
 * @param patch_transpose Allow transposition of patches (for square patches).
 * @param filter_width_ratio Ratio of filter width to overlap length for
 * smoothing.
 * @return Array Resulting synthesized heightmap.
 *
 * **Example**
 * @include ex_quilting.cpp
 *
 * **Result**
 * @image html ex_quilting0.png
 * @image html ex_quilting1.png
 * @image html ex_quilting2.png
 * @image html ex_quilting3.png
 * @image html ex_quilting4.png
 */
Array quilting_expand(const Array         &array,
                      float                expansion_ratio,
                      hmap::Vec2<int>      patch_base_shape,
                      float                overlap,
                      uint                 seed,
                      std::vector<Array *> secondary_arrays = {},
                      bool                 keep_input_shape = false,
                      bool                 patch_flip = true,
                      bool                 patch_rotate = true,
                      bool                 patch_transpose = true,
                      float                filter_width_ratio = 0.25f);

/**
 * @brief Synthesize a new heightmap by reshuffling patches of the input
 * heightmap.
 *
 * This function generates a new heightmap by reshuffling patches of the input
 * heightmap, effectively creating a new pattern while keeping the same shape as
 * the input heightmap. The function includes options for patch flipping,
 * rotating, and transposing.
 *
 * @param array Input heightmap to be reshuffled.
 * @param patch_base_shape Shape of the patches to be used.
 * @param overlap Overlap between patches, in the range ]0, 1[.
 * @param seed Random seed number for patch placement.
 * @param patch_flip Allow flipping of patches (up-down, left-right).
 * @param patch_rotate Allow 90-degree rotation of patches (for square patches).
 * @param patch_transpose Allow transposition of patches (for square patches).
 * @param filter_width_ratio Ratio of filter width to overlap length for
 * smoothing.
 * @return Array Resulting synthesized heightmap with reshuffled patches.
 *
 * **Example**
 * @include ex_quilting.cpp
 *
 * **Result**
 * @image html ex_quilting0.png
 * @image html ex_quilting1.png
 * @image html ex_quilting2.png
 * @image html ex_quilting3.png
 * @image html ex_quilting4.png
 */
Array quilting_shuffle(const Array         &array,
                       hmap::Vec2<int>      patch_base_shape,
                       float                overlap,
                       uint                 seed,
                       std::vector<Array *> secondary_arrays = {},
                       bool                 patch_flip = true,
                       bool                 patch_rotate = true,
                       bool                 patch_transpose = true,
                       float                filter_width_ratio = 0.25f);

} // namespace hmap
