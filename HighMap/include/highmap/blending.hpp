/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file blending.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-05-08
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

namespace hmap
{

/**
 * @brief Return the 'exclusion' blending of two arrays.
 *
 * See for instance https://en.wikipedia.org/wiki/Blend_modes.
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * @param array1 1st array.
 * @param array2 2nd array.
 * @return Array Reference to the resulting object.
 *
 * **Example**
 * @include ex_blend.cpp
 *
 * **Result**
 * @image html ex_blend0.png
 * @image html ex_blend1.png
 * @image html ex_blend2.png
 * @image html ex_blend3.png
 * @image html ex_blend4.png
 *
 * @see {@link blend_exclusion}, {@link blend_negate}, {@link blend_overlay},
 * {@link blend_soft}
 */
Array blend_exclusion(const Array &array1, const Array &array2);

/**
 * @brief Return the blending of two arrays based on their gradients.
 *
 * @param array1 1st array.
 * @param array2 2nd array.
 * @param ir Filtering radius (in pixels).
 * @return Array Reference to the resulting object.
 */
Array blend_gradients(const Array &array1, const Array &array2, int ir = 4);

/**
 * @brief Return the 'negate' blending of two arrays.
 *
 * @param array1 1st array.
 * @param array2 2nd array.
 * @return Array Reference to the resulting object.
 *
 * @see {@link blend_exclusion}, {@link blend_negate}, {@link blend_overlay},
 * {@link blend_soft}
 */
Array blend_negate(const Array &array1, const Array &array2);

/**
 * @brief Return the 'overlay' blending of two arrays.
 *
 * See for instance https://en.wikipedia.org/wiki/Blend_modes.
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * @param array1 1st array.
 * @param array2 2nd array.
 * @return Array Reference to the resulting object.
 *
 * @see {@link blend_exclusion}, {@link blend_negate}, {@link blend_overlay},
 * {@link blend_soft}
 */
Array blend_overlay(const Array &array1, const Array &array2);

/**
 * @brief Return the 'soft' blending of two arrays.
 *
 * Based on <a
 * href=http://www.pegtop.net/delphi/articles/blendmodes/softlight.htm>Pegtop
 * soft light mode</a>.
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * @param array1 1st array.
 * @param array2 2nd array.
 * @return Array Reference to the resulting object.
 *
 * @see {@link blend_exclusion}, {@link blend_negate}, {@link blend_overlay},
 * {@link blend_soft}
 */
Array blend_soft(const Array &array1, const Array &array2);

/**
 * @brief Return the mixing of a set of arrays based on a parameter `t`.
 *
 * @warning Values of array `t` are expected to be in [0, 1].
 *
 * @param t Mixing coefficient, define locally, in [0, 1].
 * @param arrays References to the input arrays.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_mixer.cpp
 *
 * **Result**
 * @image html ex_mixer.png
 */
Array mixer(const Array &t, const std::vector<Array *> arrays);

} // namespace hmap