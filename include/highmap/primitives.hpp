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
#include <functional>

#include "highmap/vector.hpp"

namespace hmap
{

// TODO move somewhere else (e.g. private include)
void helper_get_noise(Array                             &array,
                      std::vector<float>                &x,
                      std::vector<float>                &y,
                      Array                             *p_noise_x,
                      Array                             *p_noise_y,
                      std::function<float(float, float)> noise_fct);

Array helper_get_noise(std::vector<float>                &x,
                       std::vector<float>                &y,
                       Array                             *p_noise_x,
                       Array                             *p_noise_y,
                       std::function<float(float, float)> noise_fct);

void helper_get_noise(Array                             &array,
                      std::vector<float>                &x,
                      std::vector<float>                &y,
                      Array                             *p_noise_x,
                      Array                             *p_noise_y,
                      Array                             *p_stretching,
                      std::function<float(float, float)> noise_fct);

/**
 * @brief Return an heightmap defined by a set of elevation values defined on a
 * very coarse grid of control points.
 *
 * @param shape Array shape.
 * @param values Elevation at the control points.
 * @param width_factor Factor applied to the half-width of the base Gaussian.
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
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
                     Vec2<float>                     shift = {0.f, 0.f},
                     Vec2<float>                     scale = {1.f, 1.f});

/**
 * @brief Return a bump.
 *
 * @param shape Array shape.
 * @param gain Gain (the higher, the steeper the bump).
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
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
           Vec2<float> shift = {0.f, 0.f},
           Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return a "bump field".
 *
 * @param shape Array shape.
 * @param kw Noise wavenumber with respect to a unit domain.
 * @param seed Random seed number.
 * @param shape_factor Shape factor (> 0). Larger values lead to flatter
 * bumps.
 * @return Array New array.
 *
 * **Example**
 * @include ex_bump_field.cpp
 *
 * **Result**
 * @image html ex_bump_field.png
 */
Array bump_field(Vec2<int> shape,
                 float     kw,
                 uint      seed,
                 float     shape_factor = 2.f);

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
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
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
              Vec2<float> shift = {0.f, 0.f},
              Vec2<float> scale = {1.f, 1.f});

Array caldera(Vec2<int>   shape,
              float       radius,
              float       sigma_inner,
              float       sigma_outer,
              float       z_bottom,
              Vec2<float> shift = {0.f, 0.f},
              Vec2<float> scale = {1.f, 1.f}); ///< @overload

/**
 * @brief Return a checkerboard heightmap.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumber with respect to a unit domain.
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
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
                   Vec2<float> shift = {0.f, 0.f},
                   Vec2<float> scale = {1.f, 1.f});

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
 * @param shift Shift {xs, ys} for each directions.
 * @param scale Domain scaling, in [0, 1].
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
             Vec2<float> shift = {0.f, 0.f},
             Vec2<float> scale = {1.f, 1.f});

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
 * @param persistence 'Persistence' is a multiplier that determines how
 * quickly the amplitude diminishes for each successive octave: choose
 * 'persistence' close to 0 for a smooth noise, and close 1 for a rougher
 * noise texture.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param weigth Octave weighting.
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param p_stretching Local wavenumber multiplier.
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
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
                 Vec2<float> shift = {0.f, 0.f},
                 Vec2<float> scale = {1.f, 1.f});

Array fbm_perlin_advanced(Vec2<int>          shape,
                          Vec2<float>        kw,
                          uint               seed,
                          std::vector<float> octave_amplitudes,
                          float              lacunarity = 2.f,
                          Array             *p_weight = nullptr,
                          Array             *p_noise_x = nullptr,
                          Array             *p_noise_y = nullptr,
                          Vec2<float>        shift = {0.f, 0.f},
                          Vec2<float>        scale = {1.f, 1.f});

/**
 * @brief Return an array filled with an hybrid multifractal Worley noise.
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
 * @param persistence 'Persistence' is a multiplier that determines how
 * quickly the amplitude diminishes for each successive octave: choose
 * 'persistence' close to 0 for a smooth noise, and close 1 for a rougher
 * noise texture.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param weigth Octave weighting.
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
 * @return Array Fractal noise.
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
                 Vec2<float> shift = {0.f, 0.f},
                 Vec2<float> scale = {1.f, 1.f});

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
 * @param shift Shift {xs, ys} for each directions, with respect to a unit
 * domain.
 * @return Array
 */
