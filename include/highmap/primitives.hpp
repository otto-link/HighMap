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
Array biweight(std::vector<int> shape);

/**
 * @brief Return a "bump field".
 *
 * @param shape Array shape.
 * @param kw Noise wavenumber with respect to a unit domain.
 * @param seed Random seed number.
 * @param shape_factor Shape factor (> 0). Larger values lead to flatter bumps.
 * @return Array New array.
 *
 * **Example**
 * @include ex_bump_field.cpp
 *
 * **Result**
 * @image html ex_bump_field.png
 */
Array bump_field(std::vector<int> shape,
                 float            kw,
                 uint             seed,
                 float            shape_factor = 2.f);

/**
 * @brief Return a caldera-shaped heightmap.
 *
 * @param shape Array shape.
 * @param radius Crater radius at the ridge.
 * @param sigma_inner Inner half-width.
 * @param sigma_outer Outer half-width.
 * @param z_bottom Bottom elevation (ridge is at elevation `1`).
 * @param noise Displacement noise.
 * @param noise_amp_r Radial noise absolute scale (in pixels).
 * @param noise_ratio_z Vertical noise relative scale (in [0, 1]).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a unit
 * domain.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_caldera.cpp
 *
 * **Result**
 * @image html ex_caldera.png
 */
Array caldera(std::vector<int>   shape,
              float              radius,
              float              sigma_inner,
              float              sigma_outer,
              float              z_bottom,
              Array             &noise,
              float              noise_amp_r,
              float              noise_ratio_z,
              std::vector<float> shift = {0.f, 0.f});

Array caldera(std::vector<int>   shape,
              float              radius,
              float              sigma_inner,
              float              sigma_outer,
              float              z_bottom,
              std::vector<float> shift = {0.f, 0.f}); ///< @overload

/**
 * @brief Return a cone.
 *
 * Maximum value is 1.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array cone(std::vector<int> shape);

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
Array cone_smooth(std::vector<int> shape);

/**
 * @brief Return a constant value array.
 *
 * @param shape Array shape.
 * @param value Filling value.
 * @return Array New array.
 */
Array constant(std::vector<int> shape, float value = 0.f);

/**
 * @brief Return a crater-shaped heightmap.
 *
 * @param shape Array shape.
 * @param radius Crater radius.
 * @param lip_decay Ejecta lip decay.
 * @param lip_height_ratio Controls the ejecta lip relative height, in [0, 1].
 * @param depth Crater depth.
 * @param shift Shift {xs, ys} for each directions.
 * @return Array New array.
 *
 * **Example**
 * @include ex_crater.cpp
 *
 * **Result**
 * @image html ex_crater.png
 */
Array crater(std::vector<int>   shape,
             float              radius,
             float              depth,
             float              lip_decay,
             float              lip_height_ratio = 0.5f,
             std::vector<float> shift = {0.f, 0.f});

/**
 * @brief Return a cubic pulse kernel.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array cubic_pulse(std::vector<int> shape);

/**
 * @brief Return a disk foot-print.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array disk(std::vector<int> shape);

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
 * @param persistence 'Persistence' is a multiplier that determines how quickly
 * the amplitude diminishes for each successive octave: choose 'persistence'
 * close to 0 for a smooth noise, and close 1 for a rougher noise texture.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param weigth Octave weighting.
 * @param shift Noise shift {xs, ys} for each directions, with respect to a unit
 * domain.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_fbm_perlin.cpp
 *
 * **Result**
 * @image html ex_fbm_perlin.png
 */
Array fbm_perlin(std::vector<int>   shape,
                 std::vector<float> kw,
                 uint               seed,
                 int                octaves = 8,
                 float              weight = 0.7f,
                 float              persistence = 0.5f,
                 float              lacunarity = 2.f,
                 std::vector<float> shift = {0.f, 0.f});

Array fbm_perlin_advanced(std::vector<int>   shape,
                          std::vector<float> kw,
                          uint               seed,
                          int                octaves,
                          const Array       &weight,
                          float              persistence = 0.5f,
                          float              lacunarity = 2.f,
                          std::vector<float> shift = {0.f, 0.f});

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
 * @param persistence 'Persistence' is a multiplier that determines how quickly
 * the amplitude diminishes for each successive octave: choose 'persistence'
 * close to 0 for a smooth noise, and close 1 for a rougher noise texture.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param weigth Octave weighting.
 * @param shift Noise shift {xs, ys} for each directions, with respect to a unit
 * domain.
 * @return Array Fractal noise.
 */
Array fbm_worley(std::vector<int>   shape,
                 std::vector<float> kw,
                 uint               seed,
                 int                octaves = 8,
                 float              weight = 0.7f,
                 float              persistence = 0.5f,
                 float              lacunarity = 2.f,
                 std::vector<float> shift = {0.f, 0.f});

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
Array gabor(std::vector<int> shape,
            float            kw,
            float            angle,
            float            footprint_threshold = 0.05f);

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
Array gabor_noise(std::vector<int> shape,
                  float            kw,
                  float            angle,
                  int              width,
                  float            density,
                  uint             seed);

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
Array gaussian_pulse(std::vector<int>   shape,
                     float              sigma,
                     Array             *p_noise = nullptr,
                     std::vector<float> shift = {0.f, 0.f});

