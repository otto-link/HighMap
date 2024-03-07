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

#include "highmap/noise_function.hpp"
#include "highmap/vector.hpp"

#define HMAP_GRADIENT_OFFSET 0.001f

namespace hmap
{

// /**
//  * @brief Noise type.
//  */
// enum noise_type : int
// {
//   noise_simplex2 = FastNoiseLite::NoiseType_OpenSimplex2,   ///< OpenSimplex2
//   noise_simplex2s = FastNoiseLite::NoiseType_OpenSimplex2S, ///<
//   OpenSimplex2S noise_worley = FastNoiseLite::NoiseType_Cellular, ///< Worley
//   noise_perlin = FastNoiseLite::NoiseType_Perlin,           ///< Perlin
//   noise_value_cubic = FastNoiseLite::NoiseType_ValueCubic,  ///< Value
//   (cubic) noise_value = FastNoiseLite::NoiseType_Value              ///<
//   Value
// };

// /**
//  * @brief Fractal type.
//  */
// enum fractal_type : int
// {
//   fractal_none = FastNoiseLite::FractalType_None,     ///< No fractal
//   layering fractal_fbm = FastNoiseLite::FractalType_FBm,       ///< Fbm
//   layering fractal_ridged = FastNoiseLite::FractalType_Ridged, ///< Ridged
//   layering fractal_pingpong = FastNoiseLite::FractalType_PingPong, ///<
//   PingPong layering fractal_max, ///< Max fractal_min ///< Min
// };

/**
 * @brief Return a 'biquadratic pulse'.
 *
 * @param shape Array shape.
 * @param gain Gain (the higher, the steeper).
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
                   Array      *p_noise_x = nullptr,
                   Array      *p_noise_y = nullptr,
                   Array      *p_stretching = nullptr,
                   Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return a bump.
 *
 * @param shape Array shape.
 * @param gain Gain (the higher, the steeper the bump).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
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
           Array      *p_noise_x = nullptr,
           Array      *p_noise_y = nullptr,
           Array      *p_stretching = nullptr,
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
 * @param p_noise Displacement noise.
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
             Array      *p_noise = nullptr,
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
             Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

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
 * @brief Return an array filled with an hybrid multifractal Perlin noise
 * weighted by the gradient norm and/or elevation of each octave.
 *
 * https://iquilezles.org/articles/morenoise/
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param gradient_weight Gradient norm influence.
 * @param value_weight Elevation influence.
 * @param octaves Number of octaves.
 * @param weigth Octave weighting.
 * @param persistence Octave persistence.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_fbm_iq_perlin.cpp
 *
 * **Result**
 * @image html ex_fbm_iq_perlin.png
 */
Array fbm_iq_perlin(Vec2<int>   shape,
                    Vec2<float> kw,
                    uint        seed,
                    float       gradient_weight = 0.01f,
                    float       value_weight = 0.01f,
                    int         octaves = 8,
                    float       weight = 0.7f,
                    float       persistence = 0.5f,
                    float       lacunarity = 2.f,
                    Array      *p_noise_x = nullptr,
                    Array      *p_noise_y = nullptr,
                    Array      *p_stretching = nullptr,
                    Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with an hybrid multifractal Perlin noise.
 *
 * The function is just a wrapper based of the library <a
 * href=https://github.com/Auburn/FastNoiseLite>FastNoiseLite</a>, theoretical
 * details are available in the original paper of Musgrave et al. @cite
 * Musgrave1989.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param weigth Octave weighting.
 * @param persistence Octave persistence.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_fbm_perlin.cpp
 *
 * **Result**
 * @image html ex_fbm_perlin.png
 */
Array fbm_perlin(Vec2<int>   shape,
                 Vec2<float> kw,
                 uint        seed,
                 int         octaves = 8,
                 float       weight = 0.7f,
                 float       persistence = 0.5f,
                 float       lacunarity = 2.f,
                 Array      *p_noise_x = nullptr,
                 Array      *p_noise_y = nullptr,
                 Array      *p_stretching = nullptr,
                 Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with a pingpong hybrid multifractal Perlin
 * noise.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param weigth Octave weighting.
 * @param persistence Octave persistence.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 */
Array fbm_pingpong_perlin(Vec2<int>   shape,
                          Vec2<float> kw,
                          uint        seed,
                          int         octaves = 8,
                          float       weight = 0.7f,
                          float       persistence = 0.5f,
                          float       lacunarity = 2.f,
                          Array      *p_noise_x = nullptr,
                          Array      *p_noise_y = nullptr,
                          Array      *p_stretching = nullptr,
                          Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with a ridged hybrid multifractal Perlin
 * noise.
 *
 * The function is just a wrapper based of the library <a
 * href=https://github.com/Auburn/FastNoiseLite>FastNoiseLite</a>, theoretical
 * details are available in the original paper of Musgrave et al. @cite
 * Musgrave1989.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param weigth Octave weighting.
 * @param persistence Octave persistence.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param k_smoothing Smoothing parameter.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_ridged_perlin.cpp
 *
 * **Result**
 * @image html ex_ridged_perlin.png
 */
Array fbm_ridged_perlin(Vec2<int>   shape,
                        Vec2<float> kw,
                        uint        seed,
                        int         octaves = 8,
                        float       weight = 0.7f,
                        float       persistence = 0.5f,
                        float       lacunarity = 2.f,
                        float       k_smoothing = 0.2f,
                        Array      *p_noise_x = nullptr,
                        Array      *p_noise_y = nullptr,
                        Array      *p_stretching = nullptr,
                        Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with an hybrid multifractal OpenSimplex2 noise.
 *
 * The function is just a wrapper based of the library <a
 * href=https://github.com/Auburn/FastNoiseLite>FastNoiseLite</a>.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param weigth Octave weighting.
 * @param persistence Octave persistence.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_simplex.cpp
 *
 * **Result**
 * @image html ex_simplex.png
 */
Array fbm_simplex(Vec2<int>   shape,
                  Vec2<float> kw,
                  uint        seed,
                  int         octaves = 8,
                  float       weight = 0.7f,
                  float       persistence = 0.5f,
                  float       lacunarity = 2.f,
                  Array      *p_noise_x = nullptr,
                  Array      *p_noise_y = nullptr,
                  Array      *p_stretching = nullptr,
                  Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with a fractal Worley noise.
 *
 * The function is just a wrapper based of the library <a
 * href=https://github.com/Auburn/FastNoiseLite>FastNoiseLite</a>, theoretical
 * details are available in the original paper of Musgrave et al. @cite
 * Musgrave1989.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param weigth Octave weighting.
 * @param persistence Octave persistence.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_fbm_worley.cpp
 *
 * **Result**
 * @image html ex_fbm_worley.png
 * @image html ex_fbm_worley_double.png
 */
Array fbm_worley(Vec2<int>   shape,
                 Vec2<float> kw,
                 uint        seed,
                 int         octaves = 8,
                 float       weight = 0.7f,
                 float       persistence = 0.5f,
                 float       lacunarity = 2.f,
                 Array      *p_noise_x = nullptr,
                 Array      *p_noise_y = nullptr,
                 Array      *p_stretching = nullptr,
                 Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with a fractal double Worley noise.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param ratio Amplitude ratio between each Worley noise.
 * @param k Transition smoothing parameter.
 * @param octaves Number of octaves.
 * @param weigth Octave weighting.
 * @param persistence Octave persistence.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_fbm_worley.cpp
 *
 * **Result**
 * @image html ex_fbm_worley.png
 * @image html ex_fbm_worley_double.png
 */
Array fbm_worley_double(Vec2<int>   shape,
                        Vec2<float> kw,
                        uint        seed,
                        float       ratio = 0.5f,
                        float       k = 0.f,
                        int         octaves = 8,
                        float       weight = 0.7f,
                        float       persistence = 0.5f,
                        float       lacunarity = 2.f,
                        Array      *p_noise_x = nullptr,
                        Array      *p_noise_y = nullptr,
                        Array      *p_stretching = nullptr,
                        Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

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
 * @brief Return a gaussian pulse kernel.
 *
 * @param shape Array shape.
 * @param sigma Gaussian sigma (in pixels).
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param center Primitive reference center.
 * @param bbox Domain bounding box.
 * @return Array
 */
Array gaussian_pulse(Vec2<int>   shape,
                     float       sigma,
                     Array      *p_noise = nullptr,
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
 * @brief Return an array filled with Perlin noise.
 *
 * The function is just a wrapper based of the library <a
 * href=https://github.com/Auburn/FastNoiseLite>FastNoiseLite</a>, theoretical
 * details are available in the original paper of Perlin et al. @cite
 * Perlin1985.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Perlin noise.
 *
 * **Example**
 * @include ex_perlin.cpp
 *
 * **Result**
 * @image html ex_perlin.png
 *
 * @see {@link fbm_perlin}
 */
Array perlin(Vec2<int>   shape,
             Vec2<float> kw,
             uint        seed,
             Array      *p_noise_x = nullptr,
             Array      *p_noise_y = nullptr,
             Array      *p_stretching = nullptr,
             Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with Perlin "billow" noise.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Perlin billow noise.
 *
 * **Example**
 * @include ex_perlin.cpp
 *
 * **Result**
 * @image html ex_perlin.png
 */
Array perlin_billow(Vec2<int>   shape,
                    Vec2<float> kw,
                    uint        seed,
                    Array      *p_noise_x = nullptr,
                    Array      *p_noise_y = nullptr,
                    Array      *p_stretching = nullptr,
                    Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with Perlin noise with a clamping of lower
 * values.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param k Smoothing factor.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Perlin billow noise.
 *
 * **Example**
 * @include ex_perlin.cpp
 *
 * **Result**
 * @image html ex_perlin.png
 */
Array perlin_half(Vec2<int>   shape,
                  Vec2<float> kw,
                  uint        seed,
                  float       k = 0.5f,
                  Array      *p_noise_x = nullptr,
                  Array      *p_noise_y = nullptr,
                  Array      *p_stretching = nullptr,
                  Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with "mix" Perlin noise.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Perlin billow noise.
 */
Array perlin_mix(Vec2<int>   shape,
                 Vec2<float> kw,
                 uint        seed,
                 Array      *p_noise_x = nullptr,
                 Array      *p_noise_y = nullptr,
                 Array      *p_stretching = nullptr,
                 Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with OpenSimplex2 noise.
 *
 * The function is just a wrapper based of the library <a
 * href=https://github.com/Auburn/FastNoiseLite>FastNoiseLite</a>.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Noise.
 *
 * **Example**
 * @include ex_simplex.cpp
 *
 * **Result**
 * @image html ex_simplex.png
 */
Array simplex(Vec2<int>   shape,
              Vec2<float> kw,
              uint        seed,
              Array      *p_noise_x = nullptr,
              Array      *p_noise_y = nullptr,
              Array      *p_stretching = nullptr,
              Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array corresponding to a slope with a given overall.
 *
 * @param shape Array shape.
 * @param angle Overall rotation angle (in degree).
 * @param slope Slope (assuming a unit domain).
 * @param p_noise Reference to the input noise array.
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
 * @param p_noise Reference to the input noise array.
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
 * @brief Return an array filled with value noise.
 *
 * The function is just a wrapper based of the library <a
 * href=https://github.com/Auburn/FastNoiseLite>FastNoiseLite</a>.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Value noise.
 *
 * **Example**
 * @include ex_value_noise.cpp
 *
 * **Result**
 * @image html ex_value_noise.png
 */
Array value_noise(Vec2<int>   shape,
                  Vec2<float> kw,
                  uint        seed,
                  Array      *p_noise_x = nullptr,
                  Array      *p_noise_y = nullptr,
                  Array      *p_stretching = nullptr,
                  Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with value noise based on linear Delaunay
 * interpolation of a set of random points.
 *
 * @param shape Array shape.
 * @param kw Noise reference wavenumber.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Value noise.
 *
 * **Example**
 * @include ex_value_noise_delaunay.cpp
 *
 * **Result**
 * @image html ex_value_noise_delaunay.png
 */
Array value_noise_delaunay(Vec2<int>   shape,
                           Vec2<float> kw,
                           uint        seed,
                           Array      *p_noise_x = nullptr,
                           Array      *p_noise_y = nullptr,
                           Array      *p_stretching = nullptr,
                           Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with value noise based on linear
 * interpolation.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Value noise.
 *
 * **Example**
 * @include ex_value_noise_linear.cpp
 *
 * **Result**
 * @image html ex_value_noise_linear.png
 */
Array value_noise_linear(Vec2<int>   shape,
                         Vec2<float> kw,
                         uint        seed,
                         Array      *p_noise_x = nullptr,
                         Array      *p_noise_y = nullptr,
                         Array      *p_stretching = nullptr,
                         Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with value noise based on thinplate
 * interpolation of a set of random points.
 *
 * @param shape Array shape.
 * @param kw Noise reference wavenumber.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Value noise.
 *
 * **Example**
 * @include ex_value_noise_thinplate.cpp
 *
 * **Result**
 * @image html ex_value_noise_thinplate.png
 */
Array value_noise_thinplate(Vec2<int>   shape,
                            Vec2<float> kw,
                            uint        seed,
                            Array      *p_noise_x = nullptr,
                            Array      *p_noise_y = nullptr,
                            Array      *p_stretching = nullptr,
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
 * @brief Return an array filled with Worley (cellular) noise.
 *
 * The function is just a wrapper based of the library <a
 * href=https://github.com/Auburn/FastNoiseLite>FastNoiseLite</a>, theoretical
 * details are available in the original paper of Perlin et al. @cite
 * Worley1996.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Worley noise.
 *
 * **Example**
 * @include ex_worley.cpp
 *
 * **Result**
 * @image html ex_worley.png
 */
Array worley(Vec2<int>   shape,
             Vec2<float> kw,
             uint        seed,
             Array      *p_noise_x = nullptr,
             Array      *p_noise_y = nullptr,
             Array      *p_stretching = nullptr,
             Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

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
                    Array      *p_noise_x = nullptr,
                    Array      *p_noise_y = nullptr,
                    Array      *p_stretching = nullptr,
                    Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array filled with Worley (cellular) noise.
 *
 * The function is just a wrapper based of the library <a
 * href=https://github.com/Auburn/FastNoiseLite>FastNoiseLite</a>, theoretical
 * details are available in the original paper of Perlin et al. @cite
 * Worley1996.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Worley noise.
 *
 * **Example**
 * @include ex_worley.cpp
 *
 * **Result**
 * @image html ex_worley.png
 */
Array worley_value(Vec2<int>   shape,
                   Vec2<float> kw,
                   uint        seed,
                   Array      *p_noise_x = nullptr,
                   Array      *p_noise_y = nullptr,
                   Array      *p_stretching = nullptr,
                   Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

} // namespace hmap
