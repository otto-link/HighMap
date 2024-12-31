R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void kernel minimum_smooth(global float *array1,
                           global float *array2,
                           const int     nx,
                           const int     ny,
                           const float   k)
{
  int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  int index = linear_index(g.x, g.y, nx);
  array1[index] = min_smooth(array1[index], array2[index], k);
}
)""
