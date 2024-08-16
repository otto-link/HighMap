/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file primitives.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "FastNoiseLite.h"

#include "highmap/array.hpp"
#include "highmap/functions.hpp"

#define HMAP_GRADIENT_OFFSET 0.001f

namespace hmap
{

/**
 * @brief Return a 'biquadratic pulse'.
 *
 * @param shape Array shape.
 * @param gain Gain (the higher, the steeper).
 *  @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the gain parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Perlin billow noise.
 *
 * **Example**
 * @include ex_biquad_pulse.cpp
 *
 * **Result**
 * @image html ex_biquad_pulse.png
 */
Array biquad_pulse(Vec2<int>   shape,
                   float       gain = 1.f,
                   Array      *p_ctrl_param = nullptr,
                   Array      *p_noise_x = nullptr,
                   Array      *p_noise_y = nullptr,
                   Array      *p_stretching = nullptr,
                   Vec2<float> center = {0.5f, 0.5f},
                   Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a bump.
 *
 * @param shape Array shape.
 * @param gain Gain (the higher, the steeper the bump).
 *  @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the gain parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param center Primitive reference center.
 * @param bbox Domain bounding box.
 * @return Array Perlin billow noise.
 *
 * **Example**
 * @include ex_bump.cpp
 *
 * **Result**
 * @image html ex_bump.png
 */
Array bump(Vec2<int>   shape,
           float       gain = 1.f,
           Array      *p_ctrl_param = nullptr,
           Array      *p_noise_x = nullptr,
           Array      *p_noise_y = nullptr,
           Array      *p_stretching = nullptr,
           Vec2<float> center = {0.5f, 0.5f},
           Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a caldera-shaped heightmap.
 *
 * @param shape Array shape.
 * @param radius Crater radius at the ridge.
 * @param sigma_inner Inner half-width.
 * @param sigma_outer Outer half-width.
 * @param z_bottom Bottom elevation (ridge is at elevation `1`).
 * @param p_noise Displacement noise.
 * @param noise_amp_r Radial noise absolute scale (in pixels).
 * @param noise_ratio_z Vertical noise relative scale (in [0, 1]).
 * @param center Primitive reference center.
 * @param bbox Domain bounding box.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_caldera.cpp
 *
 * **Result**
 * @image html ex_caldera.png
 */
Array caldera(Vec2<int>   shape,
              float       radius,
              float       sigma_inner,
              float       sigma_outer,
              float       z_bottom,
              Array      *p_noise,
              float       noise_amp_r,
              float       noise_ratio_z,
              Vec2<float> center = {0.5f, 0.5f},
              Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

Array caldera(Vec2<int>   shape,
              float       radius,
              float       sigma_inner,
              float       sigma_outer,
              float       z_bottom,
              Vec2<float> center = {0.5f, 0.5f},
              Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f}); ///< @overload

/**
 * @brief Return a checkerboard heightmap.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumber with respect to a unit domain.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array New array.
 *
 * **Example**
 * @include ex_checkerboard.cpp
 *
 * **Result**
 * @image html ex_checkerboard.png
 */
Array checkerboard(Vec2<int>   shape,
                   Vec2<float> kw,
                   Array      *p_noise_x = nullptr,
                   Array      *p_noise_y = nullptr,
                   Array      *p_stretching = nullptr,
                   Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a constant value array.
 *
 * @param shape Array shape.
 * @param value Filling value.
 * @return Array New array.
 */
Array constant(Vec2<int> shape, float value = 0.f);

/**
 * @brief Return a crater-shaped heightmap.
 *
 * @param shape Array shape.
 * @param radius Crater radius.
 * @param lip_decay Ejecta lip decay.
 * @param lip_height_ratio Controls the ejecta lip relative height, in [0, 1].
 * @param depth Crater depth.
 * @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the lip_height_ratio parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param bbox Domain bounding box.
 * @return Array New array.
 *
 * **Example**
 * @include ex_crater.cpp
 *
 * **Result**
 * @image html ex_crater.png
 */
Array crater(Vec2<int>   shape,
             float       radius,
             float       depth,
             float       lip_decay,
             float       lip_height_ratio = 0.5f,
             Array      *p_ctrl_param = nullptr,
             Array      *p_noise_x = nullptr,
             Array      *p_noise_y = nullptr,
             Vec2<float> center = {0.5f, 0.5f},
             Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Dendry is a locally computable procedural function that generates
 * branching patterns at various scales (see @cite Gaillard2019).
 *
 * @param shape Array shape.
 * @param kw Noise wavenumber with respect to a unit domain.
 * @param seed Random seed number.
 * @param control_array Control array (can be of any shape, different from
 * `shape`).
 * @param eps Epsilon used to bias the area where points are generated in cells.
 * @param resolution Number of resolutions in the noise function.
 * @param displacement Maximum displacement of segments.
 * @param primitives_resolution_steps Additional resolution steps in the
 * ComputeColorPrimitives function.
 * @param slope_power Additional parameter to control the variation of slope on
 * terrains.
 * @param noise_amplitude_proportion Proportion of the amplitude of the control
 * function as noise.
 * @param add_control_function Add control function to the output.
 * @param control_function_overlap Extent of the extension added at the domain
 * frontiers of the control array.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array New array.
 *
 * **Example**
 * @include ex_dendry.cpp
 *
 * **Result**
 * @image html ex_dendry.png
 */
Array dendry(Vec2<int>   shape,
             Vec2<float> kw,
             uint        seed,
             Array      &control_function,
             float       eps = 0.05,
             int         resolution = 1,
             float       displacement = 0.075,
             int         primitives_resolution_steps = 3,
             float       slope_power = 2.f,
             float       noise_amplitude_proportion = 0.01,
             bool        add_control_function = true,
             float       control_function_overlap = 0.5f,
             Array      *p_noise_x = nullptr,
             Array      *p_noise_y = nullptr,
             Array      *p_stretching = nullptr,
             Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f},
             int         subsampling = 1);

Array dendry(Vec2<int>      shape,
             Vec2<float>    kw,
             uint           seed,
             NoiseFunction &noise_function,
             float          noise_function_offset = 0.f,
             float          noise_function_scaling = 1.f,
             float          eps = 0.05,
             int            resolution = 1,
             float          displacement = 0.075,
             int            primitives_resolution_steps = 3,
             float          slope_power = 2.f,
             float          noise_amplitude_proportion = 0.01,
             bool           add_control_function = true,
             float          control_function_overlap = 0.5f,
             Array         *p_noise_x = nullptr,
             Array         *p_noise_y = nullptr,
             Array         *p_stretching = nullptr,
             Vec4<float>    bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generates a diffusion-limited aggregation (DLA) pattern.
 *
 * This function simulates the diffusion-limited aggregation process to generate
 * a pattern within a grid of specified dimensions. The DLA process models
 * particles that undergo a random walk until they stick to a seed, gradually
 * forming complex fractal structures.
 *
 * @param shape The dimensions of the grid where the DLA pattern will be
 * generated. It is represented as a `Vec2<int>` object, where the first element
 * is the width and the second element is the height.
 * @param scale A scaling factor that influences the density of the
 * particles in the DLA pattern.
 * @param seeding_radius The radius within which initial seeding of particles
 * occurs. This radius defines the area where the first particles are placed.
 * @param seeding_outer_radius_ratio The ratio between the outer seeding radius
 * and the initial seeding radius. It determines the outer boundary for particle
 * seeding.
 * @param slope Slope of the talus added to the DLA pattern.
 * @param noise_ratio A parameter that controls the amount of randomness or
 * noise introduced in the talus formation process.
 * @param seed The seed for the random number generator, ensuring
 * reproducibility of the pattern. The same seed will generate the same pattern.
 *
 * @return A 2D array representing the generated DLA pattern. The array is of
 * the same size as specified by `shape`.
 *
 * **Example**
 * @include ex_diffusion_limited_aggregation.cpp
 *
 * **Result**
 * @image html ex_diffusion_limited_aggregation.png
 */
Array diffusion_limited_aggregation(Vec2<int> shape,
                                    float     scale,
                                    uint      seed,
                                    float     seeding_radius = 0.4f,
                                    float     seeding_outer_radius_ratio = 0.2f,
                                    float     slope = 8.f,
                                    float     noise_ratio = 0.2f);

/**
 * @brief Return a sparse Gabor noise.
 *
 * @param shape Array shape.
 * @param kw Kernel wavenumber, with respect to a unit domain.
 * @param angle Kernel angle (in degree).
 * @param width Kernel width (in pixels).
 * @param density Spot noise density.
 * @param seed Random seed number.
 * @return Array New array.
 *
 * **Example**
 * @include ex_gabor_noise.cpp
 *
 * **Result**
 * @image html ex_gabor_noise.png
 */
Array gabor_noise(Vec2<int> shape,
                  float     kw,
                  float     angle,
                  int       width,
                  float     density,
                  uint      seed);

/**
 * @brief Return a gaussian_decay pulse kernel.
 *
 * @param shape Array shape.
 * @param sigma Gaussian sigma (in pixels).
 *  @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the half-width parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param center Primitive reference center.
 * @param center Primitive reference center.
 * @param bbox Domain bounding box.
 * @return Array
 *
 * **Example**
 * @include ex_gaussian_pulse.cpp
 *
 * **Result**
 * @image html ex_gaussian_pulse.png
 */
Array gaussian_pulse(Vec2<int>   shape,
                     float       sigma,
                     Array      *p_ctrl_param = nullptr,
                     Array      *p_noise_x = nullptr,
                     Array      *p_noise_y = nullptr,
                     Array      *p_stretching = nullptr,
                     Vec2<float> center = {0.5f, 0.5f},
                     Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence noise.
 *
 * @param noise_type Noise type.
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_noise.cpp
 *
 * **Result**
 * @image html ex_noise.png
 */
Array noise(NoiseType   noise_type,
            Vec2<int>   shape,
            Vec2<float> kw,
            uint        seed,
            Array      *p_noise_x = nullptr,
            Array      *p_noise_y = nullptr,
            Array      *p_stretching = nullptr,
            Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence fbm noise.
 *
 * @param noise_type Noise type.
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param weigth Octave weighting.
 * @param persistence Octave persistence.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the weight parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_noise_fbm.cpp
 *
 * **Result**
 * @image html ex_noise_fbm0.png
 * @image html ex_noise_fbm1.png
 * @image html ex_noise_fbm2.png
 * @image html ex_noise_fbm3.png
 * @image html ex_noise_fbm4.png
 * @image html ex_noise_fbm5.png
 * @image html ex_noise_fbm6.png
 */
Array noise_fbm(NoiseType   noise_type,
                Vec2<int>   shape,
                Vec2<float> kw,
                uint        seed,
                int         octaves = 8,
                float       weight = 0.7f,
                float       persistence = 0.5f,
                float       lacunarity = 2.f,
                Array      *p_ctrl_param = nullptr,
                Array      *p_noise_x = nullptr,
                Array      *p_noise_y = nullptr,
                Array      *p_stretching = nullptr,
                Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence fbm noise.
 *
 * @param noise_type Noise type.
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param weigth Octave weighting.
 * @param persistence Octave persistence.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param gradient_scale Gradient scale.
 * @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the weight parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_noise_fbm.cpp
 *
 * **Result**
 * @image html ex_noise_fbm0.png
 * @image html ex_noise_fbm1.png
 * @image html ex_noise_fbm2.png
 * @image html ex_noise_fbm3.png
 * @image html ex_noise_fbm4.png
 * @image html ex_noise_fbm5.png
 * @image html ex_noise_fbm6.png
 */
Array noise_iq(NoiseType   noise_type,
               Vec2<int>   shape,
               Vec2<float> kw,
               uint        seed,
               int         octaves = 8,
               float       weight = 0.7f,
               float       persistence = 0.5f,
               float       lacunarity = 2.f,
               float       gradient_scale = 0.05f,
               Array      *p_ctrl_param = nullptr,
               Array      *p_noise_x = nullptr,
               Array      *p_noise_y = nullptr,
               Array      *p_stretching = nullptr,
               Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence fbm noise.
 *
 * @param noise_type Noise type.
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param weigth Octave weighting.
 * @param persistence Octave persistence.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param warp0 Initial warp scale.
 * @param damp0 Initial damp scale.
 * @param warp_scale Warp scale.
 * @param damp_scale Damp scale.
 * @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the weight parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_noise_fbm.cpp
 *
 * **Result**
 * @image html ex_noise_fbm0.png
 * @image html ex_noise_fbm1.png
 * @image html ex_noise_fbm2.png
 * @image html ex_noise_fbm3.png
 * @image html ex_noise_fbm4.png
 * @image html ex_noise_fbm5.png
 * @image html ex_noise_fbm6.png
 */
Array noise_jordan(NoiseType   noise_type,
                   Vec2<int>   shape,
                   Vec2<float> kw,
                   uint        seed,
                   int         octaves = 8,
                   float       weight = 0.7f,
                   float       persistence = 0.5f,
                   float       lacunarity = 2.f,
                   float       warp0 = 0.4f,
                   float       damp0 = 1.f,
                   float       warp_scale = 0.4f,
                   float       damp_scale = 1.f,
                   Array      *p_ctrl_param = nullptr,
                   Array      *p_noise_x = nullptr,
                   Array      *p_noise_y = nullptr,
                   Array      *p_stretching = nullptr,
                   Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherent fbm Parberry variant of Perlin
 * noise.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param weigth Octave weighting.
 * @param persistence Octave persistence.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param mu Gradient magnitude exponent.
 * @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the weight parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_noise_fbm.cpp
 *
 * **Result**
 * @image html ex_noise_fbm0.png
 * @image html ex_noise_fbm1.png
 * @image html ex_noise_fbm2.png
 * @image html ex_noise_fbm3.png
 * @image html ex_noise_fbm4.png
 * @image html ex_noise_fbm5.png
 * @image html ex_noise_fbm6.png
 */
Array noise_parberry(Vec2<int>   shape,
                     Vec2<float> kw,
                     uint        seed,
                     int         octaves = 8,
                     float       weight = 0.7f,
                     float       persistence = 0.5f,
                     float       lacunarity = 2.f,
                     float       mu = 1.02f,
                     Array      *p_ctrl_param = nullptr,
                     Array      *p_noise_x = nullptr,
                     Array      *p_noise_y = nullptr,
                     Array      *p_stretching = nullptr,
                     Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence fbm pingpong noise.
 *
 * @param noise_type Noise type.
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param weigth Octave weighting.
 * @param persistence Octave persistence.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the weight parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_noise_fbm.cpp
 *
 * **Result**
 * @image html ex_noise_fbm0.png
 * @image html ex_noise_fbm1.png
 * @image html ex_noise_fbm2.png
 * @image html ex_noise_fbm3.png
 * @image html ex_noise_fbm4.png
 * @image html ex_noise_fbm5.png
 * @image html ex_noise_fbm6.png
 */
Array noise_pingpong(NoiseType   noise_type,
                     Vec2<int>   shape,
                     Vec2<float> kw,
                     uint        seed,
                     int         octaves = 8,
                     float       weight = 0.7f,
                     float       persistence = 0.5f,
                     float       lacunarity = 2.f,
                     Array      *p_ctrl_param = nullptr,
                     Array      *p_noise_x = nullptr,
                     Array      *p_noise_y = nullptr,
                     Array      *p_stretching = nullptr,
                     Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence fbm ridged noise.
 *
 * @param noise_type Noise type.
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param weigth Octave weighting.
 * @param persistence Octave persistence.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param k_smoothing Smoothing parameter.
 * @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the weight parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_noise_fbm.cpp
 *
 * **Result**
 * @image html ex_noise_fbm0.png
 * @image html ex_noise_fbm1.png
 * @image html ex_noise_fbm2.png
 * @image html ex_noise_fbm3.png
 * @image html ex_noise_fbm4.png
 * @image html ex_noise_fbm5.png
 * @image html ex_noise_fbm6.png
 */
Array noise_ridged(NoiseType   noise_type,
                   Vec2<int>   shape,
                   Vec2<float> kw,
                   uint        seed,
                   int         octaves = 8,
                   float       weight = 0.7f,
                   float       persistence = 0.5f,
                   float       lacunarity = 2.f,
                   float       k_smoothing = 0.1f,
                   Array      *p_ctrl_param = nullptr,
                   Array      *p_noise_x = nullptr,
                   Array      *p_noise_y = nullptr,
                   Array      *p_stretching = nullptr,
                   Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence fbm swiss noise.
 *
 * @param noise_type Noise type.
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param weigth Octave weighting.
 * @param persistence Octave persistence.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param warp_scale Warp scale.
 * @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the weight parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_noise_fbm.cpp
 *
 * **Result**
 * @image html ex_noise_fbm0.png
 * @image html ex_noise_fbm1.png
 * @image html ex_noise_fbm2.png
 * @image html ex_noise_fbm3.png
 * @image html ex_noise_fbm4.png
 * @image html ex_noise_fbm5.png
 * @image html ex_noise_fbm6.png
 */
Array noise_swiss(NoiseType   noise_type,
                  Vec2<int>   shape,
                  Vec2<float> kw,
                  uint        seed,
                  int         octaves = 8,
                  float       weight = 0.7f,
                  float       persistence = 0.5f,
                  float       lacunarity = 2.f,
                  float       warp_scale = 0.1f,
                  Array      *p_ctrl_param = nullptr,
                  Array      *p_noise_x = nullptr,
                  Array      *p_noise_y = nullptr,
                  Array      *p_stretching = nullptr,
                  Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a paraboloid.
 * @param shape Array shape.
 * @param angle Rotation angle.
 * @param a Curvature parameter, first principal axis.
 * @param b Curvature parameter, second principal axis.
 * @param v0 Value at the paraboloid center.
 * @param reverse_x Reverse coefficient of first principal axis.
 * @param reverse_y Reverse coefficient of second principal axis.
 * @param p_base_elevation Reference to the control parameter array (acts as a
 * multiplier for the weight parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param center Primitive reference center.
 * @param bbox Domain bounding box.
 * @return Output array.
 *
 * **Example**
 * @include ex_paraboloid.cpp
 *
 * **Result**
 * @image html ex_paraboloid.png
 */
Array paraboloid(Vec2<int>   shape,
                 float       angle,
                 float       a,
                 float       b,
                 float       v0 = 0.f,
                 bool        reverse_x = false,
                 bool        reverse_y = false,
                 Array      *p_noise_x = nullptr,
                 Array      *p_noise_y = nullptr,
                 Array      *p_stretching = nullptr,
                 Vec2<float> center = {0.5f, 0.5f},
                 Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a peak-shaped heightmap.
 *
 * @param shape Array shape.
 * @param radius Peak outer radius.
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param noise_amp_r Radial noise absolute scale (in pixels).
 * @param noise_ratio_z Vertical noise relative scale (in [0, 1]).
 * @param bbox Domain bounding box.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_peak.cpp
 *
 * **Result**
 * @image html ex_peak.png
 */
Array peak(Vec2<int>   shape,
           float       radius,
           Array      *p_noise,
           float       noise_r_amp,
           float       noise_z_ratio,
           Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a rift function (Heaviside with an optional talus slope at
 * the transition).
 *
 * @param shape Array shape.
 * @param angle Overall rotation angle (in degree).
 * @param slope Step slope (assuming a unit domain).
 * @param width Rift width (assuming a unit domain).
 * @param sharp_bottom Decide whether the rift bottom is sharp or not.
 * @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the width parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local coordinate multiplier.
 * @param center Primitive reference center.
 * @param bbox Domain bounding box.
 * @return Array New array.
 *
 * **Example**
 * @include ex_rift.cpp
 *
 * **Result**
 * @image html ex_rift.png
 */
Array rift(Vec2<int>   shape,
           float       angle,
           float       slope,
           float       width,
           bool        sharp_bottom = false,
           Array      *p_ctrl_param = nullptr,
           Array      *p_noise_x = nullptr,
           Array      *p_noise_y = nullptr,
           Array      *p_stretching = nullptr,
           Vec2<float> center = {0.5f, 0.5f},
           Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array corresponding to a slope with a given overall.
 *
 * @param shape Array shape.
 * @param angle Overall rotation angle (in degree).
 * @param slope Slope (assuming a unit domain).
 * @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the slope parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local coordinate multiplier.
 * @param center Primitive reference center.
 * @param bbox Domain bounding box.
 * @return Array New array.
 *
 * **Example**
 * @include ex_slope.cpp
 *
 * **Result**
 * @image html ex_slope.png
 */
Array slope(Vec2<int>   shape,
            float       angle,
            float       slope,
            Array      *p_ctrl_param = nullptr,
            Array      *p_noise_x = nullptr,
            Array      *p_noise_y = nullptr,
            Array      *p_stretching = nullptr,
            Vec2<float> center = {0.5f, 0.5f},
            Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a step function (Heaviside with an optional talus slope at
 * the transition).
 *
 * @param shape Array shape.
 * @param angle Overall rotation angle (in degree).
 * @param slope Step slope (assuming a unit domain).
 * @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the slope parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local coordinate multiplier.
 * @param center Primitive reference center.
 * @param bbox Domain bounding box.
 * @return Array New array.
 *
 * **Example**
 * @include ex_step.cpp
 *
 * **Result**
 * @image html ex_step.png
 */
Array step(Vec2<int>   shape,
           float       angle,
           float       slope,
           Array      *p_ctrl_param = nullptr,
           Array      *p_noise_x = nullptr,
           Array      *p_noise_y = nullptr,
           Array      *p_stretching = nullptr,
           Vec2<float> center = {0.5f, 0.5f},
           Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generate displacements `dx` and `dy` to apply a swirl effect to
 * another primitve.
 *
 * @param dx[out] 'x' displacement (unit domain scale).
 * @param dy[out] 'y' displacement (unit domain scale).
 * @param amplitude Displacement amplitude.
 * @param exponent Distance exponent.
 * @param p_noise eference to the input noise array.
 * @param bbox Domain bounding box.
 *
 * **Example**
 * @include ex_swirl.cpp
 *
 * **Result**
 * @image html ex_swirl.png
 */
void swirl(Array      &dx,
           Array      &dy,
           float       amplitude = 1.f,
           float       exponent = 1.f,
           Array      *p_noise = nullptr,
           Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a dune shape wave.
 *
 * @param shape Array shape.
 * @param kw Wavenumber with respect to a unit domain.
 * @param angle Overall rotation angle (in degree).
 * @param xtop Relative location of the top of the dune profile (in [0, 1]).
 * @param xbottom Relative location of the foot of the dune profile (in [0, 1]).
 * @param phase_shift Phase shift (in radians).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array New array.
 */
Array wave_dune(Vec2<int>   shape,
                float       kw,
                float       angle,
                float       xtop,
                float       xbottom,
                float       phase_shift = 0.f,
                Array      *p_noise_x = nullptr,
                Array      *p_noise_y = nullptr,
                Array      *p_stretching = nullptr,
                Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a sine wave.
 *
 * @param shape Array shape.
 * @param kw Wavenumber with respect to a unit domain.
 * @param angle Overall rotation angle (in degree).
 * @param phase_shift Phase shift (in radians).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array New array.
 *
 * **Example**
 * @include ex_wave.cpp
 *
 * **Result**
 * @image html ex_wave0.png
 * @image html ex_wave1.png
 */
Array wave_sine(Vec2<int>   shape,
                float       kw,
                float       angle,
                float       phase_shift = 0.f,
                Array      *p_noise_x = nullptr,
                Array      *p_noise_y = nullptr,
                Array      *p_stretching = nullptr,
                Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a square wave.
 *
 * @param shape Array shape.
 * @param kw Wavenumber with respect to a unit domain.
 * @param angle Overall rotation angle (in degree).
 * @param phase_shift Phase shift (in radians).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array New array.
 *
 * **Example**
 * @include ex_wave.cpp
 *
 * **Result**
 * @image html ex_wave0.png
 * @image html ex_wave1.png
 */
Array wave_square(Vec2<int>   shape,
                  float       kw,
                  float       angle,
                  float       phase_shift = 0.f,
                  Array      *p_noise_x = nullptr,
                  Array      *p_noise_y = nullptr,
                  Array      *p_stretching = nullptr,
                  Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a triangular wave.
 *
 * @param shape Array shape.
 * @param kw Wavenumber with respect to a unit domain.
 * @param angle Overall rotation angle (in degree).
 * @param slant_ratio Relative location of the triangle apex, in [0, 1].
 * @param phase_shift Phase shift (in radians).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array New array.
 *
 * **Example**
 * @include ex_wave.cpp
 *
 * **Result**
 * @image html ex_wave0.png
 * @image html ex_wave1.png
 */
Array wave_triangular(Vec2<int>   shape,
                      float       kw,
                      float       angle,
                      float       slant_ratio,
                      float       phase_shift = 0.f,
                      Array      *p_noise_x = nullptr,
                      Array      *p_noise_y = nullptr,
                      Array      *p_stretching = nullptr,
                      Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with white noise.
 *
 * @param shape Array shape.
 * @param a Lower bound of random distribution.
 * @param b Upper bound of random distribution.
 * @param seed Random number seed.
 * @return Array White noise.
 *
 * **Example**
 * @include ex_white.cpp
 *
 * **Result**
 * @image html ex_white.png
 *
 * @see {@link white_sparse}
 */
Array white(Vec2<int> shape, float a, float b, uint seed);

/**
 * @brief Return an array filled `1` with a probability based on a density
 * map.
 *
 * @param density_map Density map.
 * @param seed Random number seed.
 * @return Array New array.
 *
 * **Example**
 * @include ex_white_density_map.cpp
 *
 * **Result**
 * @image html ex_white_density_map.png
 */
Array white_density_map(const Array &density_map, uint seed);

/**
 * @brief Return an array sparsely filled with white noise.
 *
 * @param shape Array shape.
 * @param a Lower bound of random distribution.
 * @param b Upper bound of random distribution.
 * @param density Array filling density, in [0, 1]. If set to 1, the function
 * is equivalent to {@link white}.
 * @param seed Random number seed.
 * @return Array Sparse white noise.
 *
 * **Example**
 * @include ex_white_sparse.cpp
 *
 * **Result**
 * @image html ex_white_sparse.png
 *
 * @see {@link white}
 */
Array white_sparse(Vec2<int> shape, float a, float b, float density, uint seed);

/**
 * @brief Return an array sparsely filled with random 0 and 1.
 *
 * @param shape Array shape.
 * @param density Array filling density, in [0, 1]. If set to 1, the function
 * is equivalent to {@link white}.
 * @param seed Random number seed.
 * @return Array Sparse white noise.
 */
Array white_sparse_binary(Vec2<int> shape, float density, uint seed);

/**
 * @brief Return an array filled with the maximum of two Worley (cellular)
 * noises.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param ratio Amplitude ratio between each Worley noise.
 * @param k Transition smoothing parameter.
 * @param p_ctrl_param Reference to the control parameter array (acts as a
 * multiplier for the ratio parameter).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Noise.
 *
 * **Example**
 * @include ex_worley_double.cpp
 *
 * **Result**
 * @image html ex_worley_double.png
 */
Array worley_double(Vec2<int>   shape,
                    Vec2<float> kw,
                    uint        seed,
                    float       ratio = 0.5f,
                    float       k = 0.f,
                    Array      *p_ctrl_param = nullptr,
                    Array      *p_noise_x = nullptr,
                    Array      *p_noise_y = nullptr,
                    Array      *p_stretching = nullptr,
                    Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

} // namespace hmap
