/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file blending.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Provides functions for various array blending modes.
 *
 * This header file contains functions for blending two arrays using different
 * techniques and modes. The available blending modes include exclusion,
 * negate, overlay, soft light, and gradient-based blending. These functions
 * are useful for image processing and heightmap manipulations where blending
 * effects are required.
 *
 * @version 0.1
 * @date 2023-05-08
 *
 * @copyright Copyright (c) 2023 Otto Link
 */

#pragma once
#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Return the 'exclusion' blending of two arrays.
 *
 * The exclusion blend mode creates a blend that is similar to the difference
 * blend mode but with lower contrast. It produces an effect that is often
 * used in image editing for special effects. For more details, see
 * https://en.wikipedia.org/wiki/Blend_modes.
 *
 * @warning Array values are expected to be in the range [0, 1].
 *
 * @param array1 First input array.
 * @param array2 Second input array.
 * @return Array Resulting array after applying the exclusion blend mode.
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
 *      {@link blend_soft}
 */
Array blend_exclusion(const Array &array1, const Array &array2);

/**
 * @brief Return the blending of two arrays based on their gradients.
 *
 * This function blends two arrays by considering their gradients, which can
 * be useful for producing smooth transitions between the arrays based on
 * their directional changes.
 *
 * @param array1 First input array.
 * @param array2 Second input array.
 * @param ir Filtering radius in pixels.
 * @return Array Resulting array after blending based on gradients.
 */
Array blend_gradients(const Array &array1, const Array &array2, int ir = 4);

/**
 * @brief Return the 'negate' blending of two arrays.
 *
 * The negate blend mode inverts the colors of one of the arrays relative to
 * the other. It creates a high-contrast effect that can be useful for
 * emphasizing differences between two arrays.
 *
 * @param array1 First input array.
 * @param array2 Second input array.
 * @return Array Resulting array after applying the negate blend mode.
 *
 * @see {@link blend_exclusion}, {@link blend_negate}, {@link blend_overlay},
 *      {@link blend_soft}
 */
Array blend_negate(const Array &array1, const Array &array2);

/**
 * @brief Return the 'overlay' blending of two arrays.
 *
 * The overlay blend mode combines the colors of two arrays in a way that
 * enhances contrast and emphasizes details.
 *
 * @warning Array values are expected to be in the range [0, 1].
 *
 * @param array1 First input array.
 * @param array2 Second input array.
 * @return Array Resulting array after applying the overlay blend mode.
 *
 * @see {@link blend_exclusion}, {@link blend_negate}, {@link blend_overlay},
 *      {@link blend_soft}
 */
Array blend_overlay(const Array &array1, const Array &array2);

/**
 * @brief Return the 'soft' blending of two arrays.
 *
 * The soft blend mode applies a soft light effect to the blending of two
 * arrays, based on the soft light mode described in
 * <a href=http://www.pegtop.net/delphi/articles/blendmodes/softlight.htm>Pegtop
 * soft light mode</a>. It creates a subtle blend with reduced contrast.
 *
 * @warning Array values are expected to be in the range [0, 1].
 *
 * @param array1 First input array.
 * @param array2 Second input array.
 * @return Array Resulting array after applying the soft light blend mode.
 *
 * @see {@link blend_exclusion}, {@link blend_negate}, {@link blend_overlay},
 *      {@link blend_soft}
 */
Array blend_soft(const Array &array1, const Array &array2);

/**
 * @brief Return the mixing of a set of arrays based on a parameter `t`.
 *
 * This function mixes a set of arrays based on a mixing coefficient `t`
 * that determines the contribution of each array. The parameter `t` should
 * be in the range [0, 1], where `t` represents the blending ratio.
 *
 * @warning Values of array `t` are expected to be in the range [0, 1].
 *
 * @param t Mixing coefficient, in the range [0, 1].
 * @param arrays References to the input arrays to be mixed.
 * @return Array Resulting array after mixing.
 *
 * **Example**
 * @include ex_mixer.cpp
 *
 * **Result**
 * @image html ex_mixer.png
 */
Array mixer(const Array &t, const std::vector<Array *> arrays);

} // namespace hmap
