/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "delaunator-cpp.hpp"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/functions.hpp"
#include "highmap/interpolate2d.hpp"
#include "highmap/operator.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array interpolate2d(Vec2<int>                 shape,
                    const std::vector<float> &x,
                    const std::vector<float> &y,
                    const std::vector<float> &values,
                    InterpolationMethod2D     interpolation_method,
                    const Array              *p_noise_x,
                    const Array              *p_noise_y,
                    const Array              *p_stretching,
                    Vec4<float>               bbox)
{
  switch (interpolation_method)
  {
  case (InterpolationMethod2D::DELAUNAY):
    return interpolate2d_delaunay(shape,
                                  x,
                                  y,
                                  values,
                                  p_noise_x,
                                  p_noise_y,
                                  p_stretching,
                                  bbox);

  case (InterpolationMethod2D::NEAREST):
    return interpolate2d_nearest(shape,
                                 x,
                                 y,
                                 values,
                                 p_noise_x,
                                 p_noise_y,
                                 p_stretching,
                                 bbox);
  default: throw std::runtime_error("unknown 2D interpolation method");
  }
}

Array interpolate2d_nearest(Vec2<int>                 shape,
                            const std::vector<float> &x,
                            const std::vector<float> &y,
                            const std::vector<float> &values,
                            const Array              *p_noise_x,
                            const Array              *p_noise_y,
                            const Array              *p_stretching,
                            Vec4<float>               bbox)
{
  auto itp_fct = [&x, &y, &values](float x_, float y_, float)
  {
    float dmax = std::numeric_limits<float>::max();
    float value_;
    for (size_t k = 0; k < x.size(); k++)
    {
      float d = std::hypot(x_ - x[k], y_ - y[k]);
      if (d < dmax)
      {
        dmax = d;
        value_ = values[k];
      }
    }

    return value_;
  };

  Array array_out = Array(shape);

  fill_array_using_xy_function(array_out,
                               bbox,
                               nullptr,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               itp_fct);

  return array_out;
}

Array interpolate2d_delaunay(Vec2<int>                 shape,
                             const std::vector<float> &x,
                             const std::vector<float> &y,
                             const std::vector<float> &values,
                             const Array              *p_noise_x,
                             const Array              *p_noise_y,
                             const Array              *p_stretching,
                             Vec4<float>               bbox)
{
  // triangulate
  std::vector<float> coords(2 * x.size());

  for (size_t k = 0; k < x.size(); k++)
  {
    coords[2 * k] = x[k];
    coords[2 * k + 1] = y[k];
  }

  delaunator::Delaunator<float> d(coords);

  // compute and store triangles area
  std::vector<float> area(d.triangles.size());

  for (size_t k = 0; k < d.triangles.size(); k += 3)
  {
    int p0 = d.triangles[k];
    int p1 = d.triangles[k + 1];
    int p2 = d.triangles[k + 2];

    // true area
    area[k] = 0.5f * (-y[p1] * x[p2] + y[p0] * (-x[p1] + x[p2]) +
                      x[p0] * (y[p1] - y[p2]) + x[p1] * y[p2]);

    // but stored like this to avoid doing it at each evaluation while
    // interpolating
    area[k] = 1.f / (2.f * area[k]);
  }

  auto itp_fct = [&x, &y, &values, &d, &area](float x_, float y_, float)
  {
    // https://stackoverflow.com/questions/2049582

    // compute barycentric coordinates to find in which triangle the
    // point (x_, y_) is inside
    for (size_t k = 0; k < d.triangles.size(); k += 3)
    {
      int p0 = d.triangles[k];
      int p1 = d.triangles[k + 1];
      int p2 = d.triangles[k + 2];

      float s = area[k] * (y[p0] * x[p2] - x[p0] * y[p2] +
                           (y[p2] - y[p0]) * x_ + (x[p0] - x[p2]) * y_);
      float t = area[k] * (x[p0] * y[p1] - y[p0] * x[p1] +
                           (y[p0] - y[p1]) * x_ + (x[p1] - x[p0]) * y_);

      if (s >= 0.f && t >= 0.f && s + t <= 1.f)
        return values[p0] + s * (values[p1] - values[p0]) +
               t * (values[p2] - values[p0]);
    }

    return 0.f;
  };

  Array array_out = Array(shape);

  fill_array_using_xy_function(array_out,
                               bbox,
                               nullptr,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               itp_fct);

  return array_out;
}

} // namespace hmap
