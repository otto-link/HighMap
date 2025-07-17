/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry/grids.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/math.hpp"

namespace hmap
{

Array sdf_2d_polyline(const Path  &path,
                      Vec2<int>    shape,
                      Vec4<float>  bbox,
                      const Array *p_noise_x,
                      const Array *p_noise_y)
{
  if (path.get_npoints() < 2)
  {
    LOG_ERROR("at least 2 points needed in the Path to compute the SDF");
    return Array(shape);
  }

  Array sdf2(shape);

  // normalize path nodes coord to unit square
  std::vector<float> xp = path.get_x();
  std::vector<float> yp = path.get_y();

  // array cell coordinates
  std::vector<float> xg(shape.x);
  std::vector<float> yg(shape.y);
  grid_xy_vector(xg, yg, shape, bbox, false);

  // compute sdf
  for (int j = 0; j < shape.y; ++j)
    for (int i = 0; i < shape.x; ++i)
    {
      float dx = p_noise_x ? (*p_noise_x)(i, j) : 0.f;
      float dy = p_noise_y ? (*p_noise_y)(i, j) : 0.f;

      float x = xg[i] + dx;
      float y = yg[j] + dy;

      // distance
      float d = std::numeric_limits<float>::max();

      for (size_t k = 0; k < path.get_npoints() - 1; k++)
      {
        size_t      p = k + 1;
        Vec2<float> e = {xp[p] - xp[k], yp[p] - yp[k]};
        Vec2<float> w = {x - xp[k], y - yp[k]};
        float       t = std::clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
        Vec2<float> b = {w.x - e.x * t, w.y - e.y * t};
        d = std::min(d, dot(b, b));
      }
      sdf2(i, j) = std::sqrt(d);
    }

  return sdf2;
}

Array sdf_2d_polyline_bezier(const Path  &path,
                             Vec2<int>    shape,
                             Vec4<float>  bbox,
                             const Array *p_noise_x,
                             const Array *p_noise_y)
{
  if (path.get_npoints() < 3)
  {
    LOG_ERROR("at least 3 points needed in the Path to compute the SDF");
    return Array(shape);
  }

  Array sdf2(shape);

  // get points, repeat every 3 points to have a continuous Bezier
  // curve and add a point if the input size is even, to ensure start
  // and end points will be taken into account
  std::vector<float> x_input = path.get_x();
  std::vector<float> y_input = path.get_y();
  std::vector<float> xp;
  std::vector<float> yp;

  if (x_input.size() % 2 == 0)
  {
    // not just in the middle to avoid Bezier degeneration
    size_t k = x_input.size();
    float  new_x = lerp(x_input[k - 1], x_input[k - 2], 0.1f);
    float  new_y = lerp(y_input[k - 1], y_input[k - 2], 0.1f);

    x_input.insert(x_input.end() - 1, new_x);
    y_input.insert(y_input.end() - 1, new_y);
  }

  for (size_t k = 0; k < x_input.size() - 2; k += 2)
  {
    xp.push_back(x_input[k]);
    xp.push_back(x_input[k + 1]);
    xp.push_back(x_input[k + 2]);

    yp.push_back(y_input[k]);
    yp.push_back(y_input[k + 1]);
    yp.push_back(y_input[k + 2]);
  }

  // array cell coordinates
  std::vector<float> xg(shape.x);
  std::vector<float> yg(shape.y);
  grid_xy_vector(xg, yg, shape, bbox, false);

  // --- compute sdf

  // https://iquilezles.org/articles/distfunctions2d/
  for (int j = 0; j < shape.y; ++j)
    for (int i = 0; i < shape.x; ++i)
    {
      float dx = p_noise_x ? (*p_noise_x)(i, j) : 0.f;
      float dy = p_noise_y ? (*p_noise_y)(i, j) : 0.f;

      float x = xg[i] + dx;
      float y = yg[j] + dy;

      // --- distance

      float dmin = std::numeric_limits<float>::max();

      for (size_t i = 0; i < xp.size() - 1; i += 3)
      {
        int j = i + 1;
        int k = i + 2;

        float dist;

        Vec2<float> a = {xp[j] - xp[i], yp[j] - yp[i]};
        Vec2<float> b = {xp[i] - 2.f * xp[j] + xp[k],
                         yp[i] - 2.f * yp[j] + yp[k]};
        Vec2<float> c = {2.f * a.x, 2.f * a.y};
        Vec2<float> d = {xp[i] - x, yp[i] - y};

        float kk = 1.f / dot(b, b);
        float kx = kk * dot(a, b);
        float ky = kk * (2.f * dot(a, a) + dot(d, b)) / 3.f;
        float kz = kk * dot(d, a);
        float p = ky - kx * kx;
        float p3 = p * p * p;
        float q = kx * (2.f * kx * kx - 3.f * ky) + kz;
        float h = q * q + 4.f * p3;

        if (h >= 0.f)
        {
          h = std::sqrt(h);
          Vec2<float> xx = {0.5f * (h - q), 0.5f * (-h - q)};
          Vec2<float> uv = {
              std::copysign(1.f, xx.x) * std::pow(std::abs(xx.x), 1.f / 3.f),
              std::copysign(1.f, xx.y) * std::pow(std::abs(xx.y), 1.f / 3.f)};
          float t = std::clamp(uv.x + uv.y - kx, 0.f, 1.f);

          Vec2<float> dd = {d.x + (c.x + b.x * t) * t,
                            d.y + (c.y + b.y * t) * t};

          dist = dot(dd, dd);
        }
        else
        {
          float       zz = std::sqrt(-p);
          float       v = std::acos(q / (p * zz * 2.f)) / 3.f;
          float       m = std::cos(v);
          float       n = std::sin(v) * 1.732050808f;
          Vec3<float> tt = {std::clamp((m + m) * zz - kx, 0.f, 1.f),
                            std::clamp((-n - m) * zz - kx, 0.f, 1.f),
                            std::clamp((n - m) * zz - kx, 0.f, 1.f)};

          Vec2<float> dd1 = {d.x + (c.x + b.x * tt.x) * tt.x,
                             d.y + (c.y + b.y * tt.x) * tt.x};
          Vec2<float> dd2 = {d.x + (c.x + b.x * tt.y) * tt.y,
                             d.y + (c.y + b.y * tt.y) * tt.y};

          float dist1 = dot(dd1, dd1);
          float dist2 = dot(dd2, dd2);

          dist = std::min(dist1, dist2);
        }

        dmin = std::min(dmin, dist);
      }

      sdf2(i, j) = std::sqrt(dmin);
    }

  return sdf2;
}

} // namespace hmap
