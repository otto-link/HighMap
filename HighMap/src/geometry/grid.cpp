/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <vector>

#include "macrologger.h"

#include "highmap/geometry/grids.hpp"
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

void expand_grid(std::vector<float> &x,
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

void expand_grid_boundaries(std::vector<float> &x,
                            std::vector<float> &y,
                            std::vector<float> &value,
                            Vec4<float>         bbox,
                            float               boundary_value)
{
  expand_grid_corners(x, y, value, bbox, boundary_value);

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

void expand_grid_corners(std::vector<float> &x,
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

void grid_from_array(Array              &array,
                     std::vector<float> &x,
                     std::vector<float> &y,
                     std::vector<float> &value,
                     Vec4<float>         bbox,
                     float               threshold)
{
  x.clear();
  y.clear();
  value.clear();

  float ax = (bbox.b - bbox.a) / (float)(array.shape.x - 1);
  float ay = (bbox.d - bbox.c) / (float)(array.shape.y - 1);

  for (int i = 0; i < array.shape.x; i++)
  {
    float xtmp = bbox.a + ax * (float)i;
    for (int j = 0; j < array.shape.y; j++)
      if (array(i, j) > threshold)
      {
        x.push_back(xtmp);
        y.push_back(bbox.c + ay * (float)j);
        value.push_back(array(i, j));
      }
  }
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

void random_grid(std::vector<float> &x,
                 std::vector<float> &y,
                 uint                seed,
                 Vec4<float>         bbox)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0.f, 1.f);
  size_t                                n = x.size();

  for (size_t k = 0; k < n; k++)
  {
    x[k] = dis(gen) * (bbox.b - bbox.a) + bbox.a;
    y[k] = dis(gen) * (bbox.d - bbox.c) + bbox.c;
  }
}

void random_grid(std::vector<float> &x,
                 std::vector<float> &y,
                 std::vector<float> &value,
                 uint                seed,
                 Vec4<float>         bbox)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0.f, 1.f);
  size_t                                n = x.size();

  for (size_t k = 0; k < n; k++)
  {
    x[k] = dis(gen) * (bbox.b - bbox.a) + bbox.a;
    y[k] = dis(gen) * (bbox.d - bbox.c) + bbox.c;
    value[k] = dis(gen);
  }
}

void random_grid(std::vector<float> &x,
                 std::vector<float> &y,
                 std::vector<float> &value,
                 uint                seed,
                 Vec2<float>         delta,
                 Vec2<float>         stagger_ratio,
                 Vec2<float>         jitter_ratio,
                 Vec4<float>         bbox)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(-1.f, 1.f);

  x.clear();
  y.clear();
  value.clear();

  int nx = (int)((bbox.b - bbox.a) / delta.x) + 3;
  int ny = (int)((bbox.d - bbox.c) / delta.y) + 3;

  x.reserve(nx * ny);
  y.reserve(nx * ny);
  value.reserve(nx * ny);

  const std::vector<float> xlist = linspace(bbox.a - delta.x,
                                            bbox.b + delta.x,
                                            nx);
  const std::vector<float> ylist = linspace(bbox.c - delta.y,
                                            bbox.d + delta.y,
                                            ny);

  Vec2<float> offset = {0.f, 0.f};

  for (float yp : ylist)
  {
    offset.x = (offset.x == 0.f) ? stagger_ratio.x * delta.x : 0.f;
    offset.y = 0.f;

    for (float xp : xlist)
    {
      float new_x = xp + offset.x;
      float new_y = yp + offset.y;

      // add jitter
      if (jitter_ratio.x > 0.f)
        new_x += 0.5f * jitter_ratio.x * delta.x * dis(gen);

      if (jitter_ratio.y > 0.f)
        new_y += 0.5f * jitter_ratio.y * delta.y * dis(gen);

      // save
      x.push_back(new_x);
      y.push_back(new_y);
      value.push_back(0.5f * (dis(gen) + 1.f));

      offset.y = (offset.y == 0.f) ? stagger_ratio.y * delta.y : 0.f;
    }
  }

  // clean-up
  remove_grid_points_outside_bbox(x, y, value, bbox);
}

void random_grid_density(std::vector<float> &x,
                         std::vector<float> &y,
                         const Array        &density,
                         uint                seed,
                         Vec4<float>         bbox)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0.f, 1.f);
  size_t                                n = x.size();

  size_t k = 0;
  while (k < n)
  {
    // take a random point within the grid and pick a random value:
    // if the value is smaller than the local density, the point is kept,
    // if not, try again...
    float xr = dis(gen);
    float yr = dis(gen);
    float rd = dis(gen);

    int i = (int)(xr * (density.shape.x - 1));
    int j = (int)(yr * (density.shape.y - 1));

    if (rd < density(i, j))
    {
      x[k] = xr * (bbox.b - bbox.a) + bbox.a;
      y[k] = yr * (bbox.d - bbox.c) + bbox.c;
      k++;
    }
  }
}

void random_grid_jittered(std::vector<float> &x,
                          std::vector<float> &y,
                          float               scale,
                          uint                seed,
                          Vec4<float>         bbox)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(-1.f, 1.f);
  size_t                                n = x.size();
  float                                 nsq = (float)std::pow((float)n, 0.5f);
  float                                 dx = 1.f / (nsq - 1.f);

  for (size_t k = 0; k < n; k++)
  {
    int j = (int)((float)k / nsq);

    x[k] = dx * ((float)(k - j * nsq) + scale * dis(gen));
    y[k] = dx * ((float)j + scale * dis(gen));

    x[k] = std::clamp(x[k], 0.f, 1.f);
    y[k] = std::clamp(y[k], 0.f, 1.f);

    x[k] = x[k] * (bbox.b - bbox.a) + bbox.a;
    y[k] = y[k] * (bbox.d - bbox.c) + bbox.c;
  }
}

void remove_grid_points_outside_bbox(std::vector<float> &x,
                                     std::vector<float> &y,
                                     std::vector<float> &v,
                                     Vec4<float>         bbox)
{
  std::vector<size_t> indices_to_remove;

  for (size_t k = 0; k < x.size(); k++)
  {
    if (x[k] < bbox.a || x[k] > bbox.b || y[k] < bbox.c || y[k] > bbox.d)
      indices_to_remove.push_back(k);
  }

  for (auto it = indices_to_remove.rbegin(); it != indices_to_remove.rend();
       ++it)
  {
    x.erase(x.begin() + *it);
    y.erase(y.begin() + *it);
    v.erase(v.begin() + *it);
  }
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

void rescale_grid_to_unit_square(std::vector<float> &x,
                                 std::vector<float> &y,
                                 Vec4<float>         bbox)
{
  for (size_t k = 0; k < x.size(); k++)
    x[k] = (x[k] - bbox.a) / (bbox.b - bbox.a);

  for (size_t k = 0; k < y.size(); k++)
    y[k] = (y[k] - bbox.c) / (bbox.d - bbox.c);
}

} // namespace hmap
