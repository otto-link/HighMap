/**
 * @file noise.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-05-01
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

namespace hmap
{

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
 * @param offset Offset applied to the noise function to move its range at each
 * octaves. Increasing the offset allows the rough peaks to rise and the valley
 * areas to lower and become smoother. For offset = 0, the function returns the
 * standard fractal brownian motion noise.
 * @param shift Noise shift {xs, ys} for each directions, with respect to a unit
 * domain.
 * @return Array Fractal noise.
 *
 * **Example**
 * @include ex_fbm_perlin.cpp
 *
 * **Result**
 * @image html ex_fbm_perlin.png
 *
 */

Array fbm_perlin(std::vector<int>   shape,
                 std::vector<float> kw,
                 uint               seed,
                 int                octaves = 8,
                 float              persistence = 0.5f,
                 float              lacunarity = 2.f,
                 float              offset = 0.5f,
                 std::vector<float> shift = {0, 0});

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
 *
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
 *
 */
hmap::Array perlin(std::vector<int>   shape,
                   std::vector<float> kw,
                   uint               seed,
                   std::vector<float> shift = {0, 0});

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
 *
 */
Array white(std::vector<int> shape, float a, float b, uint seed);

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
 *
 */
Array white_sparse(std::vector<int> shape,
                   float            a,
                   float            b,
                   float            density,
                   uint             seed);

} // namespace hmap