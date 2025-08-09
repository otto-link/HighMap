/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file point_sampling.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2025
 *
 * This software is distributed under the terms of the GNU General Public
 * License. The full license is in the file LICENSE, distributed with this
 * software.
 */
#pragma once

#include "point_sampler.hpp"

#include "highmap/geometry/point.hpp"

namespace hmap
{

/**
 * @brief Enumeration of point sampling methods.
 */
enum PointSamplingMethod : int
{
  RND_RANDOM,     ///< Purely random uniform sampling.
  RND_HALTON,     ///< Low-discrepancy Halton sequence sampling.
  RND_HAMMERSLEY, ///< Low-discrepancy Hammersley sequence sampling.
  RND_LHS,        ///< Latin Hypercube Sampling.
};

/**
 * @brief String-to-enum mapping for PointSamplingMethod.
 *
 * This map allows converting human-readable sampling method names into their
 * corresponding PointSamplingMethod values.
 */
static std::map<std::string, int> point_sampling_method_as_string = {
    {"Random", PointSamplingMethod::RND_RANDOM},
    {"Halton sequence", PointSamplingMethod::RND_HALTON},
    {"Hammersley sequence", PointSamplingMethod::RND_HAMMERSLEY},
    {"Latin Hypercube Sampling", PointSamplingMethod::RND_LHS},
};

/**
 * @brief Generates random 2D points within a bounding box using a sampling
 * method.
 *
 * @param  count  Number of points to generate.
 * @param  seed   Random number generator seed.
 * @param  method Sampling method to use. Defaults to
 *                PointSamplingMethod::RND_RANDOM.
 * @param  bbox   Bounding box in which to generate the points (a,b,c,d = xmin,
 *                xmax, ymin, ymax). Defaults to the unit square {0.f, 1.f, 0.f,
 *                1.f}.
 * @return        A pair of float vectors {x_coords, y_coords}.
 */
std::array<std::vector<float>, 2> random_points(
    size_t                     count,
    uint                       seed,
    const PointSamplingMethod &method = PointSamplingMethod::RND_RANDOM,
    const Vec4<float>         &bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generates random 2D points within a bounding box based on a density
 * map.
 *
 * Points are sampled with probability proportional to the density values.
 *
 * @param  count   Number of points to generate.
 * @param  density 2D array representing spatial density values.
 * @param  seed    Random number generator seed.
 * @param  bbox    Bounding box in which to generate the points (a,b,c,d = xmin,
 *                 xmax, ymin, ymax). Defaults to the unit square {0.f, 1.f,
 *                 0.f, 1.f}.
 * @return         A pair of float vectors {x_coords, y_coords}.
 */
std::array<std::vector<float>, 2> random_points_density(
    size_t             count,
    const Array       &density,
    uint               seed,
    const Vec4<float> &bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generates random 2D points with a minimum separation distance.
 *
 * Produces a blue-noise-like distribution where no two points are closer than
 * min_dist.
 *
 * @param  min_dist Minimum allowed distance between points.
 * @param  seed     Random number generator seed.
 * @param  bbox     Bounding box in which to generate the points (a,b,c,d =
 *                  xmin, xmax, ymin, ymax). Defaults to the unit square {0.f,
 *                  1.f, 0.f, 1.f}.
 * @return          A pair of float vectors {x_coords, y_coords}.
 */
std::array<std::vector<float>, 2> random_points_distance(
    float              min_dist,
    uint               seed,
    const Vec4<float> &bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generates random 2D points with distance constraints and a density
 * map.
 *
 * Points are separated by distances between min_dist and max_dist, and are
 * sampled proportionally to the given density map.
 *
 * @param  min_dist Minimum allowed distance between points.
 * @param  max_dist Maximum allowed distance between points.
 * @param  density  2D array representing spatial density values.
 * @param  seed     Random number generator seed.
 * @param  bbox     Bounding box in which to generate the points (a,b,c,d =
 *                  xmin, xmax, ymin, ymax). Defaults to the unit square {0.f,
 *                  1.f, 0.f, 1.f}.
 * @return          A pair of float vectors {x_coords, y_coords}.
 */
std::array<std::vector<float>, 2> random_points_distance(
    float              min_dist,
    float              max_dist,
    const Array       &density,
    uint               seed,
    const Vec4<float> &bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generates jittered grid-based 2D points.
 *
 * Points are placed on a regular grid, offset by jitter_amount, and optionally
 * staggered according to stagger_ratio.
 *
 * @param  count         Number of points to generate.
 * @param  jitter_amount Maximum jitter to apply in each axis (x, y).
 * @param  stagger_ratio Ratio of staggering between consecutive rows or
 *                       columns.
 * @param  seed          Random number generator seed.
 * @param  bbox          Bounding box in which to generate the points (a,b,c,d =
 * xmin, xmax, ymin, ymax). Defaults to the unit square {0.f, 1.f, 0.f, 1.f}.
 * @return               A pair of float vectors {x_coords, y_coords}.
 */
std::array<std::vector<float>, 2> random_points_jittered(
    size_t                   count,
    const hmap::Vec2<float> &jitter_amount,
    const hmap::Vec2<float> &stagger_ratio,
    uint                     seed,
    const Vec4<float>       &bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Remove grid points that are outside a given bounding box.
 * @param x     `x` coordinates.
 * @param y     `y` coordinates.
 * @param value Values at points.
 * @param bbox  Bounding box.
 */
void remove_points_outside_bbox(std::vector<float> &x,
                                std::vector<float> &y,
                                std::vector<float> &value,
                                Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Rescale coordinate (x, y) so that they fit in a unit-square box based
 * on a given initial bounding box.
 * @param x[in, out] `x` coordinates (output).
 * @param y[in, out] `y` coordinates (output).
 * @param bbox       Initial bounding box.
 */
void rescale_points_to_unit_square(std::vector<float> &x,
                                   std::vector<float> &y,
                                   Vec4<float>         bbox);

} // namespace hmap