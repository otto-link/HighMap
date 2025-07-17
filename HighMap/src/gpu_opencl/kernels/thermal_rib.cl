R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

void kernel thermal_rib(global float *z,
                        const int     nx,
                        const int     ny,
                        const int     it)
{
  int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  // --- boundaries

  int index = linear_index(g.x, g.y, nx);

  if (g.x == 0)
  {
    z[index] = z[linear_index(1, g.y, nx)];
    return;
  }
  if (g.x == nx - 1)
  {
    z[index] = z[linear_index(nx - 2, g.y, nx)];
    return;
  }
  if (g.y == 0)
  {
    z[index] = z[linear_index(g.x, 1, nx)];
    return;
  }
  if (g.y == ny - 1)
  {
    z[index] = z[linear_index(g.x, ny - 2, nx)];
    return;
  }

  // --- thermal erosion

  const int   di[8] = {-1, 0, 0, 1, -1, -1, 1, 1};
  const int   dj[8] = {0, 1, -1, 0, -1, 1, -1, 1};
  const float c[8] = {1.f, 1.f, 1.f, 1.f, 1.414f, 1.414f, 1.414f, 1.414f};

  float delta_min = FLT_MAX;
  float val = z[index];

  for (int k = 0; k < 8; k++)
  {
    float dz = fabs(val - z[linear_index(g.x + di[k], g.y + dj[k], nx)]) / c[k];
    delta_min = min(delta_min, dz);
  }

  z[index] -= delta_min;

  // TODO add laplacian
  /* float sigma = 0.2f; */
  /* int im1 = g.x == 0 ? 1 : g.x - 1; */
  /* int ip1 = g.x == nx - 1 ? nx - 2 : g.x + 1; */
  /* int jm1 = g.y == 0 ? 1 : g.y - 1; */
  /* int jp1 = g.y == ny - 1 ? ny - 2 : g.y + 1; */

  /* // compute laplacian */
  /* float delta = -4.f * z[linear_index(g.x, g.y, nx)] + */
  /*               z[linear_index(ip1, g.y, nx)] + */
  /*               z[linear_index(im1, g.y, nx)] + */
  /*               z[linear_index(g.x, jm1, nx)] + */
  /*               z[linear_index(g.x, jp1, nx)]; */

  /* z[linear_index(g.x, g.y, nx)] += sigma * delta; */
}
)""
