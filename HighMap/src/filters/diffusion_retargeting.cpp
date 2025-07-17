/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "macrologger.h"

#include "highmap/boundary.hpp"
#include "highmap/filters.hpp"
#include "highmap/geometry/cloud.hpp"
#include "highmap/operator.hpp"
#include "highmap/range.hpp"

namespace hmap
{

Array diffusion_retargeting(const Array &array_before,
                            const Array &array_after,
                            int          ir)
{
  Vec2<int> shape = array_before.shape;

  // select points of interest
  Array delta(shape);

  for (int j = 1; j < shape.y - 1; j++)
    for (int i = 1; i < shape.x - 1; i++)
    {
      if (array_before(i, j) > array_before(i + 1, j) &&
          array_before(i, j) > array_before(i + 1, j + 1) &&
          array_before(i, j) > array_before(i, j + 1) &&
          array_before(i, j) > array_before(i - 1, j + 1) &&
          array_before(i, j) > array_before(i - 1, j) &&
          array_before(i, j) > array_before(i - 1, j - 1) &&
          array_before(i, j) > array_before(i, j - 1) &&
          array_before(i, j) > array_before(i + 1, j - 1))
      {
        delta(i, j) = array_before(i, j) - array_after(i, j);
      }
    }

  // apply correction
  float vmin = delta.min();
  float vmax = delta.max();

  smooth_cpulse(delta, ir);

  remap(delta, vmin, vmax);

  return delta + array_after;
}

} // namespace hmap