Array gaussian_pulse(Vec2<int>   shape,
                     float       sigma,
                     Array      *p_noise = nullptr,
                     Vec2<float> shift = {0.f, 0.f},
                     Vec2<float> scale = {1.f, 1.f});

/**
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param persistence 'Persistence' is a multiplier that determines how
 * quickly the amplitude diminishes for each successive octave: choose
 * 'persistence' close to 0 for a smooth noise, and close 1 for a rougher
 * noise texture.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param offset Offset applied to the noise function to move its range to [0,
 * 2].
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @return Array Factal noise
 *
 * **Example**
 * @include ex_hybrid_fbm_perlin.cpp
 *
 * **Result**
 * @image html ex_hybrid_fbm_perlin.png
 */
Array hybrid_fbm_perlin(Vec2<int>   shape,
                        Vec2<float> kw,
                        uint        seed,
                        int         octaves = 8,
                        float       persistence = 0.9f,
                        float       lacunarity = 2.f,
                        float       offset = 0.5f,
                        Vec2<float> shift = {0.f, 0.f});

/**
 * @brief Return an array filled with an product-multifractal Perlin noise.
 *
 * Multifractals do not combine noise functions additively, as the fBM does,
 * but multiplicatively. By this, the occurence of high frequencies depend on
 * the noise values of lower frequencies. Theoretical details are available in
 * the references @cite Ebert2002 and @cite Dachsbacher2006.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param persistence Amplitude factor for each octave.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param seed Random seed number.
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
 * @return Array
 *
 * **Example**
 * @include ex_multifractal_perlin.cpp
 *
 * **Result**
 * @image html ex_multifractal_perlin.png
 *
 * @see {@link fbm_perlin}
 */
Array multifractal_perlin(Vec2<int>   shape,
                          Vec2<float> kw,
                          uint        seed,
                          int         octaves = 8,
                          float       persistence = 0.5f,
                          float       lacunarity = 2.f,
                          float       offset = 1.f,
                          Vec2<float> shift = {0.f, 0.f},
                          Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return a peak-shaped heightmap.
 *
 * @param shape Array shape.
 * @param radius Peak outer radius.
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param noise_amp_r Radial noise absolute scale (in pixels).
 * @param noise_ratio_z Vertical noise relative scale (in [0, 1]).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
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
           Vec2<float> shift = {0.f, 0.f},
           Vec2<float> scale = {1.f, 1.f});

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
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param p_stretching Local wavenumber multiplier.
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
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
             Vec2<float> shift = {0.f, 0.f},
             Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return an array filled with Perlin "billow" noise.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
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
                    Vec2<float> shift = {0.f, 0.f},
                    Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return an array filled with "mix" Perlin noise.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param p_stretching Local wavenumber multiplier.
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
 * @return Array Perlin billow noise.
 */
Array perlin_mix(Vec2<int>   shape,
                 Vec2<float> kw,
                 uint        seed,
                 Array      *p_noise_x = nullptr,
                 Array      *p_noise_y = nullptr,
                 Vec2<float> shift = {0.f, 0.f},
                 Vec2<float> scale = {1.f, 1.f});

