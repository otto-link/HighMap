/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/grids.hpp"
#include "highmap/math.hpp"
#include "highmap/operator.hpp"
#include "highmap/range.hpp"

namespace hmap
{

Array ridgelines(Vec2<int>                 shape,
                 const std::vector<float> &xr,
                 const std::vector<float> &yr,
                 const std::vector<float> &zr,
                 float                     slope,
                 float                     k_smoothing,
                 float                     width,
                 float                     vmin,
                 Vec4<float>               bbox,
                 const Array              *p_noise_x,
                 const Array              *p_noise_y,
                 const Array              *p_stretching,
                 Vec4<float>               bbox_array)
{
  // normalized node coordinates
  std::vector<float> xrs = xr;
  std::vector<float> yrs = yr;
  rescale_points_to_unit_square(xrs, yrs, bbox);

  // define noise function
  std::function<float(float, float, float)> lambda;

  if (slope > 0.f)
    lambda = [&xrs, &yrs, &zr, &vmin, &slope, &k_smoothing, &width](float x_,
                                                                    float y_,
                                                                    float)
    {
      float d = -std::numeric_limits<float>::max();
      for (size_t i = 0; i < xrs.size() - 1; i += 2)
      {
        int         j = i + 1;
        Vec2<float> e = {xrs[j] - xrs[i], yrs[j] - yrs[i]};
        Vec2<float> w = {x_ - xrs[i], y_ - yrs[i]};
        float       coeff = std::clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
        Vec2<float> b = {w.x - coeff * e.x, w.y - coeff * e.y};

        float dist = std::sqrt(dot(b, b));
        if (dist <= width) dist = width * almost_unit_identity_c2(dist / width);

        float t = smoothstep3(coeff);
        float dtmp = (1.f - t) * zr[i] + t * zr[j] - slope * dist;

        d = maximum_smooth(d, dtmp, k_smoothing);
      }
      return maximum_smooth(d, vmin, k_smoothing);
    };
  else
    lambda = [&xrs, &yrs, &zr, &vmin, &slope, &k_smoothing, &width](float x_,
                                                                    float y_,
                                                                    float)
    {
      float d = std::numeric_limits<float>::max();
      for (size_t i = 0; i < xrs.size() - 1; i += 2)
      {
        int         j = i + 1;
        Vec2<float> e = {xrs[j] - xrs[i], yrs[j] - yrs[i]};
        Vec2<float> w = {x_ - xrs[i], y_ - yrs[i]};
        float       coeff = std::clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
        Vec2<float> b = {w.x - coeff * e.x, w.y - coeff * e.y};

        float dist = std::sqrt(dot(b, b));
        if (dist <= width) dist = width * almost_unit_identity_c2(dist / width);

        float t = smoothstep3(coeff);
        float dtmp = (1.f - t) * zr[i] + t * zr[j] - slope * dist;

        d = minimum_smooth(d, dtmp, k_smoothing);
      }
      return maximum_smooth(d, vmin, k_smoothing);
    };

  // eventually fill array
  Array array = Array(shape);
  fill_array_using_xy_function(array,
                               bbox_array,
                               nullptr,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               lambda);

  return array;
}

Array ridgelines_bezier(Vec2<int>                 shape,
                        const std::vector<float> &xr,
                        const std::vector<float> &yr,
                        const std::vector<float> &zr,
                        float                     slope,
                        float                     k_smoothing,
                        float                     width,
                        float                     vmin,
                        Vec4<float>               bbox,
                        const Array              *p_noise_x,
                        const Array              *p_noise_y,
                        const Array              *p_stretching,
                        Vec4<float>               bbox_array)
{
  // normalized node coordinates
  std::vector<float> xrs = xr;
  std::vector<float> yrs = yr;
  rescale_points_to_unit_square(xrs, yrs, bbox);

  // define noise function
  std::function<float(float, float, float)> lambda;

  // --- ridges
  if (slope > 0.f)
    lambda = [&xrs, &yrs, &zr, &vmin, &slope, &k_smoothing, &width](float x_,
                                                                    float y_,
                                                                    float)
    {
      float d_res = -std::numeric_limits<float>::max();
      float d_new;

      for (size_t i = 0; i < xrs.size() - 1; i += 3)
      {
        int j = i + 1;
        int k = i + 2;

        // https://iquilezles.org/articles/distfunctions2d/

        Vec2<float> a = {xrs[j] - xrs[i], yrs[j] - yrs[i]};
        Vec2<float> b = {xrs[i] - 2.f * xrs[j] + xrs[k],
                         yrs[i] - 2.f * yrs[j] + yrs[k]};
        Vec2<float> c = {2.f * a.x, 2.f * a.y};
        Vec2<float> d = {
            xrs[i] - x_,
            yrs[i] - y_,
        };
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

          float dist = std::sqrt(dot(dd, dd));
          if (dist <= width)
            dist = width * almost_unit_identity_c2(dist / width);

          t = smoothstep3(t);
          d_new = (1.f - t) * zr[i] + t * zr[k] - slope * dist;
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

          float dist1 = std::sqrt(dot(dd1, dd1));
          float dist2 = std::sqrt(dot(dd2, dd2));

          if (dist1 <= width)
            dist1 = width * almost_unit_identity_c2(dist1 / width);
          tt.x = smoothstep3(tt.x);
          float d_new1 = (1.f - tt.x) * zr[i] + tt.x * zr[k] - slope * dist1;

          if (dist2 <= width)
            dist2 = width * almost_unit_identity_c2(dist2 / width);

          tt.y = smoothstep3(tt.y);
          float d_new2 = (1.f - tt.y) * zr[i] + tt.y * zr[k] - slope * dist2;

          d_new = std::max(maximum_smooth(d_res, d_new1, k_smoothing),
                           maximum_smooth(d_res, d_new2, k_smoothing));
        }

        d_res = maximum_smooth(d_res, d_new, k_smoothing);
      }
      return maximum_smooth(d_res, vmin, k_smoothing);
    };

