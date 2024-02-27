/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array base_elevation(Vec2<int>                       shape,
                     std::vector<std::vector<float>> values,
                     float                           width_factor,
                     Array                          *p_noise_x,
                     Array                          *p_noise_y,
                     Vec2<float>                     shift,
                     Vec2<float>                     scale)
{
  Array array = Array(shape);

  // get number of control points per direction
  size_t ni = values.size();
  size_t nj = values[0].size();

  // corresponding coordinates
  float              dxc = 1.f / (float)ni;
  float              dyc = 1.f / (float)nj;
  std::vector<float> xc = linspace(0.5f * dxc, 1.f - 0.5f * dxc, ni);
  std::vector<float> yc = linspace(0.5f * dyc, 1.f - 0.5f * dyc, nj);

  std::vector<float> x = linspace(shift.x, shift.x + scale.x, shape.x, false);
  std::vector<float> y = linspace(shift.y, shift.y + scale.y, shape.y, false);

  // Gaussian half-widths based on the mesh discretization
  float s2x = 2.f / dxc / dxc / width_factor;
  float s2y = 2.f / dyc / dyc / width_factor;

  for (size_t p = 0; p < ni; p++)
    for (size_t q = 0; q < nj; q++)
    {
      auto lambda = [&xc, &yc, &p, &q, &s2x, &s2y](float x_, float y_)
      {
        float r2 = s2x * (x_ - xc[p]) * (x_ - xc[p]) +
                   s2y * (y_ - yc[q]) * (y_ - yc[q]);
        return std::exp(-r2);
      };

      Array array_tmp = Array(array.shape);
      helper_get_noise(array_tmp, x, y, p_noise_x, p_noise_y, nullptr, lambda);
      array += values[p][q] * array_tmp;
    }

  return array;
}

Array ridgelines(Vec2<int>          shape,
                 std::vector<float> xr,
                 std::vector<float> yr,
                 std::vector<float> zr,
                 float              slope,
                 float              k_smoothing,
                 float              width,
                 float              vmin,
                 Vec4<float>        bbox,
                 Array             *p_noise_x,
                 Array             *p_noise_y,
                 Vec2<float>        shift,
                 Vec2<float>        scale)
{
  std::vector<float> x = linspace(shift.x, shift.x + scale.x, shape.x, false);
  std::vector<float> y = linspace(shift.y, shift.y + scale.y, shape.y, false);

  // normalized node coordinates
  std::vector<float> xr_scaled = xr;
  std::vector<float> yr_scaled = yr;
  rescale_grid_to_unit_square(xr_scaled, yr_scaled, bbox);

  // define noise function
  std::function<float(float x, float y)> lambda;

  if (slope > 0.f)
    lambda = [&xr_scaled, &yr_scaled, &zr, &vmin, &slope, &k_smoothing, &width](
                 float x_,
                 float y_)
    {
      float d = -std::numeric_limits<float>::max();
      for (size_t i = 0; i < xr_scaled.size() - 1; i += 2)
      {
        int         j = i + 1;
        Vec2<float> e = {xr_scaled[j] - xr_scaled[i],
                         yr_scaled[j] - yr_scaled[i]};
        Vec2<float> w = {x_ - xr_scaled[i], y_ - yr_scaled[i]};
        float       coeff = std::clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
        Vec2<float> b = {w.x - coeff * e.x, w.y - coeff * e.y};

        float dist = sqrt(dot(b, b));
        if (dist <= width)
        {
          float dw = dist / width;
          // amost unit with second-order derivative equals 0 at x = 1
          // also to avoid discontinuities in some cases
          dw = dw * dw * (dw * dw - 3.f * dw + 3.f);
          dist = width * dw;
        }

        float coeff_z = coeff * coeff * (3.f - 2.f * coeff);
        float dtmp = (1.f - coeff_z) * zr[i] + coeff_z * zr[j] - slope * dist;

        d = maximum_smooth(d, dtmp, k_smoothing);
      }
      return maximum_smooth(d, vmin, k_smoothing);
    };
  else
    lambda = [&xr_scaled, &yr_scaled, &zr, &vmin, &slope, &k_smoothing, &width](
                 float x_,
                 float y_)
    {
      float d = std::numeric_limits<float>::max();
      for (size_t i = 0; i < xr_scaled.size() - 1; i += 2)
      {
        int         j = i + 1;
        Vec2<float> e = {xr_scaled[j] - xr_scaled[i],
                         yr_scaled[j] - yr_scaled[i]};
        Vec2<float> w = {x_ - xr_scaled[i], y_ - yr_scaled[i]};
        float       coeff = std::clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
        Vec2<float> b = {w.x - coeff * e.x, w.y - coeff * e.y};

        float dist = sqrt(dot(b, b));
        if (dist <= width)
        {
          float dw = dist / width;
          // amost unit with second-order derivative equals 0 at x = 1
          // also to avoid discontinuities in some cases
          dw = dw * dw * (dw * dw - 3.f * dw + 3.f);
          dist = width * dw;
        }

        float coeff_z = coeff * coeff * (3.f - 2.f * coeff);
        float dtmp = (1.f - coeff_z) * zr[i] + coeff_z * zr[j] - slope * dist;

        d = minimum_smooth(d, dtmp, k_smoothing);
      }
      return maximum_smooth(d, vmin, k_smoothing);
    };

  // eventually fill array
  Array array = Array(shape);
  helper_get_noise(array, x, y, p_noise_x, p_noise_y, nullptr, lambda);

  return array;
}

} // namespace hmap
