/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file primitives.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Core procedural primitives for heightmap generation, including noise
 * functions (Perlin, Gabor, Voronoi, Phasor), terrain features (hills, craters,
 * calderas, dunes), geometric shapes (disk, rectangle), and advanced patterns
 * (DLA, Dendry). Supports both CPU and GPU-accelerated generation for complex
 * terrain synthesis.
 *
 * @copyright Copyright (c) 2023
 */
#pragma once

#include "FastNoiseLite.h"

#include "highmap/array.hpp"
#include "highmap/functions.hpp"

#define HMAP_GRADIENT_OFFSET 0.001f

namespace hmap
{

enum VoronoiReturnType : int
{
  F1_SQRT,
  F1_SQUARED,
  F2_SQRT,
  F2_SQUARED,
  F1F2_SQRT,
  F1F2_SQUARED,
};

/**
 * @brief Return a 'biquadratic pulse'.
 *
 * @param  shape                Array shape.
 * @param  gain                 Gain (the higher, the steeper).
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the gain parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array Perlin billow noise.
 *
 * **Example**
 * @include ex_biquad_pulse.cpp
 *
 * **Result**
 * @image html ex_biquad_pulse.png
 */
Array biquad_pulse(Vec2<int>    shape,
                   float        gain = 1.f,
                   const Array *p_ctrl_param = nullptr,
                   const Array *p_noise_x = nullptr,
                   const Array *p_noise_y = nullptr,
                   const Array *p_stretching = nullptr,
                   Vec2<float>  center = {0.5f, 0.5f},
                   Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a bump.
 *
 * @param  shape                Array shape.
 * @param  gain                 Gain (the higher, the steeper the bump).
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the gain parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  center               Primitive reference center.
 * @param  bbox                 Domain bounding box.
 * @return                      Array Perlin billow noise.
 *
 * **Example**
 * @include ex_bump.cpp
 *
 * **Result**
 * @image html ex_bump.png
 */
Array bump(Vec2<int>    shape,
           float        gain = 1.f,
           const Array *p_ctrl_param = nullptr,
           const Array *p_noise_x = nullptr,
           const Array *p_noise_y = nullptr,
           const Array *p_stretching = nullptr,
           Vec2<float>  center = {0.5f, 0.5f},
           Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a caldera-shaped heightmap.
 *
 * @param  shape         Array shape.
 * @param  radius        Crater radius at the ridge.
 * @param  sigma_inner   Inner half-width.
 * @param  sigma_outer   Outer half-width.
 * @param  z_bottom      Bottom elevation (ridge is at elevation `1`).
 * @param  p_noise       Displacement noise.
 * @param  noise_amp_r   Radial noise absolute scale (in pixels).
 * @param  noise_ratio_z Vertical noise relative scale (in [0, 1]).
 * @param  center        Primitive reference center.
 * @param  bbox          Domain bounding box.
 * @return               Array Resulting array.
 *
 * **Example**
 * @include ex_caldera.cpp
 *
 * **Result**
 * @image html ex_caldera.png
 */
Array caldera(Vec2<int>    shape,
              float        radius,
              float        sigma_inner,
              float        sigma_outer,
              float        z_bottom,
              const Array *p_noise,
              float        noise_amp_r,
              float        noise_ratio_z,
              Vec2<float>  center = {0.5f, 0.5f},
              Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

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
 * @param  shape                Array shape.
 * @param  kw                   Noise wavenumber with respect to a unit domain.
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array New array.
 *
 * **Example**
 * @include ex_checkerboard.cpp
 *
 * **Result**
 * @image html ex_checkerboard.png
 */
Array checkerboard(Vec2<int>    shape,
                   Vec2<float>  kw,
                   const Array *p_noise_x = nullptr,
                   const Array *p_noise_y = nullptr,
                   const Array *p_stretching = nullptr,
                   Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a constant value array.
 *
 * @param  shape Array shape.
 * @param  value Filling value.
 * @return       Array New array.
 */
Array constant(Vec2<int> shape, float value = 0.f);

/**
 * @brief Return a crater-shaped heightmap.
 *
 * @param  shape                Array shape.
 * @param  radius               Crater radius.
 * @param  lip_decay            Ejecta lip decay.
 * @param  lip_height_ratio     Controls the ejecta lip relative height, in [0,
 *                              1].
 * @param  depth                Crater depth.
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the lip_height_ratio
 *                              parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  bbox                 Domain bounding box.
 * @return                      Array New array.
 *
 * **Example**
 * @include ex_crater.cpp
 *
 * **Result**
 * @image html ex_crater.png
 */
Array crater(Vec2<int>    shape,
             float        radius,
             float        depth,
             float        lip_decay,
             float        lip_height_ratio = 0.5f,
             const Array *p_ctrl_param = nullptr,
             const Array *p_noise_x = nullptr,
             const Array *p_noise_y = nullptr,
             Vec2<float>  center = {0.5f, 0.5f},
             Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Dendry is a locally computable procedural function that generates
 * branching patterns at various scales (see @cite Gaillard2019).
 *
 * @param  shape                       Array shape.
 * @param  kw                          Noise wavenumber with respect to a unit
 *                                     domain.
 * @param  seed                        Random seed number.
 * @param  control_array               Control array (can be of any shape,
 *                                     different from
 * `shape`).
 * @param  eps                         Epsilon used to bias the area where
 *                                     points are generated in cells.
 * @param  resolution                  Number of resolutions in the noise
 *                                     function.
 * @param  displacement                Maximum displacement of segments.
 * @param  primitives_resolution_steps Additional resolution steps in the
 *                                     ComputeColorPrimitives function.
 * @param  slope_power                 Additional parameter to control the
 *                                     variation of slope on terrains.
 * @param  noise_amplitude_proportion  Proportion of the amplitude of the
 *                                     control function as noise.
 * @param  add_control_function        Add control function to the output.
 * @param  control_function_overlap    Extent of the extension added at the
 *                                     domain frontiers of the control array.
 * @param  p_noise_x, p_noise_y        Reference to the input noise arrays.
 * @param  p_stretching                Local wavenumber multiplier.
 * @param  bbox                        Domain bounding box.
 * @return                             Array New array.
 *
 * **Example**
 * @include ex_dendry.cpp
 *
 * **Result**
 * @image html ex_dendry.png
 */
Array dendry(Vec2<int>    shape,
             Vec2<float>  kw,
             uint         seed,
             Array       &control_function,
             float        eps = 0.05,
             int          resolution = 1,
             float        displacement = 0.075,
             int          primitives_resolution_steps = 3,
             float        slope_power = 2.f,
             float        noise_amplitude_proportion = 0.01,
             bool         add_control_function = true,
             float        control_function_overlap = 0.5f,
             const Array *p_noise_x = nullptr,
             const Array *p_noise_y = nullptr,
             const Array *p_stretching = nullptr,
             Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f},
             int          subsampling = 1);

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
             const Array   *p_noise_x = nullptr,
             const Array   *p_noise_y = nullptr,
             const Array   *p_stretching = nullptr,
             Vec4<float>    bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generates a diffusion-limited aggregation (DLA) pattern.
 *
 * This function simulates the diffusion-limited aggregation process to generate
 * a pattern within a grid of specified dimensions. The DLA process models
 * particles that undergo a random walk until they stick to a seed, gradually
 * forming complex fractal structures.
 *
 * @param  shape                      The dimensions of the grid where the DLA
 *                                    pattern will be generated. It is
 *                                    represented as a `Vec2<int>` object, where
 *                                    the first element is the width and the
 *                                    second element is the height.
 * @param  scale                      A scaling factor that influences the
 *                                    density of the particles in the DLA
 *                                    pattern.
 * @param  seeding_radius             The radius within which initial seeding of
 *                                    particles occurs. This radius defines the
 *                                    area where the first particles are placed.
 * @param  seeding_outer_radius_ratio The ratio between the outer seeding radius
 *                                    and the initial seeding radius. It
 *                                    determines the outer boundary for particle
 *                                    seeding.
 * @param  slope                      Slope of the talus added to the DLA
 *                                    pattern.
 * @param  noise_ratio                A parameter that controls the amount of
 *                                    randomness or noise introduced in the
 *                                    talus formation process.
 * @param  seed                       The seed for the random number generator,
 *                                    ensuring reproducibility of the pattern.
 *                                    The same seed will generate the same
 *                                    pattern.
 *
 * @return                            A 2D array representing the generated DLA
 *                                    pattern. The array is of the same size as
 *                                    specified by `shape`.
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
 * @brief Generates a disk-shaped heightmap with optional modifications.
 *
 * This function creates a 2D array representing a disk shape with a specified
 * radius, slope, and other optional parameters such as control parameters,
 * noise, and stretching for additional customization.
 *
 * @param  shape        Dimensions of the output array (width, height).
 * @param  radius       Radius of the disk, in normalized coordinates (0.0 to
 *                      1.0).
 * @param  slope        Slope of the disk edge transition. A larger value makes
 *                      the edge transition sharper. Defaults to 1.0.
 * @param  p_ctrl_param Optional pointer to an `Array` controlling custom
 *                      parameters for the disk generation.
 * @param  p_noise_x    Optional pointer to an `Array` for adding noise in the
 *                      x-direction.
 * @param  p_noise_y    Optional pointer to an `Array` for adding noise in the
 *                      y-direction.
 * @param  p_stretching Optional pointer to an `Array` for stretching the disk
 *                      horizontally or vertically.
 * @param  center       Center of the disk in normalized coordinates (0.0 to
 *                      1.0). Defaults to {0.5, 0.5}.
 * @param  bbox         Bounding box for the disk in normalized coordinates
 *                     {x_min, x_max, y_min, y_max}. Defaults to {0.0, 1.0, 0.0,
 * 1.0}.
 *
 * @return              A 2D array representing the generated disk shape.
 *
 * * **Example**
 * @include ex_disk.cpp
 *
 * **Result**
 * @image html ex_disk.png
 */
Array disk(Vec2<int>    shape,
           float        radius,
           float        slope = 1.f,
           const Array *p_ctrl_param = nullptr,
           const Array *p_noise_x = nullptr,
           const Array *p_noise_y = nullptr,
           const Array *p_stretching = nullptr,
           Vec2<float>  center = {0.5f, 0.5f},
           Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a sparse Gabor noise.
 *
 * @param  shape   Array shape.
 * @param  kw      Kernel wavenumber, with respect to a unit domain.
 * @param  angle   Kernel angle (in degree).
 * @param  width   Kernel width (in pixels).
 * @param  density Spot noise density.
 * @param  seed    Random seed number.
 * @return         Array New array.
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
 * @param  shape                Array shape.
 * @param  sigma                Gaussian sigma (in pixels).
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the half-width parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  center               Primitive reference center.
 * @param  center               Primitive reference center.
 * @param  bbox                 Domain bounding box.
 * @return                      Array
 *
 * **Example**
 * @include ex_gaussian_pulse.cpp
 *
 * **Result**
 * @image html ex_gaussian_pulse.png
 */
Array gaussian_pulse(Vec2<int>    shape,
                     float        sigma,
                     const Array *p_ctrl_param = nullptr,
                     const Array *p_noise_x = nullptr,
                     const Array *p_noise_y = nullptr,
                     const Array *p_stretching = nullptr,
                     Vec2<float>  center = {0.5f, 0.5f},
                     Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence noise.
 *
 * @param  noise_type           Noise type.
 * @param  shape                Array shape.
 * @param  kw                   Noise wavenumbers {kx, ky} for each directions.
 * @param  seed                 Random seed number.
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array Resulting array.
 *
 * **Example**
 * @include ex_noise.cpp
 *
 * **Result**
 * @image html ex_noise.png
 */
Array noise(NoiseType    noise_type,
            Vec2<int>    shape,
            Vec2<float>  kw,
            uint         seed,
            const Array *p_noise_x = nullptr,
            const Array *p_noise_y = nullptr,
            const Array *p_stretching = nullptr,
            Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence fbm noise.
 *
 * @param  noise_type           Noise type.
 * @param  shape                Array shape.
 * @param  kw                   Noise wavenumbers {kx, ky} for each directions.
 * @param  seed                 Random seed number.
 * @param  octaves              Number of octaves.
 * @param  weigth               Octave weighting.
 * @param  persistence          Octave persistence.
 * @param  lacunarity           Defines the wavenumber ratio between each
 *                              octaves.
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the weight parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array Fractal noise.
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
Array noise_fbm(NoiseType    noise_type,
                Vec2<int>    shape,
                Vec2<float>  kw,
                uint         seed,
                int          octaves = 8,
                float        weight = 0.7f,
                float        persistence = 0.5f,
                float        lacunarity = 2.f,
                const Array *p_ctrl_param = nullptr,
                const Array *p_noise_x = nullptr,
                const Array *p_noise_y = nullptr,
                const Array *p_stretching = nullptr,
                Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence fbm noise.
 *
 * @param  noise_type           Noise type.
 * @param  shape                Array shape.
 * @param  kw                   Noise wavenumbers {kx, ky} for each directions.
 * @param  seed                 Random seed number.
 * @param  octaves              Number of octaves.
 * @param  weigth               Octave weighting.
 * @param  persistence          Octave persistence.
 * @param  lacunarity           Defines the wavenumber ratio between each
 *                              octaves.
 * @param  gradient_scale       Gradient scale.
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the weight parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array Fractal noise.
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
Array noise_iq(NoiseType    noise_type,
               Vec2<int>    shape,
               Vec2<float>  kw,
               uint         seed,
               int          octaves = 8,
               float        weight = 0.7f,
               float        persistence = 0.5f,
               float        lacunarity = 2.f,
               float        gradient_scale = 0.05f,
               const Array *p_ctrl_param = nullptr,
               const Array *p_noise_x = nullptr,
               const Array *p_noise_y = nullptr,
               const Array *p_stretching = nullptr,
               Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence fbm noise.
 *
 * @param  noise_type           Noise type.
 * @param  shape                Array shape.
 * @param  kw                   Noise wavenumbers {kx, ky} for each directions.
 * @param  seed                 Random seed number.
 * @param  octaves              Number of octaves.
 * @param  weigth               Octave weighting.
 * @param  persistence          Octave persistence.
 * @param  lacunarity           Defines the wavenumber ratio between each
 *                              octaves.
 * @param  warp0                Initial warp scale.
 * @param  damp0                Initial damp scale.
 * @param  warp_scale           Warp scale.
 * @param  damp_scale           Damp scale.
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the weight parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array Fractal noise.
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
Array noise_jordan(NoiseType    noise_type,
                   Vec2<int>    shape,
                   Vec2<float>  kw,
                   uint         seed,
                   int          octaves = 8,
                   float        weight = 0.7f,
                   float        persistence = 0.5f,
                   float        lacunarity = 2.f,
                   float        warp0 = 0.4f,
                   float        damp0 = 1.f,
                   float        warp_scale = 0.4f,
                   float        damp_scale = 1.f,
                   const Array *p_ctrl_param = nullptr,
                   const Array *p_noise_x = nullptr,
                   const Array *p_noise_y = nullptr,
                   const Array *p_stretching = nullptr,
                   Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherent fbm Parberry variant of Perlin
 * noise.
 *
 * @param  shape                Array shape.
 * @param  kw                   Noise wavenumbers {kx, ky} for each directions.
 * @param  seed                 Random seed number.
 * @param  octaves              Number of octaves.
 * @param  weigth               Octave weighting.
 * @param  persistence          Octave persistence.
 * @param  lacunarity           Defines the wavenumber ratio between each
 *                              octaves.
 * @param  mu                   Gradient magnitude exponent.
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the weight parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array Fractal noise.
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
Array noise_parberry(Vec2<int>    shape,
                     Vec2<float>  kw,
                     uint         seed,
                     int          octaves = 8,
                     float        weight = 0.7f,
                     float        persistence = 0.5f,
                     float        lacunarity = 2.f,
                     float        mu = 1.02f,
                     const Array *p_ctrl_param = nullptr,
                     const Array *p_noise_x = nullptr,
                     const Array *p_noise_y = nullptr,
                     const Array *p_stretching = nullptr,
                     Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence fbm pingpong noise.
 *
 * @param  noise_type           Noise type.
 * @param  shape                Array shape.
 * @param  kw                   Noise wavenumbers {kx, ky} for each directions.
 * @param  seed                 Random seed number.
 * @param  octaves              Number of octaves.
 * @param  weigth               Octave weighting.
 * @param  persistence          Octave persistence.
 * @param  lacunarity           Defines the wavenumber ratio between each
 *                              octaves.
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the weight parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array Fractal noise.
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
Array noise_pingpong(NoiseType    noise_type,
                     Vec2<int>    shape,
                     Vec2<float>  kw,
                     uint         seed,
                     int          octaves = 8,
                     float        weight = 0.7f,
                     float        persistence = 0.5f,
                     float        lacunarity = 2.f,
                     const Array *p_ctrl_param = nullptr,
                     const Array *p_noise_x = nullptr,
                     const Array *p_noise_y = nullptr,
                     const Array *p_stretching = nullptr,
                     Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence fbm ridged noise.
 *
 * @param  noise_type           Noise type.
 * @param  shape                Array shape.
 * @param  kw                   Noise wavenumbers {kx, ky} for each directions.
 * @param  seed                 Random seed number.
 * @param  octaves              Number of octaves.
 * @param  weigth               Octave weighting.
 * @param  persistence          Octave persistence.
 * @param  lacunarity           Defines the wavenumber ratio between each
 *                              octaves.
 * @param  k_smoothing          Smoothing parameter.
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the weight parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array Fractal noise.
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
Array noise_ridged(NoiseType    noise_type,
                   Vec2<int>    shape,
                   Vec2<float>  kw,
                   uint         seed,
                   int          octaves = 8,
                   float        weight = 0.7f,
                   float        persistence = 0.5f,
                   float        lacunarity = 2.f,
                   float        k_smoothing = 0.1f,
                   const Array *p_ctrl_param = nullptr,
                   const Array *p_noise_x = nullptr,
                   const Array *p_noise_y = nullptr,
                   const Array *p_stretching = nullptr,
                   Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence fbm swiss noise.
 *
 * @param  noise_type           Noise type.
 * @param  shape                Array shape.
 * @param  kw                   Noise wavenumbers {kx, ky} for each directions.
 * @param  seed                 Random seed number.
 * @param  octaves              Number of octaves.
 * @param  weigth               Octave weighting.
 * @param  persistence          Octave persistence.
 * @param  lacunarity           Defines the wavenumber ratio between each
 *                              octaves.
 * @param  warp_scale           Warp scale.
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the weight parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array Fractal noise.
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
Array noise_swiss(NoiseType    noise_type,
                  Vec2<int>    shape,
                  Vec2<float>  kw,
                  uint         seed,
                  int          octaves = 8,
                  float        weight = 0.7f,
                  float        persistence = 0.5f,
                  float        lacunarity = 2.f,
                  float        warp_scale = 0.1f,
                  const Array *p_ctrl_param = nullptr,
                  const Array *p_noise_x = nullptr,
                  const Array *p_noise_y = nullptr,
                  const Array *p_stretching = nullptr,
                  Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a paraboloid.
 * @param  shape                Array shape.
 * @param  angle                Rotation angle.
 * @param  a                    Curvature parameter, first principal axis.
 * @param  b                    Curvature parameter, second principal axis.
 * @param  v0                   Value at the paraboloid center.
 * @param  reverse_x            Reverse coefficient of first principal axis.
 * @param  reverse_y            Reverse coefficient of second principal axis.
 * @param  p_base_elevation     Reference to the control parameter array (acts
 *                              as a multiplier for the weight parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  center               Primitive reference center.
 * @param  bbox                 Domain bounding box.
 * @return                      Output array.
 *
 * **Example**
 * @include ex_paraboloid.cpp
 *
 * **Result**
 * @image html ex_paraboloid.png
 */
Array paraboloid(Vec2<int>    shape,
                 float        angle,
                 float        a,
                 float        b,
                 float        v0 = 0.f,
                 bool         reverse_x = false,
                 bool         reverse_y = false,
                 const Array *p_noise_x = nullptr,
                 const Array *p_noise_y = nullptr,
                 const Array *p_stretching = nullptr,
                 Vec2<float>  center = {0.5f, 0.5f},
                 Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a peak-shaped heightmap.
 *
 * @param  shape         Array shape.
 * @param  radius        Peak outer radius.
 * @param  p_noise       Reference to the input noise array used for domain
 *                       warping (NOT in pixels, with respect to a unit domain).
 * @param  noise_amp_r   Radial noise absolute scale (in pixels).
 * @param  noise_ratio_z Vertical noise relative scale (in [0, 1]).
 * @param  bbox          Domain bounding box.
 * @return               Array Resulting array.
 *
 * **Example**
 * @include ex_peak.cpp
 *
 * **Result**
 * @image html ex_peak.png
 */
Array peak(Vec2<int>    shape,
           float        radius,
           const Array *p_noise,
           float        noise_r_amp,
           float        noise_z_ratio,
           Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generates a phasor noise field based on a Gabor noise model and phase
 * profile.
 *
 * This function creates a phasor noise array using a Gabor kernel approach,
 * applying a specified phase profile, smoothing, and density settings. The
 * output is influenced by the shape, frequency, and various noise
 * characteristics, allowing fine control over the generated noise field.
 *
 * @param  phasor_profile     The phase profile to apply. Determines the type of
 *                            phasor function used (e.g., bulky cosine, peaky
 *                            cosine).
 * @param  shape              The dimensions of the output array as a 2D vector
 *                            (width x height).
 * @param  kw                 The wave number (frequency) of the Gabor kernel.
 * @param  angle              An array specifying the angle field for the Gabor
 *                            kernel orientation.
 * @param  seed               A seed value for the random number generator used
 *                            to create jittered spawn points for Gabor kernels.
 * @param  profile_delta      A parameter for adjusting the delta in the phase
 *                            profile function.
 * @param  density_factor     A scaling factor for the density of Gabor kernel
 *                            spawn points.
 * @param  kernel_width_ratio The ratio of the kernel width to the phase field
 *                            resolution.
 * @param  phase_smoothing    A factor for controlling the blending of the phase
 *                            profile. Larger values result in smoother
 *                            transitions.
 * @return                    An `Array` containing the generated phasor noise
 *                            field.
 *
 * @throws std::invalid_argumentIfaninvalid`phasor_profile`isprovided.
 *
 * @note If the kernel width is too small (less than 4), the function returns a
 * zeroed array.
 *
 * @details The function performs the following steps:
 * - Generates Gabor kernel spawn points using jittered random sampling.
 * - Constructs Gabor kernels based on the input angle field and applies them to
 * noise arrays.
 * - Computes a phase field from the Gabor noise using `atan2`.
 * - Applies the specified phase profile using the
 * `get_phasor_profile_function`.
 * - Smooths the phase field if `phase_smoothing` is greater than zero.
 *
 * **Example**
 * @include ex_phasor.cpp
 *
 * **Result**
 * @image html ex_phasor.png
 */
Array phasor(PhasorProfile phasor_profile,
             Vec2<int>     shape,
             float         kw,
             const Array  &angle,
             uint          seed,
             float         profile_delta = 0.1f,
             float         density_factor = 1.f,
             float         kernel_width_ratio = 2.f,
             float         phase_smoothing = 2.f);

/**
 * @brief Generates a fractal Brownian motion (fBm) noise field using layered
 * phasor profiles.
 *
 * @param  phasor_profile     The phase profile to apply for each noise layer
 *                            (e.g., bulky cosine, peaky cosine).
 * @param  shape              The dimensions of the output array as a 2D vector
 *                            (width x height).
 * @param  kw                 The base wave number (frequency) for the first
 *                            noise layer.
 * @param  angle              An array specifying the angle field for the Gabor
 *                            kernel orientation in each layer.
 * @param  seed               A seed value for the random number generator used
 *                            in all noise layers.
 * @param  profile_delta      A parameter for adjusting the delta in the phase
 *                            profile function.
 * @param  density_factor     A scaling factor for the density of Gabor kernel
 *                            spawn points in each layer.
 * @param  kernel_width_ratio The ratio of the kernel width to the phase field
 *                            resolution.
 * @param  phase_smoothing    A factor for controlling the blending of the phase
 *                            profile. Larger values result in smoother
 *                            transitions.
 * @param  octaves            The number of noise layers (octaves) to generate.
 * @param  weight             A factor for controlling amplitude adjustments
 *                            based on the previous layer's values.
 * @param  persistence        A factor controlling how amplitude decreases
 *                            across successive octaves. Values <1 cause rapid
 *                            decay.
 * @param  lacunarity         A factor controlling how frequency increases
 *                            across successive octaves. Values >1 cause rapid
 *                            growth.
 * @return                    An `Array` containing the generated fBm noise
 *                            field.
 *
 * @throws std::invalid_argumentIfaninvalid`phasor_profile`isprovidedtothe
 *                            underlying `phasor` function.
 *
 * **Example**
 * @include ex_phasor.cpp
 *
 * **Result**
 * @image html ex_phasor.png
 */
Array phasor_fbm(PhasorProfile phasor_profile,
                 Vec2<int>     shape,
                 float         kw,
                 const Array  &angle,
                 uint          seed,
                 float         profile_delta = 0.1f,
                 float         density_factor = 1.f,
                 float         kernel_width_ratio = 2.f,
                 float         phase_smoothing = 2.f,
                 int           octaves = 8,
                 float         weight = 0.7f,
                 float         persistence = 0.5f,
                 float         lacunarity = 2.f);

/**
 * @brief Generates a rectangle-shaped heightmap with optional modifications.
 *
 * This function creates a 2D array representing a rectangle with specified
 * dimensions, rotation, and optional parameters for customization such as
 * control parameters, noise, and stretching.
 *
 * @param  shape        Dimensions of the output array (width, height).
 * @param  rx           Half-width of the rectangle, in normalized coordinates
 *                      (0.0 to 1.0).
 * @param  ry           Half-height of the rectangle, in normalized coordinates
 *                      (0.0 to 1.0).
 * @param  angle        Rotation angle of the rectangle in radians. Positive
 *                      values rotate counterclockwise.
 * @param  slope        Slope of the rectangle edge transition. A larger value
 *                      makes the edge transition sharper. Defaults to 1.0.
 * @param  p_ctrl_param Optional pointer to an `Array` controlling custom
 *                      parameters for the rectangle generation.
 * @param  p_noise_x    Optional pointer to an `Array` for adding noise in the
 *                      x-direction.
 * @param  p_noise_y    Optional pointer to an `Array` for adding noise in the
 *                      y-direction.
 * @param  p_stretching Optional pointer to an `Array` for stretching the
 *                      rectangle horizontally or vertically.
 * @param  center       Center of the rectangle in normalized coordinates (0.0
 *                      to 1.0). Defaults to {0.5, 0.5}.
 * @param  bbox         Bounding box for the rectangle in normalized coordinates
 *                     {x_min, x_max, y_min, y_max}. Defaults to {0.0, 1.0, 0.0,
 * 1.0}.
 *
 * @return              A 2D array representing the generated rectangle shape.
 *    *
 * **Example**
 * @include ex_rectangle.cpp
 *
 * **Result**
 * @image html ex_rectangle.png
 */
Array rectangle(Vec2<int>    shape,
                float        rx,
                float        ry,
                float        angle,
                float        slope = 1.f,
                const Array *p_ctrl_param = nullptr,
                const Array *p_noise_x = nullptr,
                const Array *p_noise_y = nullptr,
                const Array *p_stretching = nullptr,
                Vec2<float>  center = {0.5f, 0.5f},
                Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a rift function (Heaviside with an optional talus slope at the
 * transition).
 *
 * @param  shape                Array shape.
 * @param  angle                Overall rotation angle (in degree).
 * @param  slope                Step slope (assuming a unit domain).
 * @param  width                Rift width (assuming a unit domain).
 * @param  sharp_bottom         Decide whether the rift bottom is sharp or not.
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the width parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local coordinate multiplier.
 * @param  center               Primitive reference center.
 * @param  bbox                 Domain bounding box.
 * @return                      Array New array.
 *
 * **Example**
 * @include ex_rift.cpp
 *
 * **Result**
 * @image html ex_rift.png
 */
Array rift(Vec2<int>    shape,
           float        angle,
           float        slope,
           float        width,
           bool         sharp_bottom = false,
           const Array *p_ctrl_param = nullptr,
           const Array *p_noise_x = nullptr,
           const Array *p_noise_y = nullptr,
           const Array *p_stretching = nullptr,
           Vec2<float>  center = {0.5f, 0.5f},
           Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array corresponding to a slope with a given overall.
 *
 * @param  shape                Array shape.
 * @param  angle                Overall rotation angle (in degree).
 * @param  slope                Slope (assuming a unit domain).
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the slope parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local coordinate multiplier.
 * @param  center               Primitive reference center.
 * @param  bbox                 Domain bounding box.
 * @return                      Array New array.
 *
 * **Example**
 * @include ex_slope.cpp
 *
 * **Result**
 * @image html ex_slope.png
 */
Array slope(Vec2<int>    shape,
            float        angle,
            float        slope,
            const Array *p_ctrl_param = nullptr,
            const Array *p_noise_x = nullptr,
            const Array *p_noise_y = nullptr,
            const Array *p_stretching = nullptr,
            Vec2<float>  center = {0.5f, 0.5f},
            Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a step function (Heaviside with an optional talus slope at the
 * transition).
 *
 * @param  shape                Array shape.
 * @param  angle                Overall rotation angle (in degree).
 * @param  slope                Step slope (assuming a unit domain).
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the slope parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local coordinate multiplier.
 * @param  center               Primitive reference center.
 * @param  bbox                 Domain bounding box.
 * @return                      Array New array.
 *
 * **Example**
 * @include ex_step.cpp
 *
 * **Result**
 * @image html ex_step.png
 */
Array step(Vec2<int>    shape,
           float        angle,
           float        slope,
           const Array *p_ctrl_param = nullptr,
           const Array *p_noise_x = nullptr,
           const Array *p_noise_y = nullptr,
           const Array *p_stretching = nullptr,
           Vec2<float>  center = {0.5f, 0.5f},
           Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generate displacements `dx` and `dy` to apply a swirl effect to
 * another primitve.
 *
 * @param dx[out]   'x' displacement (unit domain scale).
 * @param dy[out]   'y' displacement (unit domain scale).
 * @param amplitude Displacement amplitude.
 * @param exponent  Distance exponent.
 * @param p_noise   eference to the input noise array.
 * @param bbox      Domain bounding box.
 *
 * **Example**
 * @include ex_swirl.cpp
 *
 * **Result**
 * @image html ex_swirl.png
 */
void swirl(Array       &dx,
           Array       &dy,
           float        amplitude = 1.f,
           float        exponent = 1.f,
           const Array *p_noise = nullptr,
           Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a dune shape wave.
 *
 * @param  shape                Array shape.
 * @param  kw                   Wavenumber with respect to a unit domain.
 * @param  angle                Overall rotation angle (in degree).
 * @param  xtop                 Relative location of the top of the dune profile
 *                              (in [0, 1]).
 * @param  xbottom              Relative location of the foot of the dune
 *                              profile (in [0, 1]).
 * @param  phase_shift          Phase shift (in radians).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array New array.
 */
Array wave_dune(Vec2<int>    shape,
                float        kw,
                float        angle,
                float        xtop,
                float        xbottom,
                float        phase_shift = 0.f,
                const Array *p_noise_x = nullptr,
                const Array *p_noise_y = nullptr,
                const Array *p_stretching = nullptr,
                Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a sine wave.
 *
 * @param  shape                Array shape.
 * @param  kw                   Wavenumber with respect to a unit domain.
 * @param  angle                Overall rotation angle (in degree).
 * @param  phase_shift          Phase shift (in radians).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array New array.
 *
 * **Example**
 * @include ex_wave.cpp
 *
 * **Result**
 * @image html ex_wave0.png
 * @image html ex_wave1.png
 */
Array wave_sine(Vec2<int>    shape,
                float        kw,
                float        angle,
                float        phase_shift = 0.f,
                const Array *p_noise_x = nullptr,
                const Array *p_noise_y = nullptr,
                const Array *p_stretching = nullptr,
                Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a square wave.
 *
 * @param  shape                Array shape.
 * @param  kw                   Wavenumber with respect to a unit domain.
 * @param  angle                Overall rotation angle (in degree).
 * @param  phase_shift          Phase shift (in radians).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array New array.
 *
 * **Example**
 * @include ex_wave.cpp
 *
 * **Result**
 * @image html ex_wave0.png
 * @image html ex_wave1.png
 */
Array wave_square(Vec2<int>    shape,
                  float        kw,
                  float        angle,
                  float        phase_shift = 0.f,
                  const Array *p_noise_x = nullptr,
                  const Array *p_noise_y = nullptr,
                  const Array *p_stretching = nullptr,
                  Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a triangular wave.
 *
 * @param  shape                Array shape.
 * @param  kw                   Wavenumber with respect to a unit domain.
 * @param  angle                Overall rotation angle (in degree).
 * @param  slant_ratio          Relative location of the triangle apex, in [0,
 *                              1].
 * @param  phase_shift          Phase shift (in radians).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array New array.
 *
 * **Example**
 * @include ex_wave.cpp
 *
 * **Result**
 * @image html ex_wave0.png
 * @image html ex_wave1.png
 */
Array wave_triangular(Vec2<int>    shape,
                      float        kw,
                      float        angle,
                      float        slant_ratio,
                      float        phase_shift = 0.f,
                      const Array *p_noise_x = nullptr,
                      const Array *p_noise_y = nullptr,
                      const Array *p_stretching = nullptr,
                      Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with white noise.
 *
 * @param  shape Array shape.
 * @param  a     Lower bound of random distribution.
 * @param  b     Upper bound of random distribution.
 * @param  seed  Random number seed.
 * @return       Array White noise.
 *
 * **Example**
 * @include ex_white.cpp
 *
 * **Result**
 * @image html ex_white.png
 *
 * @see          {@link white_sparse}
 */
Array white(Vec2<int> shape, float a, float b, uint seed);

/**
 * @brief Return an array filled `1` with a probability based on a density map.
 *
 * @param  density_map Density map.
 * @param  seed        Random number seed.
 * @return             Array New array.
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
 * @param  shape   Array shape.
 * @param  a       Lower bound of random distribution.
 * @param  b       Upper bound of random distribution.
 * @param  density Array filling density, in [0, 1]. If set to 1, the function
 *                 is equivalent to {@link white}.
 * @param  seed    Random number seed.
 * @return         Array Sparse white noise.
 *
 * **Example**
 * @include ex_white_sparse.cpp
 *
 * **Result**
 * @image html ex_white_sparse.png
 *
 * @see            {@link white}
 */
Array white_sparse(Vec2<int> shape, float a, float b, float density, uint seed);

/**
 * @brief Return an array sparsely filled with random 0 and 1.
 *
 * @param  shape   Array shape.
 * @param  density Array filling density, in [0, 1]. If set to 1, the function
 *                 is equivalent to {@link white}.
 * @param  seed    Random number seed.
 * @return         Array Sparse white noise.
 */
Array white_sparse_binary(Vec2<int> shape, float density, uint seed);

/**
 * @brief Return an array filled with the maximum of two Worley (cellular)
 * noises.
 *
 * @param  shape                Array shape.
 * @param  kw                   Noise wavenumbers {kx, ky} for each directions,
 *                              with respect to a unit domain.
 * @param  seed                 Random seed number.
 * @param  ratio                Amplitude ratio between each Worley noise.
 * @param  k                    Transition smoothing parameter.
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the ratio parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  p_stretching         Local wavenumber multiplier.
 * @param  bbox                 Domain bounding box.
 * @return                      Array Noise.
 *
 * **Example**
 * @include ex_worley_double.cpp
 *
 * **Result**
 * @image html ex_worley_double.png
 */
Array worley_double(Vec2<int>    shape,
                    Vec2<float>  kw,
                    uint         seed,
                    float        ratio = 0.5f,
                    float        k = 0.f,
                    const Array *p_ctrl_param = nullptr,
                    const Array *p_noise_x = nullptr,
                    const Array *p_noise_y = nullptr,
                    const Array *p_stretching = nullptr,
                    Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

} // namespace hmap

namespace hmap::gpu
{

/**
 * @brief Return an array filled with coherence Gabor noise.
 *
 * @param  shape              Array shape.
 * @param  kw                 Noise wavenumbers {kx, ky} for each directions.
 * @param  seed               Random seed number.
 * @param  angle              Base orientation angle for the Gabor wavelets (in
 *                            radians). Defaults to 0.
 * @param  angle_spread_ratio Ratio that controls the spread of wave
 *                            orientations around the base angle. Defaults to 1.
 * @param  bbox               Domain bounding box.
 * @return                    Array Fractal noise.
 *
 * @note Taken from https://www.shadertoy.com/view/clGyWm
 *
 * @note Only available if OpenCL is enabled.
 *
 * **Example**
 * @include ex_gabor_wave.cpp
 *
 * **Result**
 * @image html ex_gabor_wave.png
 */
Array gabor_wave(Vec2<int>    shape,
                 Vec2<float>  kw,
                 uint         seed,
                 const Array &angle,
                 float        angle_spread_ratio = 1.f,
                 Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

Array gabor_wave(Vec2<int>   shape,
                 Vec2<float> kw,
                 uint        seed,
                 float       angle = 0.f,
                 float       angle_spread_ratio = 1.f,
                 Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence Gabor noise.
 *
 * @param  shape                Array shape.
 * @param  kw                   Noise wavenumbers {kx, ky} for each directions.
 * @param  seed                 Random seed number.
 * @param  angle                Base orientation angle for the Gabor wavelets
 *                              (in radians). Defaults to 0.
 * @param  angle_spread_ratio   Ratio that controls the spread of wave
 *                              orientations around the base angle. Defaults to
 *                              1.
 * @param  octaves              Number of octaves.
 * @param  weigth               Octave weighting.
 * @param  persistence          Octave persistence.
 * @param  lacunarity           Defines the wavenumber ratio between each
 *                              octaves.
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the weight parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  bbox                 Domain bounding box.
 * @return                      Array Fractal noise.
 *
 * @note Taken from https://www.shadertoy.com/view/clGyWm
 *
 * @note Only available if OpenCL is enabled.
 *
 * **Example**
 * @include ex_gabor_wave.cpp
 *
 * **Result**
 * @image html ex_gabor_wave.png
 */
Array gabor_wave_fbm(Vec2<int>    shape,
                     Vec2<float>  kw,
                     uint         seed,
                     const Array &angle,
                     float        angle_spread_ratio = 1.f,
                     int          octaves = 8,
                     float        weight = 0.7f,
                     float        persistence = 0.5f,
                     float        lacunarity = 2.f,
                     const Array *p_ctrl_param = nullptr,
                     const Array *p_noise_x = nullptr,
                     const Array *p_noise_y = nullptr,
                     Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

Array gabor_wave_fbm(Vec2<int>    shape,
                     Vec2<float>  kw,
                     uint         seed,
                     float        angle = 0.f,
                     float        angle_spread_ratio = 1.f,
                     int          octaves = 8,
                     float        weight = 0.7f,
                     float        persistence = 0.5f,
                     float        lacunarity = 2.f,
                     const Array *p_ctrl_param = nullptr,
                     const Array *p_noise_x = nullptr,
                     const Array *p_noise_y = nullptr,
                     Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generates a 2D array using the GavoroNoise algorithm, which is a
 * procedural noise technique for terrain generation and other applications.
 *
 * @param  shape           Dimensions of the output array.
 * @param  kw              Wave number vector controlling the noise frequency.
 * @param  seed            Seed value for random number generation.
 * @param  amplitude       Amplitude of the noise.
 * @param  kw_multiplier   Multiplier for wave numbers in the noise function.
 * @param  slope_strength  Strength of slope-based directional erosion in the
 *                         noise.
 * @param  branch_strength Strength of branch-like structures in the generated
 *                         noise.
 * @param  z_cut_min       Minimum cutoff for Z-value in the noise.
 * @param  z_cut_max       Maximum cutoff for Z-value in the noise.
 * @param  octaves         Number of octaves for fractal Brownian motion (fBm).
 * @param  persistence     Amplitude scaling factor between noise octaves.
 * @param  lacunarity      Frequency scaling factor between noise octaves.
 * @param  p_ctrl_param    Optional array for control parameters, can modify the
 *                         Z cutoff dynamically.
 * @param  p_noise_x       Optional array for X-axis noise perturbation.
 * @param  p_noise_y       Optional array for Y-axis noise perturbation.
 * @param  bbox            Bounding box for mapping grid coordinates to world
 *                         space.
 *
 * @return                 A 2D array containing the generated GavoroNoise
 *                         values.
 *
 * @note Taken from https://www.shadertoy.com/view/MtGcWh
 *
 * @note Only available if OpenCL is enabled.
 *
 * This function leverages an OpenCL kernel to compute the GavoroNoise values on
 * the GPU, allowing for efficient large-scale generation. The kernel applies a
 * combination of fractal Brownian motion (fBm), directional erosion, and other
 * procedural techniques to generate intricate noise patterns.
 *
 * The optional `p_ctrl_param`, `p_noise_x`, and `p_noise_y` buffers provide
 * additional flexibility for dynamically adjusting noise parameters and
 * perturbations.
 *
 * **Example**
 * @include ex_gavoronoise.cpp
 *
 * **Result**
 * @image html ex_gavoronoise.png
 */
Array gavoronoise(Vec2<int>    shape,
                  Vec2<float>  kw,
                  uint         seed,
                  const Array &angle,
                  float        amplitude = 0.05f,
                  float        angle_spread_ratio = 1.f,
                  Vec2<float>  kw_multiplier = {4.f, 4.f},
                  float        slope_strength = 1.f,
                  float        branch_strength = 2.f,
                  float        z_cut_min = 0.2f,
                  float        z_cut_max = 1.f,
                  int          octaves = 8,
                  float        persistence = 0.4f,
                  float        lacunarity = 2.f,
                  const Array *p_ctrl_param = nullptr,
                  const Array *p_noise_x = nullptr,
                  const Array *p_noise_y = nullptr,
                  Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

Array gavoronoise(Vec2<int>    shape,
                  Vec2<float>  kw,
                  uint         seed,
                  float        angle = 0.f,
                  float        amplitude = 0.05f,
                  float        angle_spread_ratio = 1.f,
                  Vec2<float>  kw_multiplier = {4.f, 4.f},
                  float        slope_strength = 1.f,
                  float        branch_strength = 2.f,
                  float        z_cut_min = 0.2f,
                  float        z_cut_max = 1.f,
                  int          octaves = 8,
                  float        persistence = 0.4f,
                  float        lacunarity = 2.f,
                  const Array *p_ctrl_param = nullptr,
                  const Array *p_noise_x = nullptr,
                  const Array *p_noise_y = nullptr,
                  Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

Array gavoronoise(const Array &base,
                  Vec2<float>  kw,
                  uint         seed,
                  float        amplitude = 0.05f,
                  Vec2<float>  kw_multiplier = {4.f, 4.f},
                  float        slope_strength = 1.f,
                  float        branch_strength = 2.f,
                  float        z_cut_min = 0.2f,
                  float        z_cut_max = 1.f,
                  int          octaves = 8,
                  float        persistence = 0.4f,
                  float        lacunarity = 2.f,
                  const Array *p_ctrl_param = nullptr,
                  const Array *p_noise_x = nullptr,
                  const Array *p_noise_y = nullptr,
                  Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generates a heightmap representing a radial mountain range.
 *
 * This function creates a heightmap that simulates a mountain range emanating
 * radially from a specified center. The mountain range is influenced by various
 * noise parameters and control attributes.
 *
 * @param  shape              The dimensions of the output heightmap as a 2D
 *                            vector.
 * @param  kw                 The wave numbers (frequency components) as a 2D
 *                            vector.
 * @param  seed               The seed for random noise generation.
 * @param  half_width         The half-width of the radial mountain range,
 *                            controlling its spread. Default is 0.2f.
 * @param  angle_spread_ratio The ratio controlling the angular spread of the
 *                            mountain range. Default is 0.5f.
 * @param  center             The center point of the radial mountain range as
 *                            normalized coordinates within [0, 1]. Default is
 *                           {0.5f, 0.5f}.
 * @param  octaves            The number of octaves for fractal noise
 *                            generation. Default is 8.
 * @param  weight             The initial weight for noise contribution. Default
 *                            is 0.7f.
 * @param  persistence        The amplitude scaling factor for subsequent noise
 *                            octaves. Default is 0.5f.
 * @param  lacunarity         The frequency scaling factor for subsequent noise
 *                            octaves. Default is 2.0f.
 * @param  p_ctrl_param       Optional pointer to an array of control parameters
 *                            influencing the terrain generation.
 * @param  p_noise_x          Optional pointer to a precomputed noise array for
 *                            the X-axis.
 * @param  p_noise_y          Optional pointer to a precomputed noise array for
 *                            the Y-axis.
 * @param  p_angle            Optional pointer to an array to output the angle.
 * @param  bbox               The bounding box of the output heightmap in
 *                            normalized coordinates [xmin, xmax, ymin, ymax].
 *                            Default is {0.0f, 1.0f, 0.0f, 1.0f}.
 *
 * @return                    Array The generated heightmap representing the
 *                            radial mountain range.
 *
 * @note Only available if OpenCL is enabled.
 *
 * **Example**
 * @include ex_mountain_range_radial.cpp
 *
 * **Result**
 * @image html ex_mountain_range_radial.png
 */
Array mountain_range_radial(Vec2<int>    shape,
                            Vec2<float>  kw,
                            uint         seed,
                            float        half_width = 0.2f,
                            float        angle_spread_ratio = 0.5f,
                            float        core_size_ratio = 1.f,
                            Vec2<float>  center = {0.5f, 0.5f},
                            int          octaves = 8,
                            float        weight = 0.7f,
                            float        persistence = 0.5f,
                            float        lacunarity = 2.f,
                            const Array *p_ctrl_param = nullptr,
                            const Array *p_noise_x = nullptr,
                            const Array *p_noise_y = nullptr,
                            const Array *p_angle = nullptr,
                            Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/*! @brief See hmap::noise */
Array noise(NoiseType    noise_type,
            Vec2<int>    shape,
            Vec2<float>  kw,
            uint         seed,
            const Array *p_noise_x = nullptr,
            const Array *p_noise_y = nullptr,
            const Array *p_stretching = nullptr,
            Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/*! @brief See hmap::noise_fbm */
Array noise_fbm(NoiseType    noise_type,
                Vec2<int>    shape,
                Vec2<float>  kw,
                uint         seed,
                int          octaves = 8,
                float        weight = 0.7f,
                float        persistence = 0.5f,
                float        lacunarity = 2.f,
                const Array *p_ctrl_param = nullptr,
                const Array *p_noise_x = nullptr,
                const Array *p_noise_y = nullptr,
                const Array *p_stretching = nullptr,
                Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generates a Voronoi diagram in a 2D array with configurable
 * properties.
 *
 * @param  shape        The dimensions of the output array as a 2D vector of
 *                      integers.
 * @param  kw           The frequency scale factors for the Voronoi cells, given
 *                      as a 2D vector of floats.
 * @param  seed         A seed value for random number generation, ensuring
 *                      reproducibility.
 * @param  jitter       (Optional) The amount of random variation in the
 *                      positions of Voronoi cell sites, given as a 2D vector of
 *                      floats. Defaults to {0.5f, 0.5f}.
 * @param  return_type  (Optional) The type of value to compute for the Voronoi
 *                      diagram. Defaults to `VoronoiReturnType::F1_SQUARED`.
 * @param  p_ctrl_param (Optional) A pointer to an `Array` used to control the
 *                      Voronoi computation. If nullptr, no control is applied.
 * @param  p_noise_x    (Optional) A pointer to an `Array` providing additional
 *                      noise in the x-direction for cell positions. If nullptr,
 *                      no x-noise is applied.
 * @param  p_noise_y    (Optional) A pointer to an `Array` providing additional
 *                      noise in the y-direction for cell positions. If nullptr,
 *                      no y-noise is applied.
 * @param  bbox         (Optional) The bounding box for the Voronoi computation,
 *                      given as a 4D vector of floats representing {min_x,
 *                      max_x, min_y, max_y}. Defaults to
 * {0.f, 1.f, 0.f, 1.f}.
 *
 * @return              A 2D array representing the generated Voronoi diagram.
 *
 * @note Only available if OpenCL is enabled.
 *
 * **Example**
 * @include ex_voronoi.cpp
 *
 * **Result**
 * @image html ex_voronoi.png
 */
Array voronoi(Vec2<int>         shape,
              Vec2<float>       kw,
              uint              seed,
              Vec2<float>       jitter = {0.5f, 0.5f},
              VoronoiReturnType return_type = VoronoiReturnType::F1_SQUARED,
              const Array      *p_ctrl_param = nullptr,
              const Array      *p_noise_x = nullptr,
              const Array      *p_noise_y = nullptr,
              Vec4<float>       bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generates a Voronoi diagram in a 2D array with configurable
 * properties.
 *
 * @param  shape        The dimensions of the output array as a 2D vector of
 *                      integers.
 * @param  kw           The frequency scale factors for the base Voronoi cells,
 *                      given as a 2D vector of floats.
 * @param  seed         A seed value for random number generation, ensuring
 *                      reproducibility.
 * @param  jitter       (Optional) The amount of random variation in the
 *                      positions of Voronoi cell sites, given as a 2D vector of
 *                      floats. Defaults to {0.5f, 0.5f}.
 * @param  return_type  (Optional) The type of value to compute for the Voronoi
 *                      diagram. Defaults to `VoronoiReturnType::F1_SQUARED`.
 * @param  octaves      (Optional) The number of layers (octaves) in the fractal
 *                      Brownian motion. Defaults to 8.
 * @param  weight       (Optional) The initial weight of the base layer in the
 *                      FBM computation. Defaults to 0.7f.
 * @param  persistence  (Optional) The persistence factor that controls the
 *                      amplitude reduction between octaves. Defaults to 0.5f.
 * @param  lacunarity   (Optional) The lacunarity factor that controls the
 *                      frequency increase between octaves. Defaults to 2.f.
 * @param  p_ctrl_param (Optional) A pointer to an `Array` used to control the
 *                      Voronoi computation. If nullptr, no control is applied.
 * @param  p_noise_x    (Optional) A pointer to an `Array` providing additional
 *                      noise in the x-direction for cell positions. If nullptr,
 *                      no x-noise is applied.
 * @param  p_noise_y    (Optional) A pointer to an `Array` providing additional
 *                      noise in the y-direction for cell positions. If nullptr,
 *                      no y-noise is applied.
 * @param  bbox         (Optional) The bounding box for the Voronoi computation,
 *                      given as a 4D vector of floats representing {min_x,
 *                      max_x, min_y, max_y}. Defaults to
 * {0.f, 1.f, 0.f, 1.f}.
 *
 * @return              A 2D array representing the generated Voronoi diagram.
 *
 * @note Only available if OpenCL is enabled.
 *
 * **Example**
 * @include ex_voronoi.cpp
 *
 * **Result**
 * @image html ex_voronoi.png
 */
Array voronoi_fbm(Vec2<int>         shape,
                  Vec2<float>       kw,
                  uint              seed,
                  Vec2<float>       jitter = {0.5f, 0.5f},
                  VoronoiReturnType return_type = VoronoiReturnType::F1_SQUARED,
                  int               octaves = 8,
                  float             weight = 0.7f,
                  float             persistence = 0.5f,
                  float             lacunarity = 2.f,
                  const Array      *p_ctrl_param = nullptr,
                  const Array      *p_noise_x = nullptr,
                  const Array      *p_noise_y = nullptr,
                  Vec4<float>       bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Computes the Voronoi edge distance.
 *
 * @param shape                The shape of the grid as a 2D vector (width,
 *                             height).
 * @param kw                   The weights for the Voronoi kernel as a 2D
 *                             vector.
 * @param seed                 The random seed used for generating Voronoi
 *                             points.
 * @param jitter               Optional parameter for controlling jitter in
 *                             Voronoi point placement (default is {0.5f,
 *                             0.5f}).
 * @param p_ctrl_param         Optional pointer to an Array specifying control
 *                             parameters for Voronoi grid jitter (default is
 *                             nullptr).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param bbox                 The bounding box for the Voronoi diagram as
 *                            {x_min, x_max, y_min, y_max} (default is {0.f,
 * 1.f, 0.f, 1.f}).
 *
 * @note Taken from https://www.shadertoy.com/view/llG3zy
 *
 * @note Only available if OpenCL is enabled.
 *
 * @note The resulting Array has the same dimensions as the input shape.
 */
Array voronoi_edge_distance(Vec2<int>    shape,
                            Vec2<float>  kw,
                            uint         seed,
                            Vec2<float>  jitter = {0.5f, 0.5f},
                            const Array *p_ctrl_param = nullptr,
                            const Array *p_noise_x = nullptr,
                            const Array *p_noise_y = nullptr,
                            Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generates a 2D Voronoi noise array.
 *
 * This function computes a Voronoi noise pattern based on the input parameters
 * and returns it as a 2D array. The noise is calculated in the OpenCL kernel
 * `noise_voronoise`, which uses a combination of hashing and smoothstep
 * functions to generate a weighted Voronoi noise field.
 *
 * @note Taken from https://www.shadertoy.com/view/Xd23Dh
 *
 * @note Only available if OpenCL is enabled.
 *
 * @param  shape                The dimensions of the 2D output array as a
 *                              vector (width and height).
 * @param  kw                   Wave numbers for scaling the noise pattern,
 *                              represented as a 2D vector.
 * @param  u_param              A control parameter for the noise, adjusting the
 *                              contribution of random offsets.
 * @param  v_param              A control parameter for the noise, affecting the
 *                              smoothness of the pattern.
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  seed                 A seed value for random number generation,
 *                              ensuring reproducibility.
 *
 * @return                      An `Array` object containing the generated 2D
 *                              Voronoi noise values.
 *
 * **Example**
 * @include ex_voronoise.cpp
 *
 * **Result**
 * @image html ex_voronoise.png
 */
Array voronoise(Vec2<int>    shape,
                Vec2<float>  kw,
                float        u_param,
                float        v_param,
                uint         seed,
                const Array *p_noise_x = nullptr,
                const Array *p_noise_y = nullptr,
                Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with coherence Voronoise.
 *
 * @param  shape                Array shape.
 * @param  kw                   Noise wavenumbers {kx, ky} for each directions.
 * @param  seed                 Random seed number.
 * @param  octaves              Number of octaves.
 * @param  weigth               Octave weighting.
 * @param  persistence          Octave persistence.
 * @param  lacunarity           Defines the wavenumber ratio between each
 *                              octaves.
 * @param  p_ctrl_param         Reference to the control parameter array (acts
 *                              as a multiplier for the weight parameter).
 * @param  p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param  bbox                 Domain bounding box.
 * @return                      Array Fractal noise.
 *
 * @note Taken from https://www.shadertoy.com/view/clGyWm
 *
 * @note Only available if OpenCL is enabled.
 *
 * **Example**
 * @include ex_voronoise.cpp
 *
 * **Result**
 * @image html ex_voronoise.png
 */
Array voronoise_fbm(Vec2<int>    shape,
                    Vec2<float>  kw,
                    float        u_param,
                    float        v_param,
                    uint         seed,
                    int          octaves = 8,
                    float        weight = 0.7f,
                    float        persistence = 0.5f,
                    float        lacunarity = 2.f,
                    const Array *p_ctrl_param = nullptr,
                    const Array *p_noise_x = nullptr,
                    const Array *p_noise_y = nullptr,
                    Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});
} // namespace hmap::gpu
