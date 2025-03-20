/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/grids.hpp"
#include "highmap/interpolate2d.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array tessellate(Array       &array,
                 uint         seed,
                 float        node_density,
                 const Array *p_weight)
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
  Vec4<float>        bbox = unit_square_bbox();

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
                                  InterpolationMethod2D::DELAUNAY,
                                  nullptr, // noise
                                  nullptr, // noise
                                  nullptr,
                                  bbox);

  return array_out;
}

} // namespace hmap
