/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file synthesis.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "macrologger.h"

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Synthesize a new heightmap based on an input array using a
 * non-parametric sampling method (very slow).
 *
 * See @cite Efros1999
 *
 * @param array Input array.
 * @param patch_shape Patch shape.
 * @param seed Random seed number.
 * @param error_threshold Error threshold for the patch selection process.
 * @return Synthetized array.
 *
 * **Example**
 * @include non_parametric_sampling.cpp
 *
 * **Result**
 * @image html non_parametric_sampling.png
 */
Array non_parametric_sampling(Array          &array,
                              hmap::Vec2<int> patch_shape,
                              uint            seed,
                              float           error_threshold = 0.1f);

/**
 * @brief Synthesize a new heightmap by stitching together small patches of the
 * input heightmaps.
 *
 * See @cite Efros2001.
 *
 * @param p_arrays Reference to the input arrays as a vector.
 * @param patch_base_shape Patch shape.
 * @param tiling Patch tiling.
 * @param overlap Patch overlap, in ]0, 1[.
 * @param seed Random seed number.
 * @param patch_flip Allow patch flipping (up-down and left-right).
 * @param patch_rotate Allow patch 90 degree rotation (square patches only).
 * @param patch_transpose Allow patch tranposition (square patches only).
 * @param filter_width_ratio Smooth filter width with respect the overlap
 * length.
 * @return Synthetized array (shape determined by patch shape and tiling
 * features).
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
Array quilting(std::vector<Array *> p_arrays,
               hmap::Vec2<int>      patch_base_shape,
               hmap::Vec2<int>      tiling,
               float                overlap,
               uint                 seed,
               bool                 patch_flip = true,
               bool                 patch_rotate = true,
               bool                 patch_transpose = true,
               float                filter_width_ratio = 0.25f);

/**
 * @brief Synthesize a new heightmap by stitching together small patches from a
 * list of input heightmaps.
 *
 * @param array Reference to the input arrays.
 * @param patch_base_shape Patch shape.
 * @param overlap Patch overlap, in ]0, 1[.
 * @param seed Random seed number.
 * @param patch_flip Allow patch flipping (up-down and left-right).
 * @param patch_rotate Allow patch 90 degree rotation (square patches only).
 * @param patch_transpose Allow patch tranposition (square patches only).
 * @param filter_width_ratio Smooth filter width with respect the overlap
 * length.
 * @return Synthetized array.
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
Array quilting_blend(std::vector<Array *> p_arrays,
                     hmap::Vec2<int>      patch_base_shape,
                     float                overlap,
                     uint                 seed,
                     bool                 patch_flip = true,
                     bool                 patch_rotate = true,
                     bool                 patch_transpose = true,
                     float                filter_width_ratio = 0.25f);

/**
 * @brief Synthesize a new heightmap by stitching together small patches of the
 * input heightmap. Wrapper to expand the input.
 *
 * @param array Input array.
 * @param expansion_ratio Determine the nex extent of the heightmap. If set to
 * 2, the heightmap is 2-times larger with features of the same "size".
 * @param patch_base_shape Patch shape.
 * @param overlap Patch overlap, in ]0, 1[.
 * @param seed Random seed number.
 * @param keep_input_shape If set to true, the output has the same shape as the
 * input.
 * @param patch_flip Allow patch flipping (up-down and left-right).
 * @param patch_rotate Allow patch 90 degree rotation (square patches only).
 * @param patch_transpose Allow patch tranposition (square patches only).
 * @param filter_width_ratio Smooth filter width with respect the overlap
 * length.
 * @return Synthetized array.
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
Array quilting_expand(Array          &array,
                      float           expansion_ratio,
                      hmap::Vec2<int> patch_base_shape,
                      float           overlap,
                      uint            seed,
                      bool            keep_input_shape = false,
                      bool            patch_flip = true,
                      bool            patch_rotate = true,
                      bool            patch_transpose = true,
                      float           filter_width_ratio = 0.25f);

/**
 * @brief Synthesize a new heightmap by stitching together small patches of the
 * input heightmap. Wrapper to reshuffle an heightmap with the same shape as the
 * input.
 *
 * @param array Input array.
 * @param patch_base_shape Patch shape.
 * @param overlap Patch overlap, in ]0, 1[.
 * @param seed Random seed number.
 * @param patch_flip Allow patch flipping (up-down and left-right).
 * @param patch_rotate Allow patch 90 degree rotation (square patches only).
 * @param patch_transpose Allow patch tranposition (square patches only).
 * @param filter_width_ratio Smooth filter width with respect the overlap
 * length.
 * @return Synthetized array.
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
Array quilting_shuffle(Array          &array,
                       hmap::Vec2<int> patch_base_shape,
                       float           overlap,
                       uint            seed,
                       bool            patch_flip = true,
                       bool            patch_rotate = true,
                       bool            patch_transpose = true,
                       float           filter_width_ratio = 0.25f);

} // namespace hmap