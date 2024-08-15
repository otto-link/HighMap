/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file authoring.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include "highmap/array.hpp"

namespace hmap
{

class Cloud; // from highmap/geometry.hpp

/**
 * @brief Blending method for the stamping operator.
 */
enum StampingBlendMethod : int
{
  ADD,            ///< add
  MAXIMUM,        ///< maximum
  MAXIMUM_SMOOTH, ///< maximum smooth
  MINIMUM,        ///< minimum
  MINIMUM_SMOOTH, ///< minimum smooth
  MULTIPLY,       ///< multiply
  SUBSTRACT,      ///< substract
};

/**
 * @brief Point-wise alteration: locally enforce a new elevation value while
    maintaining the 'shape' of the heightmap.
 *
 * @param array Input array.
 * @param cloud Cloud object, defining alteration coordinates and elevation
 variations.
 * @param ir Alteration kernel minimal radius.
 * @param footprint_ratio Defined how the radius is scales with variation
 intensity (the greater the larger the alterations footprint)
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
 *
 * **Example**
 * @include ex_alter_elevation.cpp
 *
 * **Result**
 * @image html ex_alter_elevation.png
 */
void alter_elevation(Array      &array,
                     Cloud      &cloud,
                     int         ir,
                     float       footprint_ratio = 1.f,
                     Vec2<float> shift = {0.f, 0.f},
                     Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return an heightmap defined by a set of elevation values defined on a
 * very coarse grid of control points.
 *
 * @param shape Array shape.
 * @param values Elevation at the control points.
 * @param width_factor Factor applied to the half-width of the base Gaussian.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array New array.
 *
 * **Example**
 * @include ex_base_elevation.cpp
 *
 * **Result**
 * @image html ex_base_elevation.png
 */
Array base_elevation(Vec2<int>                       shape,
                     std::vector<std::vector<float>> values,
                     float                           width_factor = 1.f,
                     Array                          *p_noise_x = nullptr,
                     Array                          *p_noise_y = nullptr,
                     Array                          *p_stretching = nullptr,
                     Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Apply the reverse midpoint displacement algorithm to the input array
 * (see @cite Belhadj2005).
 *
 * @param array Input array.
 * @param seed Random seed number.
 * @param noise_scale Noise scale amplitude.
 * @param threshold Theshold 'background' value.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_reverse_midpoint.cpp
 *
 * **Result**
 * @image html ex_reverse_midpoint.png
 */
Array reverse_midpoint(Array &array,
                       uint   seed,
                       float  noise_scale = 1.f,
                       float  threshold = 0.f);

/**
 * @brief Return an heightmap defined by a set of ridgelines and a given slope.
 *
 * @param shape Array shape.
 * @param xr Ridge segments x coordinates (as pairs).
 * @param yr Ridge segments y coordinates (as pairs).
 * @param zr Ridge segments z coordinates (as pairs).
 * @param slope Ridge slope (can be negative).
 * @param k_smoothing Smoothing.
 * @param width Ridge edge width.
 * @param vmin Minimum value (lower values are clamped).
 * @param bbox Bounding box corresponding to the whole domain.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox_array Array domain bounding box.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_ridgelines.cpp
 *
 * **Result**
 * @image html ex_ridgelines.png
 */
Array ridgelines(Vec2<int>          shape,
                 std::vector<float> xr,
                 std::vector<float> yr,
                 std::vector<float> zr,
                 float              slope,
                 float              k_smoothing = 1.f,
                 float              width = 0.1f,
                 float              vmin = 0.f,
                 Vec4<float>        bbox = {0.f, 1.f, 0.f, 1.f},
                 Array             *p_noise_x = nullptr,
                 Array             *p_noise_y = nullptr,
                 Array             *p_stretching = nullptr,
                 Vec4<float>        bbox_array = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an heightmap defined by a set of ridgelines and a given slope.
 * Quadratic Bezier interpolation is used for ridgeline curve.
 *
 * @param shape Array shape.
 * @param xr Ridge segments x coordinates (as groups of three).
 * @param yr Ridge segments y coordinates (as groups of three).
 * @param zr Ridge segments z coordinates (as groups of three).
 * @param slope Ridge slope (can be negative).
 * @param k_smoothing Smoothing.
 * @param width Ridge edge width.
 * @param vmin Minimum value (lower values are clamped).
 * @param bbox Bounding box corresponding to the whole domain.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox_array Array domain bounding box.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_ridgelines_bezier.cpp
 *
 * **Result**
 * @image html ex_ridgelines_bezier.png
 */
Array ridgelines_bezier(Vec2<int>          shape,
                        std::vector<float> xr,
                        std::vector<float> yr,
                        std::vector<float> zr,
                        float              slope,
                        float              k_smoothing = 1.f,
                        float              width = 0.1f,
                        float              vmin = 0.f,
                        Vec4<float>        bbox = {0.f, 1.f, 0.f, 1.f},
                        Array             *p_noise_x = nullptr,
                        Array             *p_noise_y = nullptr,
                        Array             *p_stretching = nullptr,
                        Vec4<float>        bbox_array = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an heightmap filled by stamping a kernel at predefined input
 * locations.
 * @param shape Output array shape.
 * @param xr Stamping locations x coordinates.
 * @param yr Stamping locations y coordinates.
 * @param zr Stamping locations z coordinates.
 * @param kernel Kernel to be stamped
 * @param kernel_ir Kernel radius (in pixels).
 * @param kernel_scale_radius Scale, or not, the kernel radius using the 'z'
 * coordinates of the points.
 * @param kernel_scale_amplitude Scale, or not, the kernel amplitude using the
 * 'z' coordinates of the points.
 * @param blend_method Blending method.
 * @param seed Random seed number.
 * @param k_smoothing Smoothing (if any, e.g. for smooth min and max).
 * @param kernel_flip Randomly flip, or not, the kernel before stamping
 * (flipping including tranposing).
 * @param kernel_rotate Randomly rotate, or not, the kernel before stamping (can
 * be any rotation angle, can also be ressource consuming).
 * @param bbox_array Array domain bounding box.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_stamping.cpp
 *
 * **Result**
 * @image html ex_stamping0.png
 * @image html ex_stamping1.png
 * @image html ex_stamping2.png
 */
Array stamping(Vec2<int>           shape,
               std::vector<float>  xr,
               std::vector<float>  yr,
               std::vector<float>  zr,
               Array               kernel,
               int                 kernel_ir,
               bool                kernel_scale_radius,
               bool                kernel_scale_amplitude,
               StampingBlendMethod blend_method,
               uint                seed,
               float               k_smoothing = 0.1f,
               bool                kernel_flip = true,
               bool                kernel_rotate = false,
               Vec4<float>         bbox_array = {0.f, 1.f, 0.f, 1.f});

} // namespace hmap
