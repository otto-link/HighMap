/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file point_sampling.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
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

enum PointSamplingMethod : int
{
  RND_RANDOM,
  RND_HALTON,
  RND_HAMMERSLEY,
  RND_LHS,
};

static std::map<std::string, int> point_sampling_method_as_string = {
    {"Random", PointSamplingMethod::RND_RANDOM},
    {"Halton sequence", PointSamplingMethod::RND_HALTON},
    {"Hammersley sequence", PointSamplingMethod::RND_HAMMERSLEY},
    {"Latin Hypercube Sampling", PointSamplingMethod::RND_LHS},
};

std::array<std::vector<float>, 2> random_points(
    size_t                     count,
    uint                       seed,
    const PointSamplingMethod &method = PointSamplingMethod::RND_RANDOM,
    const Vec4<float>         &bbox = {0.f, 1.f, 0.f, 1.f});

std::array<std::vector<float>, 2> random_points_density(
    size_t             count,
    const Array       &density,
    uint               seed,
    const Vec4<float> &bbox = {0.f, 1.f, 0.f, 1.f});

std::array<std::vector<float>, 2> random_points_distance(
    float              min_dist,
    uint               seed,
    const Vec4<float> &bbox = {0.f, 1.f, 0.f, 1.f});

std::array<std::vector<float>, 2> random_points_distance(
    float              min_dist,
    float              max_dist,
    const Array       &density,
    uint               seed,
    const Vec4<float> &bbox = {0.f, 1.f, 0.f, 1.f});

std::array<std::vector<float>, 2> random_points_jittered(
    size_t                   count,
    const hmap::Vec2<float> &jitter_amount,
    const hmap::Vec2<float> &stagger_ratio,
    uint                     seed,
    const Vec4<float>       &bbox = {0.f, 1.f, 0.f, 1.f});

} // namespace hmap