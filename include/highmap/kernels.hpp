/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file kernels.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <functional>

#include "highmap/vector.hpp"

namespace hmap
{

/**
 * @brief Return a biweight kernel.
 *
 * See https://en.wikipedia.org/wiki/Kernel_%28statistics%29.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array biweight(Vec2<int> shape);

/**
 * @brief Return a cone.
 *
 * Maximum value is 1.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array cone(Vec2<int> shape);

/**
 * @brief Return a cone with a given height and talus (output array shape is
 * adjusted accordingly).
 *
 * @param height Cone height.
 * @param talus Cone talus.
 * @return Array
 */
Array cone_talus(float height, float talus);

/**
 * @brief Return a cone with a smooth landing (zero derivative at the cone
 * bottom).
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array cone_smooth(Vec2<int> shape);

/**
 * @brief Return a cubic pulse kernel.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array cubic_pulse(Vec2<int> shape);

/**
 * @brief Return a disk foot-print.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array disk(Vec2<int> shape);

/**
 * @brief Return a Gabor kernel of a given shape.
 *
 *   Kernel width is deduced from the array shape: the kernel footprint
 *   is limited to where envelop is larger than the value
 *   `footprint_threshold`.
 *
 * @param shape Array shape.
 * @param kw Kernel wavenumber, with respect to a unit domain.
 * @param angle Kernel angle (in degree).
 * @param footprint_threshold The kernel footprint is limited where envelop is
 * larger than the value `footprint_threshold`.
 * @return Array New array.
 *
 * **Example**
 * @include ex_gabor.cpp
 *
 * **Result**
 * @image html ex_gabor.png
 */
Array gabor(Vec2<int> shape,
            float     kw,
            float     angle,
            float     footprint_threshold = 0.05f);

/**
 * @brief Return the Lorentzian kernel.
 *
 * @param shape Array shape.
 * @param footprint_threshold The kernel width is determined using the footprint
 * threshold.
 * @return Array New array.
 */
Array lorentzian(Vec2<int> shape, float footprint_threshold = 0.1f);

/**
 * @brief Return a smooth cosine kernel.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array smooth_cosine(Vec2<int> shape);

/**
 * @brief Return a tricube kernel.
 *
 * See https://en.wikipedia.org/wiki/Kernel_%28statistics%29.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array tricube(Vec2<int> shape);

} // namespace hmap
