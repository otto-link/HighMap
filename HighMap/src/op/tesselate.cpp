/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/interpolate.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array tessellate(Array &array, uint seed, float node_density, Array *p_weight)
{
  Array density = Array();

  if (!p_weight)
  {
    density = constant(array.shape, 1.f);
    p_weight = &density;
  }

  // generate control nodes
  int nnodes = (int)(node_density * array.shape.x * array.shape.y);

  std::vector<float> x(nnodes);
  std::vector<float> y(nnodes);
  Vec4<float>        bbox = Vec4<float>(0.f, 1.f, 0.f, 1.f);

  random_grid_density(x, y, *p_weight, seed, bbox);

  {
    std::vector<float> value(nnodes);
    expand_grid_boundaries(x, y, value, bbox);
  }

  // get values at control nodes
  Cloud cloud = Cloud(x, y);
  cloud.set_values_from_array(array, bbox);

  // interpolate
  Array array_out = interpolate2d(array.shape,
                                  x,
                                  y,
                                  cloud.get_values(),
                                  interpolator2d::delaunay,
                                  nullptr, // noise
                                  nullptr, // noise
                                  nullptr,
                                  bbox);

  return array_out;
}

} // namespace hmap
