/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry/grids.hpp"
#include "highmap/geometry/path.hpp"

namespace hmap
{

Array sdf_2d_polyline(const Path &path,
                      Vec2<int>   shape,
                      Vec4<float> bbox_path,
                      Array      *p_noise_x,
                      Array      *p_noise_y,
                      Vec4<float> bbox_array)
{
  Array sdf2(shape);

  // normalize path nodes coord to unit square
  std::vector<float> xp = path.get_x();
  std::vector<float> yp = path.get_y();

  rescale_grid_to_unit_square(xp, yp, bbox_path);

  // array cell coordinates
  std::vector<float> xg(shape.x);
  std::vector<float> yg(shape.y);

  rescale_grid_from_unit_square_to_bbox(xg, yg, bbox_array);

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
        float       coeff = std::clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
        Vec2<float> b = {w.x - e.x * coeff, w.y - e.y * coeff};
        d = std::min(d, dot(b, b));
      }
      sdf2(i, j) = std::sqrt(d);
    }

  return sdf2;
}

} // namespace hmap