Array pingpong_perlin(Vec2<int>   shape,
                      Vec2<float> kw,
                      uint        seed,
                      int         octaves = 8,
                      float       weight = 0.7f,
                      float       persistence = 0.5f,
                      float       lacunarity = 2.f,
                      Array      *p_noise_x = nullptr,
                      Array      *p_noise_y = nullptr,
                      Array      *p_stretching = nullptr,
                      Vec2<float> shift = {0.f, 0.f},
                      Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return an array based on a plane equation.
 *
 * @todo TO BE DONE.
 *
 * @param shape Array shape.
 * @param talus Plane talus.
 * @param yaw_angle Plane yaw_angle.
 * @param xyz_center Reference point: the plane passes through this point,
 * which can be anywhere.
 * @return Array New array.
 *
 * **Example**
 * @include ex_plane.cpp
 *
 * **Result**
 * @image html ex_plane.png
 */
Array plane(Vec2<int>   shape,
            float       talus,
            float       yaw_angle,
            Vec3<float> xyz_center);

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
 * @param persistence 'Persistence' is a multiplier that determines how
 * quickly the amplitude diminishes for each successive octave: choose
 * 'persistence' close to 0 for a smooth noise, and close 1 for a rougher
 * noise texture.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param offset Offset applied to the noise function to move its range at
 * each octaves. Increasing the offset allows the rough peaks to rise and the
 * valley areas to lower and become smoother. For offset = 0, the function
 * returns the standard fractal brownian motion noise.
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param p_stretching Local wavenumber multiplier.
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_ridged_perlin.cpp
 *
 * **Result**
 * @image html ex_ridged_perlin.png
 */
Array ridged_perlin(Vec2<int>   shape,
                    Vec2<float> kw,
                    uint        seed,
                    int         octaves = 8,
                    float       weight = 0.7f,
                    float       persistence = 0.5f,
                    float       lacunarity = 2.f,
                    Array      *p_noise_x = nullptr,
                    Array      *p_noise_y = nullptr,
                    Array      *p_stretching = nullptr,
                    Vec2<float> shift = {0.f, 0.f},
                    Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return an array corresponding to a slope in the x direction.
 *
 * @param shape Array shape.
 * @param talus Slope talus.
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
 * @return Array New array.
 *
 * **Example**
 * @include ex_slope.cpp
 *
 * **Result**
 * @image html ex_slope.png
 */
Array slope_x(Vec2<int>   shape,
              float       talus,
              Array      *p_noise = nullptr,
              Vec2<float> shift = {0.f, 0.f},
              Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return an array corresponding to a slope in the y direction.
 *
 * @param shape Array shape.
 * @param talus Slope talus.
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
 * @return Array New array.
 *
 * **Example**
 * @include ex_slope.cpp
 *
 * **Result**
 * @image html ex_slope.png
 */
Array slope_y(Vec2<int>   shape,
              float       talus,
              Array      *p_noise = nullptr,
              Vec2<float> shift = {0.f, 0.f},
              Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return a step function (Heaviside with an optional talus slope at
 * the transition).
 *
 * @param shape Array shape.
 * @param angle Overall rotation angle (in degree).
 * @param talus Talus slope.
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param center Primitive reference center.
 * @param shift Shift {xs, ys} for each directions.
 * @param scale Domain scaling, in [0, 1].
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
           float       talus,
           Array      *p_noise = nullptr,
           Vec2<float> center = {0.5f, 0.5f},
           Vec2<float> shift = {0.f, 0.f},
           Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Generate displacements `dx` and `dy` to apply a swirl effect to
 * another primitve.
 *
 * @param dx[out] 'x' displacement (unit domain scale).
 * @param dy[out] 'y' displacement (unit domain scale).
 * @param amplitude Displacement amplitude.
 * @param exponent Distance exponent.
 * @param p_noise eference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param shift Shift {xs, ys} for each directions.
 * @param scale Domain scaling, in [0, 1].
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
           Vec2<float> shift = {0.f, 0.f},
           Vec2<float> scale = {1.f, 1.f});

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
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
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
                  Vec2<float> shift = {0.f, 0.f},
                  Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return an array filled with value noise based on linear Delaunay
 * interpolation of a set of random points.
 *
 * @param shape Array shape.
 * @param kw Noise reference wavenumber.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
 * @return Array Value noise.
 *
 * **Example**
 * @include ex_value_noise_delaunay.cpp
 *
 * **Result**
 * @image html ex_value_noise_delaunay.png
 */
Array value_noise_delaunay(Vec2<int>   shape,
                           float       kw,
                           uint        seed,
                           Array      *p_noise_x = nullptr,
                           Array      *p_noise_y = nullptr,
                           Vec2<float> shift = {0.f, 0.f},
                           Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return an array filled with value noise based on linear
 * interpolation.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
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
                         Vec2<float> shift = {0.f, 0.f},
                         Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return an array filled with value noise based on thinplate
 * interpolation of a set of random points.
 *
 * @param shape Array shape.
 * @param kw Noise reference wavenumber.
 * @param seed Random seed number.
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
 * @return Array Value noise.
 *
 * **Example**
 * @include ex_value_noise_thinplate.cpp
 *
 * **Result**
 * @image html ex_value_noise_thinplate.png
 */
Array value_noise_thinplate(Vec2<int>   shape,
                            float       kw,
                            uint        seed,
                            Array      *p_noise_x = nullptr,
                            Array      *p_noise_y = nullptr,
                            Vec2<float> shift = {0.f, 0.f},
                            Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return a dune shape wave.
 *
 * @param shape Array shape.
 * @param kw Wavenumber with respect to a unit domain.
 * @param angle Overall rotation angle (in degree).
 * @param xtop Relative location of the top of the dune profile (in [0, 1]).
 * @param xbottom Relative location of the foot of the dune profile (in [0, 1]).
 * @param phase_shift Phase shift (in radians).
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param shift Shift {xs, ys} for each directions.
 * @param scale Domain scaling, in [0, 1].
 * @return Array New array.
 */
Array wave_dune(Vec2<int>   shape,
                float       kw,
                float       angle,
                float       xtop,
                float       xbottom,
                float       phase_shift = 0.f,
                Array      *p_noise = nullptr,
                Vec2<float> shift = {0.f, 0.f},
                Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return a sine wave.
 *
 * @param shape Array shape.
 * @param kw Wavenumber with respect to a unit domain.
 * @param angle Overall rotation angle (in degree).
 * @param phase_shift Phase shift (in radians).
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param shift Shift {xs, ys} for each directions.
 * @param scale Domain scaling, in [0, 1].
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
                Array      *p_noise = nullptr,
                Vec2<float> shift = {0.f, 0.f},
                Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return a square wave.
 *
 * @param shape Array shape.
 * @param kw Wavenumber with respect to a unit domain.
 * @param angle Overall rotation angle (in degree).
 * @param phase_shift Phase shift (in radians).
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param shift Shift {xs, ys} for each directions.
 * @param scale Domain scaling, in [0, 1].
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
                  Array      *p_noise = nullptr,
                  Vec2<float> shift = {0.f, 0.f},
                  Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return a triangular wave.
 *
 * @param shape Array shape.
 * @param kw Wavenumber with respect to a unit domain.
 * @param angle Overall rotation angle (in degree).
 * @param slant_ratio Relative location of the triangle apex, in [0, 1].
 * @param phase_shift Phase shift (in radians).
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param shift Shift {xs, ys} for each directions.
 * @param scale Domain scaling, in [0, 1].
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
                      Array      *p_noise = nullptr,
                      Vec2<float> shift = {0.f, 0.f},
                      Vec2<float> scale = {1.f, 1.f});

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
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
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
             Vec2<float> shift = {0.f, 0.f},
             Vec2<float> scale = {1.f, 1.f});

/**
 * @brief Return an array filled with the maximum of two Worley (cellular)
 * noises.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param ratio Amplitude ratio between each Worley noise.
 * @param k
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
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
                    Vec2<float> shift = {0.f, 0.f},
                    Vec2<float> scale = {1.f, 1.f});

} // namespace hmap
