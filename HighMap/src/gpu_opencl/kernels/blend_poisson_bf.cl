R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void kernel blend_poisson_bf(global float       *array1,
                             const global float *array2,
                             const global float *mask,
                             const int           nx,
                             const int           ny,
                             const int           has_mask)
{
  int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  int im1 = g.x == 0 ? 1 : g.x - 1;
  int ip1 = g.x == nx - 1 ? nx - 2 : g.x + 1;
  int jm1 = g.y == 0 ? 1 : g.y - 1;
  int jp1 = g.y == ny - 1 ? ny - 2 : g.y + 1;

  // compute laplacian
  float delta1 = -4.f * array1[linear_index(g.x, g.y, nx)] +
                 array1[linear_index(ip1, g.y, nx)] +
                 array1[linear_index(im1, g.y, nx)] +
                 array1[linear_index(g.x, jm1, nx)] +
                 array1[linear_index(g.x, jp1, nx)];

  float delta2 = -4.f * array2[linear_index(g.x, g.y, nx)] +
                 array2[linear_index(ip1, g.y, nx)] +
                 array2[linear_index(im1, g.y, nx)] +
                 array2[linear_index(g.x, jm1, nx)] +
                 array2[linear_index(g.x, jp1, nx)];

  delta1 *= 0.25f;
  delta2 *= 0.25f;

  float amp = has_mask > 0 ? mask[linear_index(g.x, g.y, nx)] : 1.f;

  array1[linear_index(g.x, g.y, nx)] -= 0.25f * amp * (delta2 - delta1);
}
)""
