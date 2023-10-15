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
  if (array(i, j) != background_value)
    return;
  else if (i < 0 || i > array.shape.x - 1 || j < 0 || j > array.shape.y - 1)
    return;
  else
  {
    array(i, j) = fill_value;
    flood_fill(array, i + 1, j, fill_value);
    flood_fill(array, i - 1, j, fill_value);
    flood_fill(array, i + 1, j - 1, fill_value);
    flood_fill(array, i + 1, j + 1, fill_value);
  }
}

Array skeleton_middle(const Array &array)
{
  Array sk = Array(array.shape);
  Array v = array;
  make_binary(v, 0.f); // reduce array values to {0, 1}

  for (int i = 0; i < v.shape.x; i++)
  {
    int j1 = 0;
    for (int j = 0; j < v.shape.y - 1; j++)
    {
      float dv = (v(i, j + 1) - v(i, j));

      if (dv > 0.f)
        j1 = j;
      else if (dv < 0.f)
        sk(i, j1 + (int)(0.5f * (float)(j - j1))) = 1.f;
    }
  }

  // for (int j = 0; j < v.shape.y; j++)
  //   {
  //     int i1 = 0;
  //     for (int i = 0; i < v.shape.x - 1; i++)
  // 	{
  // 	  float dv = (v(i + 1, j) - v(i, j));

  // 	  if (dv > 0.f)
  // 	    i1 = j;
  // 	  else if (dv < 0.f)
  // 	    sk(i1 + (int)(0.5f * (float)(i - i1)), j) = 1.f;
  // 	}
  //   }

  return sk;
}

} // namespace hmap
