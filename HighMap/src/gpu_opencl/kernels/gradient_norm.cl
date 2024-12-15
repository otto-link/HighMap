R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void kernel gradient_norm(global float *array,
                          global float *g_norm,
                          const int     nx,
                          const int     ny)
{
  int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  float dx, dy;

  if (g.x == 0)
    dx = array[linear_index(g.x + 1, g.y, nx)] -
         array[linear_index(g.x, g.y, nx)];
  else if (g.x == nx - 1)
    dx = array[linear_index(g.x, g.y, nx)] -
         array[linear_index(g.x - 1, g.y, nx)];
  else
    dx = 0.5f * (array[linear_index(g.x + 1, g.y, nx)] -
                 array[linear_index(g.x - 1, g.y, nx)]);

  if (g.y == 0)
    dy = array[linear_index(g.x, g.y + 1, nx)] -
         array[linear_index(g.x, g.y, nx)];
  else if (g.y == ny - 1)
    dy = array[linear_index(g.x, g.y, nx)] -
         array[linear_index(g.x, g.y - 1, nx)];
  else
    dy = 0.5f * (array[linear_index(g.x, g.y + 1, nx)] -
                 array[linear_index(g.x, g.y - 1, nx)]);

  g_norm[linear_index(g.x, g.y, nx)] = hypot(dx, dy);
}
)""
