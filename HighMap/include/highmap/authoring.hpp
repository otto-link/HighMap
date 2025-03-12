/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file authoring.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file containing functions for generating and manipulating
 * heightmaps through various techniques.
 *
 * This header file provides functions for creating and modifying heightmaps
 * using techniques such as stamping kernels, defining ridgelines, applying the
 * reverse midpoint displacement algorithm, and more. These functions offer a
 * range of capabilities for terrain generation, elevation modifications, and
 * heightmap processing.
 *
 * @version 0.1 Initial version
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023 Otto Link
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
 *        maintaining the 'shape' of the heightmap.
 *
 * This function modifies the elevation values in the input array based on a
 * cloud of points that specify the coordinates and variations for the
 * alterations. The function ensures that the shape of the heightmap is
 * preserved while applying these local changes.
 *
 * The alteration is performed using a kernel with a minimal radius (`ir`) and
 * the intensity of the alterations is scaled based on the `footprint_ratio`.
 * The `shift` parameter allows for introducing noise or offset in the
 * alteration process, and the `scale` parameter is used to adjust the domain of
 * the alteration.
 *
 * @param array Input array representing the heightmap to be altered. The
 * elevation values in this array will be adjusted according to the cloud data.
 * @param cloud Cloud object that defines the coordinates and elevation
 * variations for the alterations. This object provides the necessary data for
 * applying the local changes.
 * @param ir Alteration kernel minimal radius. This parameter defines the
 * minimum size of the kernel used for local alterations.
 * @param footprint_ratio Defines how the radius of the alteration scales with
 * the variation intensity. A higher ratio results in a larger footprint of the
 *                        alterations.
 * @param shift Noise shift specified as a vector {xs, ys} for each direction,
 * relative to a unit domain. This allows for adding random offsets to the
 * alteration process.
 * @param scale Domain scaling in the range [0, 1]. This parameter adjusts the
 * domain size over which the alteration is applied.
 *
 * **Example**
 * @include ex_alter_elevation.cpp
 *
 * **Result**
 * @image html ex_alter_elevation.png
 */
void alter_elevation(Array       &array,
                     const Cloud &cloud,
                     int          ir,
                     float        footprint_ratio = 1.f,
                     Vec2<float>  shift = {0.f, 0.f},
                     Vec2<float>  scale = {1.f, 1.f});

/**
 * @brief Generate a heightmap from a coarse grid of control points with defined
 * elevation values.
 *
 * This function creates a heightmap by interpolating elevation values provided
 * at specific control points on a coarse grid. The result is a finer resolution
 * heightmap where the elevation values are smoothly distributed over the entire
 * grid.
 *
 * The interpolation is performed using a Gaussian function, with the
 * `width_factor` parameter controlling the half-width of the base Gaussian
 * function used for smoothing. Optionally, noise can be added using the
 * provided `p_noise_x` and `p_noise_y` arrays, and the heightmap can be
 * adjusted by a local wavenumber multiplier specified by `p_stretching`. The
 * `bbox` parameter defines the domain bounding box for the heightmap.
 *
 * @param shape Dimensions of the output array (heightmap).
 * @param values Elevation values at the control points specified on a coarse
 * grid. This 2D vector holds the elevation data at each control point.
 * @param width_factor Factor applied to the half-width of the Gaussian function
 * used for smoothing the elevations. A value of 1.0 represents the default
 *                     smoothing width.
 * @param p_noise_x Optional pointer to an input noise array affecting the
 * x-direction. If provided, it is used to add noise to the elevation values.
 * @param p_noise_y Optional pointer to an input noise array affecting the
 * y-direction. If provided, it is used to add noise to the elevation values.
 * @param p_stretching Optional pointer to an input array that acts as a local
 * wavenumber multiplier. If provided, it adjusts the elevation values according
 * to the local wavenumber.
 * @param bbox Domain bounding box specified as a vector {xmin, xmax, ymin,
 * ymax}. This defines the extent of the heightmap in the coordinate space.
 * @return Array A new array representing the generated heightmap with
 * interpolated elevation values.
 *
 * **Example**
 * @include ex_base_elevation.cpp
 *
 * **Result**
 * @image html ex_base_elevation.png
 */
