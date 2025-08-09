/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <vector>

#include "macrologger.h"

#include "highmap/geometry/point_sampling.hpp"
#include "highmap/operator.hpp"

namespace hmap
{

int convert_length_to_pixel(float x,
                            int   nx,
                            bool  lim_inf,
                            bool  lim_sup,
                            float scale)
{
  int ir = x / scale * static_cast<float>(nx);
  if (lim_inf) ir = std::max(ir, 1);
  if (lim_sup) ir = std::min(ir, nx - 1);
  return ir;
}

void expand_points_domain(std::vector<float> &x,
                          std::vector<float> &y,
                          std::vector<float> &value,
                          Vec4<float>         bbox)
{
  size_t n = x.size();
  x.resize(9 * n);
  y.resize(9 * n);
  value.resize(9 * n);

  float lx = bbox.b - bbox.a;
  float ly = bbox.d - bbox.c;

  int kshift = 0;
  for (int i = -1; i < 2; i++)
    for (int j = -1; j < 2; j++)
      if ((i != 0) or (j != 0))
      {
        kshift++;
        for (size_t k = 0; k < n; k++)
        {
          x[k + kshift * n] = x[k] + (float)i * lx;
          y[k + kshift * n] = y[k] + (float)j * ly;
          value[k + kshift * n] = value[k];
        }
      }
}

void expand_points_at_domain_boundaries(std::vector<float> &x,
                                        std::vector<float> &y,
                                        std::vector<float> &value,
                                        Vec4<float>         bbox,
                                        float               boundary_value)
{
  expand_points_domain_corners(x, y, value, bbox, boundary_value);

  int npoints = std::max(0, (int)std::sqrt((float)x.size()));

  const std::vector<float> xb = linspace(bbox.a, bbox.b, npoints, false);
  const std::vector<float> yb = linspace(bbox.c, bbox.d, npoints, false);

  for (int i = 0; i < npoints; i++)
  {
    x.push_back(xb[i]);
    y.push_back(bbox.c);

    x.push_back(xb[i]);
    y.push_back(bbox.d);

    x.push_back(bbox.a);
    y.push_back(yb[i]);

    x.push_back(bbox.b);
    y.push_back(yb[i]);

    value.push_back(boundary_value);
    value.push_back(boundary_value);
    value.push_back(boundary_value);
    value.push_back(boundary_value);
  }
}

void expand_points_domain_corners(std::vector<float> &x,
                                  std::vector<float> &y,
                                  std::vector<float> &value,
                                  Vec4<float>         bbox,
                                  float               corner_value)
{
  x.push_back(bbox.a);
  x.push_back(bbox.a);
  x.push_back(bbox.b);
  x.push_back(bbox.b);

  y.push_back(bbox.c);
  y.push_back(bbox.d);
  y.push_back(bbox.c);
  y.push_back(bbox.d);

  for (int i = 0; i < 4; i++)
    value.push_back(corner_value);
}

void grid_xy_vector(std::vector<float> &x,
                    std::vector<float> &y,
                    Vec2<int>           shape,
                    Vec4<float>         bbox,
                    bool                endpoint)
{
  x = linspace(bbox.a, bbox.b, shape.x, endpoint);
  y = linspace(bbox.c, bbox.d, shape.y, endpoint);
}

void rescale_grid_from_unit_square_to_bbox(std::vector<float> &x,
                                           std::vector<float> &y,
                                           Vec4<float>         bbox)
{
  size_t nx = x.size();
  size_t ny = y.size();

  for (size_t i = 0; i < nx; ++i)
  {
    float t = (float)i / ((float)nx - 1.f);
    x[i] = bbox.a + (bbox.b - bbox.a) * t;
  }

  for (size_t j = 0; j < ny; ++j)
  {
    float t = (float)j / ((float)ny - 1.f);
    y[j] = bbox.c + (bbox.d - bbox.c) * t;
  }
}

void rescale_points_to_unit_square(std::vector<float> &x,
                                   std::vector<float> &y,
                                   Vec4<float>         bbox)
{
  for (size_t k = 0; k < x.size(); k++)
    x[k] = (x[k] - bbox.a) / (bbox.b - bbox.a);

  for (size_t k = 0; k < y.size(); k++)
    y[k] = (y[k] - bbox.c) / (bbox.d - bbox.c);
}

} // namespace hmap
