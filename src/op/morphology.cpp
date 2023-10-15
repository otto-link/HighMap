/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap
{

void flood_fill(Array &array,
                int    i,
                int    j,
                float  fill_value,
                float  background_value)
{
  if (i < 0 || i > array.shape.x - 1 || j < 0 || j > array.shape.y - 1)
    return;
  else if (array(i, j) != background_value)
    return;
  else
  {
    array(i, j) = fill_value;
    flood_fill(array, i + 1, j, fill_value, background_value);
    flood_fill(array, i - 1, j, fill_value, background_value);
    flood_fill(array, i, j - 1, fill_value, background_value);
    flood_fill(array, i, j + 1, fill_value, background_value);
  }
}

} // namespace hmap