Array base_elevation(Vec2<int>                              shape,
                     const std::vector<std::vector<float>> &values,
                     float                                  width_factor = 1.f,
                     const Array                           *p_noise_x = nullptr,
                     const Array                           *p_noise_y = nullptr,
                     const Array *p_stretching = nullptr,
                     Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Apply the reverse midpoint displacement algorithm to the input array.
 *
 * This function implements the reverse midpoint displacement algorithm as
 * described in Belhadj et al. (2005). The algorithm generates a terrain-like
 * structure by applying displacement to the values in the input array based on
 * random noise. The function can be tuned using the `seed` parameter for random
 * number generation, `noise_scale` for controlling the amplitude of the noise,
 * and `threshold` for setting a background value.
 *
 * @param array Input array to which the reverse midpoint displacement algorithm
 * will be applied. The array's values are modified to produce the output
 * terrain.
 * @param seed Random seed number used to initialize the random number generator
 * for reproducibility.
 * @param noise_scale Amplitude of the noise applied during the displacement
 * process. A higher value results in more pronounced terrain features.
 * @param threshold Threshold 'background' value used to influence the
 * displacement algorithm. Values below this threshold may be treated
 * differently depending on the algorithm's design.
 * @return Array The output array after applying the reverse midpoint
 * displacement algorithm. This array contains the modified values representing
 * the generated terrain.
 *
 * **Example**
 * @include ex_reverse_midpoint.cpp
 *
 * **Result**
 * @image html ex_reverse_midpoint.png
 */
Array reverse_midpoint(const Array &array,
                       uint         seed,
                       float        noise_scale = 1.f,
                       float        threshold = 0.f);

/**
 * @brief Generate a heightmap based on a set of ridgelines and a specified
 * slope.
 *
 * This function creates a heightmap using a set of ridgelines defined by their
 * x, y, and z coordinates, and applies a specified slope to the ridgelines. The
 * function can optionally apply smoothing, adjust the ridge edge width, and
 * clamp values below a minimum threshold. Additional noise and stretching
 * parameters can be applied to modify the resulting heightmap.
 *
 * @param shape The dimensions of the output array (heightmap).
 * @param xr A vector of x-coordinates defining ridge segments.
 * @param yr A vector of y-coordinates defining ridge segments.
 * @param zr A vector of z-coordinates defining ridge segments.
 * @param slope The slope applied to the ridgelines. Can be negative to invert
 * the slope.
 * @param k_smoothing Smoothing parameter to control the smoothness of the
 * ridgelines.
 * @param width Width of the ridge edges. Determines how broad the ridges
 * appear.
 * @param vmin Minimum value for the heightmap. Values below this threshold will
 * be clamped to `vmin`.
 * @param bbox Bounding box for the entire domain, defining the area covered by
 * the heightmap.
 * @param p_noise_x Pointer to an optional array for x-direction noise to apply
 * to the heightmap.
 * @param p_noise_y Pointer to an optional array for y-direction noise to apply
 * to the heightmap.
 * @param p_stretching Pointer to an optional array for local wavenumber
 * multipliers to stretch the ridges.
 * @param bbox_array Bounding box for the array domain, defining the spatial
 * extent of the heightmap.
 *
 * @return Array The generated heightmap with ridgelines and applied slope.
 *
 * **Example**
 * @include ex_ridgelines.cpp
 *
 * **Result**
 * @image html ex_ridgelines.png
 */
Array ridgelines(Vec2<int>                 shape,
                 const std::vector<float> &xr,
                 const std::vector<float> &yr,
                 const std::vector<float> &zr,
                 float                     slope,
                 float                     k_smoothing = 1.f,
                 float                     width = 0.1f,
                 float                     vmin = 0.f,
                 Vec4<float>               bbox = {0.f, 1.f, 0.f, 1.f},
                 const Array              *p_noise_x = nullptr,
                 const Array              *p_noise_y = nullptr,
                 const Array              *p_stretching = nullptr,
                 Vec4<float>               bbox_array = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generate a heightmap based on a set of ridgelines with quadratic
 * Bezier interpolation.
 *
 * This function creates a heightmap using ridgelines defined by their x, y, and
 * z coordinates. The ridgelines are interpolated using quadratic Bezier curves
 * to smooth the transitions. The function applies a specified slope to the
 * ridgelines and can optionally apply smoothing, adjust ridge edge width, and
 * clamp values below a minimum threshold. Additional noise and stretching
 * parameters can be used to modify the resulting heightmap.
 *
 * @param shape The dimensions of the output array (heightmap).
 * @param xr A vector of x-coordinates defining ridge segments, organized in
 * groups of three (control points of the Bezier curves).
 * @param yr A vector of y-coordinates defining ridge segments, organized in
 * groups of three (control points of the Bezier curves).
 * @param zr A vector of z-coordinates defining ridge segments, organized in
 * groups of three (control points of the Bezier curves).
 * @param slope The slope applied to the ridgelines. Can be negative to invert
 * the slope.
 * @param k_smoothing Smoothing parameter to control the smoothness of the
 * ridgelines.
 * @param width Width of the ridge edges. Determines how broad the ridges
 * appear.
 * @param vmin Minimum value for the heightmap. Values below this threshold will
 * be clamped to `vmin`.
 * @param bbox Bounding box for the entire domain, defining the area covered by
 * the heightmap.
 * @param p_noise_x Pointer to an optional array for x-direction noise to apply
 * to the heightmap.
 * @param p_noise_y Pointer to an optional array for y-direction noise to apply
 * to the heightmap.
 * @param p_stretching Pointer to an optional array for local wavenumber
 * multipliers to stretch the ridges.
 * @param bbox_array Bounding box for the array domain, defining the spatial
 * extent of the heightmap.
 *
 * @return Array The generated heightmap with ridgelines interpolated using
 * quadratic Bezier curves and applied slope.
 *
 * **Example**
 * @include ex_ridgelines_bezier.cpp
 *
 * **Result**
 * @image html ex_ridgelines_bezier.png
 */
Array ridgelines_bezier(Vec2<int>                 shape,
                        const std::vector<float> &xr,
                        const std::vector<float> &yr,
                        const std::vector<float> &zr,
                        float                     slope,
                        float                     k_smoothing = 1.f,
                        float                     width = 0.1f,
                        float                     vmin = 0.f,
                        Vec4<float>               bbox = {0.f, 1.f, 0.f, 1.f},
                        const Array              *p_noise_x = nullptr,
                        const Array              *p_noise_y = nullptr,
                        const Array              *p_stretching = nullptr,
                        Vec4<float> bbox_array = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generate a heightmap by stamping a kernel at predefined locations.
 *
 * This function creates a heightmap by stamping a specified kernel at given x,
 * y, and z coordinates. The kernel can be scaled and manipulated based on the
 * z-coordinates of the stamping points, and can be optionally flipped or
 * rotated. The heightmap is generated by blending the results using the
 * specified blending method. Randomization options are available for kernel
 * manipulation.
 *
 * @param shape The dimensions of the output array (heightmap).
 * @param xr A vector of x-coordinates where the kernel is stamped.
 * @param yr A vector of y-coordinates where the kernel is stamped.
 * @param zr A vector of z-coordinates where the kernel is stamped. This affects
 * the kernel radius and amplitude scaling.
 * @param kernel The kernel to be stamped onto the heightmap.
 * @param kernel_ir The radius of the kernel in pixels.
 * @param kernel_scale_radius Boolean flag to scale the kernel radius based on
 * the z-coordinates.
 * @param kernel_scale_amplitude Boolean flag to scale the kernel amplitude
 * based on the z-coordinates.
 * @param blend_method The method used for blending multiple kernel stamps.
 * Options may include additive, maximum, minimum, etc.
 * @param seed Random seed number for kernel randomization.
 * @param k_smoothing Smoothing parameter for the heightmap (e.g., for smooth
 * minimum or maximum blending).
 * @param kernel_flip Boolean flag to randomly flip the kernel before stamping.
 * Flipping includes transposing.
 * @param kernel_rotate Boolean flag to randomly rotate the kernel before
 * stamping. Rotation can be resource-intensive.
 * @param bbox_array Bounding box for the array domain, defining the spatial
 * extent of the heightmap.
 *
 * @return Array The generated heightmap with kernel stamps applied at the
 * specified locations.
 *
 * **Example**
 * @include ex_stamping.cpp
 *
 * **Result**
 * @image html ex_stamping0.png
 * @image html ex_stamping1.png
 * @image html ex_stamping2.png
 *
 * @see {@link other_related_functions}
 */
Array stamping(Vec2<int>                 shape,
               const std::vector<float> &xr,
               const std::vector<float> &yr,
               const std::vector<float> &zr,
               Array                     kernel,
               int                       kernel_ir,
               bool                      kernel_scale_radius,
               bool                      kernel_scale_amplitude,
               StampingBlendMethod       blend_method,
               uint                      seed,
               float                     k_smoothing = 0.1f,
               bool                      kernel_flip = true,
               bool                      kernel_rotate = false,
               Vec4<float>               bbox_array = {0.f, 1.f, 0.f, 1.f});

} // namespace hmap
