/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry/grids.hpp"

namespace hmap
{

void fill_array_using_xy_function(
    Array                                    &array,
    Vec4<float>                               bbox,
    const Array                              *p_ctrl_param,
    const Array                              *p_noise_x,
    const Array                              *p_noise_y,
    const Array                              *p_stretching,
    std::function<float(float, float, float)> fct_xy)
{
  Vec2<int>          shape = array.shape;
  std::vector<float> x, y;
  grid_xy_vector(x, y, shape, bbox, false); // no endpoint

  Array ctrl_array = p_ctrl_param == nullptr ? Array(shape, 1.f)
                                             : *p_ctrl_param;

  if (p_stretching) // with stretching
  {
    if ((!p_noise_x) and (!p_noise_y))
    {
      for (int j = 0; j < shape.y; j++)
        for (int i = 0; i < shape.x; i++)
          array(i, j) = fct_xy(x[i] * (*p_stretching)(i, j),
                               y[j] * (*p_stretching)(i, j),
                               ctrl_array(i, j));
    }
    else if (p_noise_x and (!p_noise_y))
    {
      for (int j = 0; j < shape.y; j++)
        for (int i = 0; i < shape.x; i++)
          array(i,
                j) = fct_xy(x[i] * (*p_stretching)(i, j) + (*p_noise_x)(i, j),
                            y[j] * (*p_stretching)(i, j),
                            ctrl_array(i, j));
    }
    else if ((!p_noise_x) and p_noise_y)
    {
      for (int j = 0; j < shape.y; j++)
        for (int i = 0; i < shape.x; i++)
          array(i,
                j) = fct_xy(x[i] * (*p_stretching)(i, j),
                            y[j] * (*p_stretching)(i, j) + (*p_noise_y)(i, j),
                            ctrl_array(i, j));
    }
    else if (p_noise_x and p_noise_y)
    {
      for (int j = 0; j < shape.y; j++)
        for (int i = 0; i < shape.x; i++)
          array(i,
                j) = fct_xy(x[i] * (*p_stretching)(i, j) + (*p_noise_x)(i, j),
                            y[j] * (*p_stretching)(i, j) + (*p_noise_y)(i, j),
                            ctrl_array(i, j));
    }
  }
  else // without stretching
  {
    if ((!p_noise_x) and (!p_noise_y))
    {
      for (int j = 0; j < shape.y; j++)
        for (int i = 0; i < shape.x; i++)
          array(i, j) = fct_xy(x[i], y[j], ctrl_array(i, j));
    }
    else if (p_noise_x and (!p_noise_y))
    {
      for (int j = 0; j < shape.y; j++)
        for (int i = 0; i < shape.x; i++)
          array(i,
                j) = fct_xy(x[i] + (*p_noise_x)(i, j), y[j], ctrl_array(i, j));
    }
    else if ((!p_noise_x) and p_noise_y)
    {
      for (int j = 0; j < shape.y; j++)
        for (int i = 0; i < shape.x; i++)
          array(i,
                j) = fct_xy(x[i], y[j] + (*p_noise_y)(i, j), ctrl_array(i, j));
    }
    else if (p_noise_x and p_noise_y)
    {
      for (int j = 0; j < shape.y; j++)
        for (int i = 0; i < shape.x; i++)
          array(i, j) = fct_xy(x[i] + (*p_noise_x)(i, j),
                               y[j] + (*p_noise_y)(i, j),
                               ctrl_array(i, j));
    }
  }
}

void fill_array_using_xy_function(
    Array                                    &array,
    Vec4<float>                               bbox,
    const Array                              *p_ctrl_param,
    const Array                              *p_noise_x,
    const Array                              *p_noise_y,
    const Array                              *p_stretching,
    std::function<float(float, float, float)> fct_xy,
    int                                       subsampling)
{
  Vec2<int> shape = array.shape;
  Vec2<int> shape_sub = Vec2<int>((int)(shape.x / subsampling),
                                  (int)(shape.y / subsampling));
  Array     array_sub = Array(shape_sub);

  std::vector<float> x, y;
  grid_xy_vector(x, y, shape, bbox, false);

  Array ctrl_array = p_ctrl_param == nullptr ? Array(shape, 1.f)
                                             : *p_ctrl_param;

  // subsampled grid (with endpoints to cover the exact same domain as
  // the original grid)
  Vec4<float> bbox_sub = Vec4<float>(x.front(), x.back(), y.front(), y.back());

  // resample control parameter
  Array ctrl_array_sub = ctrl_array.resample_to_shape(shape_sub);

  // also resample input noise and stretching
  Array noise_x_sub;
  Array noise_y_sub;
  Array stretching_sub;

  Array *p_noise_x_sub = nullptr;
  Array *p_noise_y_sub = nullptr;
  Array *p_stretching_sub = nullptr;

  if (p_noise_x != nullptr)
  {
    noise_x_sub = p_noise_x->resample_to_shape(shape_sub);
    p_noise_x_sub = &noise_x_sub;
  }

  if (p_noise_y != nullptr)
  {
    noise_x_sub = p_noise_y->resample_to_shape(shape_sub);
    p_noise_y_sub = &noise_x_sub;
  }

  if (p_stretching != nullptr)
  {
    noise_x_sub = p_stretching->resample_to_shape(shape_sub);
    p_stretching_sub = &noise_x_sub;
  }

  fill_array_using_xy_function(array_sub,
                               bbox_sub,
                               &ctrl_array_sub,
                               p_noise_x_sub,
                               p_noise_y_sub,
                               p_stretching_sub,
                               fct_xy);

  // interpolate on finer grid
  array = array_sub.resample_to_shape(shape);
}

} // namespace hmap