  else // --- valleys
    lambda = [&xrs, &yrs, &zr, &vmin, &slope, &k_smoothing, &width](float x_,
                                                                    float y_,
                                                                    float)
    {
      float d_res = std::numeric_limits<float>::max();
      float d_new;

      for (size_t i = 0; i < xrs.size() - 1; i += 3)
      {
        int j = i + 1;
        int k = i + 2;

        Vec2<float> a = {xrs[j] - xrs[i], yrs[j] - yrs[i]};
        Vec2<float> b = {xrs[i] - 2.f * xrs[j] + xrs[k],
                         yrs[i] - 2.f * yrs[j] + yrs[k]};
        Vec2<float> c = {2.f * a.x, 2.f * a.y};
        Vec2<float> d = {
            xrs[i] - x_,
            yrs[i] - y_,
        };
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

          float dist = std::sqrt(dot(dd, dd));
          if (dist <= width)
            dist = width * almost_unit_identity_c2(dist / width);

          t = smoothstep3(t);
          d_new = (1.f - t) * zr[i] + t * zr[k] - slope * dist;
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

          float dist1 = std::sqrt(dot(dd1, dd1));
          float dist2 = std::sqrt(dot(dd2, dd2));

          if (dist1 <= width)
            dist1 = width * almost_unit_identity_c2(dist1 / width);
          tt.x = smoothstep3(tt.x);
          float d_new1 = (1.f - tt.x) * zr[i] + tt.x * zr[k] - slope * dist1;

          if (dist2 <= width)
            dist2 = width * almost_unit_identity_c2(dist2 / width);

          tt.y = smoothstep3(tt.y);
          float d_new2 = (1.f - tt.y) * zr[i] + tt.y * zr[k] - slope * dist2;

          d_new = std::min(d_new1, d_new2);
        }

        d_res = minimum_smooth(d_res, d_new, k_smoothing);
      }
      return minimum_smooth(d_res, vmin, k_smoothing);
    };

  // eventually fill array
  Array array = Array(shape);
  fill_array_using_xy_function(array,
                               bbox_array,
                               nullptr,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               lambda);

  return array;
}

} // namespace hmap
