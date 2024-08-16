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

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Kernel function.
 */
enum KernelType : int
{
  BIWEIGHT,
  CUBIC_PULSE,
  CONE,
  CONE_SMOOTH,
  DISK,
  LORENTZIAN,
  SMOOTH_COSINE,
  SQUARE,
  TRICUBE
};

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
 * @brief Return a "directional" cubic pulse kernel.
 *
 * @param shape Array shape.
 * @param angle Angle (in degrees).
 * @param aspect_ratio Pulse aspect ratio.
 * @param anisotropy Pulse width ratio between upstream and downstream sides.
 * @return Array New array.
 */
Array cubic_pulse_directional(Vec2<int> shape,
                              float     angle,
                              float     aspect_ratio,
                              float     anisotropy);

/**
 * @brief Return a "truncated" cubic pulse kernel.
 *
 * @param shape Array shape.
 * @param slant_ratio Truncation slope.
 * @param angle Angle (in degrees).
 * @return Array
 *
 * **Example**
 * @include ex_cubic_pulse_truncated.cpp
 *
 * **Result**
 * @image html ex_cubic_pulse_truncated.png
 */
Array cubic_pulse_truncated(Vec2<int> shape, float slant_ratio, float angle);

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
 * @param shape Array shape.
 * @param kw Kernel wavenumber, with respect to a unit domain.
 * @param angle Kernel angle (in degree).
 * @return Array New array.
 *
 * **Example**
 * @include ex_gabor.cpp
 *
 * **Result**
 * @image html ex_gabor.png
 */
Array gabor(Vec2<int> shape, float kw, float angle);

/**
 * @brief Return a modified dune-like Gabor kernel of a given shape.
 *
 * @param shape Array shape.
 * @param kw Kernel wavenumber, with respect to a unit domain.
 * @param angle Kernel angle (in degree).
 * @param xtop Relative location of the top of the dune profile (in [0, 1]).
 * @param xbottom Relative location of the foot of the dune profile (in [0, 1]).
 * @return Array New array.
 *
 * **Example**
 * @include ex_gabor_dune.cpp
 *
 * **Result**
 * @image html ex_gabor_dune.png
 */
Array gabor_dune(Vec2<int> shape,
                 float     kw,
                 float     angle,
                 float     xtop,
                 float     xbottom);

/**
 * @brief Return a kernel.
 * @param shape Kernel shape.
 * @param kernel_type Kernel type.
 * @return Array.
 */
Array get_kernel(Vec2<int> shape, KernelType kernel_type);

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
 * @brief Return a modified Lorentzian kernel with compact support.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array lorentzian_compact(Vec2<int> shape);

/**
 * @brief Return a smooth cosine kernel.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array smooth_cosine(Vec2<int> shape);

/**
 * @brief Return a squre.
 *
 * Maximum value is 1.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array square(Vec2<int> shape);

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