/**
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param persistence 'Persistence' is a multiplier that determines how quickly
 * the amplitude diminishes for each successive octave: choose 'persistence'
 * close to 0 for a smooth noise, and close 1 for a rougher noise texture.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param offset Offset applied to the noise function to move its range to [0,
 * 2].
 * @param shift Noise shift {xs, ys} for each directions, with respect to a unit
 * domain.
 * @return Array Factal noise
 *
 * **Example**
 * @include ex_hybrid_fbm_perlin.cpp
 *
 * **Result**
 * @image html ex_hybrid_fbm_perlin.png
 */
Array hybrid_fbm_perlin(std::vector<int>   shape,
                        std::vector<float> kw,
                        uint               seed,
                        int                octaves = 8,
                        float              persistence = 0.9f,
                        float              lacunarity = 2.f,
                        float              offset = 0.5f,
                        std::vector<float> shift = {0.f, 0.f});

/**
 * @brief Return an array filled with an product-multifractal Perlin noise.
 *
 * Multifractals do not combine noise functions additively, as the fBM does, but
 * multiplicatively. By this, the occurence of high frequencies depend on the
 * noise values of lower frequencies. Theoretical details are available in the
 * references @cite Ebert2002 and @cite Dachsbacher2006.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param persistence Amplitude factor for each octave.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param seed Random seed number.
 * @param shift Noise shift {xs, ys} for each directions, with respect to a unit
 * domain.
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
Array multifractal_perlin(std::vector<int>   shape,
                          std::vector<float> kw,
                          uint               seed,
                          int                octaves = 8,
                          float              persistence = 0.5f,
                          float              lacunarity = 2.f,
                          float              offset = 1.f,
                          std::vector<float> shift = {0, 0});

/**
 * @brief Return a peak-shaped heightmap.
 *
 * @param shape Array shape.
 * @param radius Peak outer radius.
 * @param noise Displacement noise.
 * @param noise_amp_r Radial noise absolute scale (in pixels).
 * @param noise_ratio_z Vertical noise relative scale (in [0, 1]).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a unit
 * domain.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_peak.cpp
 *
 * **Result**
 * @image html ex_peak.png
 */
Array peak(std::vector<int>   shape,
           float              radius,
           Array             &noise,
           float              noise_r_amp,
           float              noise_z_ratio,
           std::vector<float> shift = {0, 0});

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
 * @param shift Noise shift {xs, ys} for each directions, with respect to a unit
 * domain.
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
Array perlin(std::vector<int>   shape,
             std::vector<float> kw,
             uint               seed,
             std::vector<float> shift = {0, 0});

/**
 * @brief Return an array filled with Perlin "billow" noise.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param shift Noise shift {xs, ys} for each directions, with respect to a unit
 * domain.
 * @return Array Perlin billow noise.
 *
 * **Example**
 * @include ex_perlin.cpp
 *
 * **Result**
 * @image html ex_perlin.png
 *
 */
Array perlin_billow(std::vector<int>   shape,
                    std::vector<float> kw,
                    uint               seed,
                    std::vector<float> shift = {0, 0});

Array perlin_mix(std::vector<int>   shape,
                 std::vector<float> kw,
                 uint               seed,
                 std::vector<float> shift = {0, 0});

Array pingpong_perlin(std::vector<int>   shape,
                      std::vector<float> kw,
                      uint               seed,
                      int                octaves = 8,
                      float              weight = 0.7f,
                      float              persistence = 0.5f,
                      float              lacunarity = 2.f,
                      std::vector<float> shift = {0, 0});

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
Array plane(std::vector<int>   shape,
            float              talus,
            float              yaw_angle,
            std::vector<float> xyz_center);

/**
 * @brief Return an array filled with a ridged hybrid multifractal Perlin noise.
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
 * @param persistence 'Persistence' is a multiplier that determines how quickly
 * the amplitude diminishes for each successive octave: choose 'persistence'
 * close to 0 for a smooth noise, and close 1 for a rougher noise texture.
 * @param lacunarity Defines the wavenumber ratio between each octaves.
 * @param offset Offset applied to the noise function to move its range at each
 * octaves. Increasing the offset allows the rough peaks to rise and the valley
 * areas to lower and become smoother. For offset = 0, the function returns the
 * standard fractal brownian motion noise.
 * @param shift Noise shift {xs, ys} for each directions, with respect to a unit
 * domain.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_ridged_perlin.cpp
 *
 * **Result**
 * @image html ex_ridged_perlin.png
 */
