/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/point_sampling.hpp"
#include "highmap/functions.hpp"

namespace hmap
{

auto helper_make_pointwise_function(const Array &array, const Vec4<float> &bbox)
{
  return [&array, &bbox](const ps::Point<float, 2> &p) -> float
  {
    float x = (p[0] - bbox.a) / (bbox.b - bbox.a);
    float y = (p[1] - bbox.c) / (bbox.d - bbox.c);

    x = std::clamp(x, 0.f, 1.f);
    y = std::clamp(y, 0.f, 1.f);

    float xn = x * (array.shape.x - 1);
    float yn = y * (array.shape.y - 1);

    int   i = static_cast<int>(xn);
    int   j = static_cast<int>(yn);
    float u = xn - i;
    float v = yn - j;

    return array.get_value_bilinear_at(i, j, u, v);
  };
}

std::array<std::pair<float, float>, 2> bbox_to_ranges2d(const Vec4<float> &bbox)
{
  std::array<std::pair<float, float>, 2> ranges = {
      std::make_pair(bbox.a, bbox.b),
      std::make_pair(bbox.c, bbox.d)};
  return ranges;
}

std::array<std::vector<float>, 2> random_points(
    size_t                     count,
    uint                       seed,
    const PointSamplingMethod &method,
    const Vec4<float>         &bbox)
{
  std::vector<ps::Point<float, 2>> points;
  auto                             ranges = bbox_to_ranges2d(bbox);

  switch (method)
  {
  case PointSamplingMethod::RND_RANDOM:
  {
    points = ps::random<float, 2>(count, ranges, seed);
  }
  break;
  //
  case PointSamplingMethod::RND_HALTON:
  {
    points = ps::halton<float, 2>(count, ranges, seed);
  }
  break;
  //
  case PointSamplingMethod::RND_HAMMERSLEY:
  {
    points = ps::hammersley<float, 2>(count, ranges, seed);
  }
  break;
  //
  case PointSamplingMethod::RND_LHS:
  {
    points = ps::latin_hypercube_sampling<float, 2>(count, ranges, seed);
  }
  break;
    //
  }

  return ps::split_by_dimension(points);
}

std::array<std::vector<float>, 2> random_points_density(size_t       count,
                                                        const Array &density,
                                                        uint         seed,
                                                        const Vec4<float> &bbox)
{
  auto ranges = bbox_to_ranges2d(bbox);
  auto density_fct = helper_make_pointwise_function(density, bbox);

  auto points = ps::rejection_sampling<float, 2>(count,
                                                 ranges,
                                                 density_fct,
                                                 seed);
  return ps::split_by_dimension(points);
}

std::array<std::vector<float>, 2> random_points_distance(
    float              min_dist,
    uint               seed,
    const Vec4<float> &bbox)
{
  auto ranges = bbox_to_ranges2d(bbox);

  // estimate a maximum count using the minimum distance
  size_t count = (size_t)(2.f * (bbox.b - bbox.a) / min_dist *
                          (bbox.d - bbox.c) / min_dist);

  auto points = ps::poisson_disk_sampling_uniform(count,
                                                  ranges,
                                                  min_dist,
                                                  seed);
  return ps::split_by_dimension(points);
}

std::array<std::vector<float>, 2> random_points_distance(
    float              min_dist,
    float              max_dist,
    const Array       &density,
    uint               seed,
    const Vec4<float> &bbox)
{
  auto ranges = bbox_to_ranges2d(bbox);

  // convert density [0, 1] to scale (when density = 0, enforce
  // max_dist, when density = 1, enforce min_dist)
  Array scale = (1.f - density) * (max_dist / min_dist - 1.f) + 1.f;
  auto  scale_fct = helper_make_pointwise_function(scale, bbox);

  // estimate a maximum count using the minimum distance
  size_t count = (size_t)(2.f * (bbox.b - bbox.a) / min_dist *
                          (bbox.d - bbox.c) / min_dist);

  auto points = ps::poisson_disk_sampling<float, 2>(count,
                                                    ranges,
                                                    min_dist,
                                                    scale_fct,
                                                    seed);
  return ps::split_by_dimension(points);
}

std::array<std::vector<float>, 2> random_points_jittered(
    size_t                   count,
    const hmap::Vec2<float> &jitter_amount,
    const hmap::Vec2<float> &stagger_ratio,
    uint                     seed,
    const Vec4<float>       &bbox)
{
  auto                 ranges = bbox_to_ranges2d(bbox);
  std::array<float, 2> jt = {jitter_amount.x, jitter_amount.y};
  std::array<float, 2> sr = {stagger_ratio.x, stagger_ratio.y};

  auto points = ps::jittered_grid(count, ranges, jt, sr, seed);
  return ps::split_by_dimension(points);
}

} // namespace hmap
