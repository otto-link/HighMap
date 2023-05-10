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
 * @brief Return an array filled with Perlin noise.
 *
 * The function is just a wrapper based of the library <a
 * href=https://github.com/Auburn/FastNoiseLite>FastNoiseLite</a>, theoretical
 * details are available in the original paper of Perlin et al. @cite
 * Perlin1985.
 *
 * @see {@link fbm_perlin}
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
 */
hmap::Array perlin(std::vector<int>   shape,
                   std::vector<float> kw,
                   uint               seed,
                   std::vector<float> shift = {0, 0});

/**
 * @brief Return an array filled with white noise.
 *
 * Usage example:
 * @include ex_white.cpp
 *
 * @see {@link white_sparse}
 *
 * @param shape Array shape.
 * @param a Lower bound of random distribution.
 * @param b Upper bound of random distribution.
 * @param seed Random number seed.
 * @return Array White noise.
 */
Array white(std::vector<int> shape, float a, float b, uint seed);

/**
 * @brief Return an array sparsely filled with white noise.
 *
 * @see {@link white}
 *
 * @param shape Array shape.
 * @param a Lower bound of random distribution.
 * @param b Upper bound of random distribution.
 * @param density Array filling density, in [0, 1]. If set to 1, the function is
 * equivalent to {@link white}.
 * @param seed Random number seed.
 * @return Array Sparse white noise.
 */
Array white_sparse(std::vector<int> shape,
                   float            a,
                   float            b,
                   float            density,
                   uint             seed);

} // namespace hmap