Array ridged_perlin(std::vector<int>   shape,
                    std::vector<float> kw,
                    uint               seed,
                    int                octaves = 8,
                    float              weight = 0.7f,
                    float              persistence = 0.5f,
                    float              lacunarity = 2.f,
                    std::vector<float> shift = {0, 0});

/**
 * @brief Return a smooth cosine kernel.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array smooth_cosine(std::vector<int> shape);

/**
 * @brief Return a step function (Heaviside with an optional talus slope at the
 * transition).
 *
 * @param shape Array shape.
 * @param angle Overall rotation angle (in degree).
 * @param talus Talus slope.
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param shift Shift {xs, ys} for each directions.
 * @return Array New array.
 *
 * **Example**
 * @include ex_step.cpp
 *
 * **Result**
 * @image html ex_step.png
 */
Array step(std::vector<int>   shape,
           float              angle,
           float              talus,
           Array             *p_noise = nullptr,
           std::vector<float> shift = {0.f, 0.f});

/**
 * @brief Return a tricube kernel.
 *
 * See https://en.wikipedia.org/wiki/Kernel_%28statistics%29.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array tricube(std::vector<int> shape);

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
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a unit
 * domain.
 * @return Array Value noise.
 *
 * **Example**
 * @include ex_value_noise.cpp
 *
 * **Result**
 * @image html ex_value_noise.png
 */
Array value_noise(std::vector<int>   shape,
                  std::vector<float> kw,
                  uint               seed,
                  Array             *p_noise = nullptr,
                  std::vector<float> shift = {0.f, 0.f});

/**
 * @brief Return an array filled with value noise based on linear interpolation.
 *
 * @param shape Array shape.
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param shift Noise shift {xs, ys} for each directions, with respect to a unit
 * domain.
 * @return Array Value noise.
 *
 * **Example**
 * @include ex_value_noise_linear.cpp
 *
 * **Result**
 * @image html ex_value_noise_linear.png
 */
Array value_noise_linear(std::vector<int>   shape,
                         std::vector<float> kw,
                         uint               seed,
                         Array             *p_noise = nullptr,
                         std::vector<float> shift = {0.f, 0.f});

/**
 * @brief Return a sine wave.
 *
 * @param shape Array shape.
 * @param kw Wavenumber with respect to a unit domain.
 * @param angle Overall rotation angle (in degree).
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param shift Shift {xs, ys} for each directions.
 * @return Array New array.
 *
 * **Example**
 * @include ex_wave.cpp
 *
 * **Result**
 * @image html ex_wave0.png
 * @image html ex_wave1.png
 */
Array wave_sine(std::vector<int>   shape,
                float              kw,
                float              angle,
                Array             *p_noise = nullptr,
                std::vector<float> shift = {0.f, 0.f});

/**
 * @brief Return a square wave.
 *
 * @param shape Array shape.
 * @param kw Wavenumber with respect to a unit domain.
 * @param angle Overall rotation angle (in degree).
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param shift Shift {xs, ys} for each directions.
 * @return Array New array.
 *
 * **Example**
 * @include ex_wave.cpp
 *
 * **Result**
 * @image html ex_wave0.png
 * @image html ex_wave1.png
 */
Array wave_square(std::vector<int>   shape,
                  float              kw,
                  float              angle,
                  Array             *p_noise = nullptr,
                  std::vector<float> shift = {0.f, 0.f});

/**
 * @brief Return a triangular wave.
 *
 * @param shape Array shape.
 * @param kw Wavenumber with respect to a unit domain.
 * @param angle Overall rotation angle (in degree).
 * @param slant_ratio Relative location of the triangle apex, in [0, 1].
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param shift Shift {xs, ys} for each directions.
 * @return Array New array.
 *
 * **Example**
 * @include ex_wave.cpp
 *
 * **Result**
 * @image html ex_wave0.png
 * @image html ex_wave1.png
 */
Array wave_triangular(std::vector<int>   shape,
                      float              kw,
                      float              angle,
                      float              slant_ratio,
                      Array             *p_noise = nullptr,
                      std::vector<float> shift = {0.f, 0.f});

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
Array white(std::vector<int> shape, float a, float b, uint seed);

/**
 * @brief Return an array filled `1` with a probability based on a density map.
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
 * @param density Array filling density, in [0, 1]. If set to 1, the function is
 * equivalent to {@link white}.
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
Array white_sparse(std::vector<int> shape,
                   float            a,
                   float            b,
                   float            density,
                   uint             seed);

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
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param shift Noise shift {xs, ys} for each directions, with respect to a unit
 * domain.
 * @return Array Worley noise.
 *
 * **Example**
 * @include ex_worley.cpp
 *
 * **Result**
 * @image html ex_worley.png
 */
Array worley(std::vector<int>   shape,
             std::vector<float> kw,
             uint               seed,
             Array             *p_noise = nullptr,
             std::vector<float> shift = {0.f, 0.f});

} // namespace hmap