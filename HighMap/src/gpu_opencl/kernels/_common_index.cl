R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

float2 g_to_xy(const int2   g,
               const int    nx,
               const int    ny,
               const float  kx,
               const float  ky,
               const float  dx,
               const float  dy,
               const float4 bbox)
{
  float x = dx + (float)g.x / (float)nx;
  float y = dy + (float)g.y / (float)ny;

  x = kx * (x * (bbox.y - bbox.x) + bbox.x);
  y = ky * (y * (bbox.w - bbox.z) + bbox.z);

  return (float2)(x, y);
}

int linear_index(const int i, const int j, const int nx)
{
  return j * nx + i;
}

void update_interp_param(float2 pos, int *i, int *j, float *u, float *v)
{
  *i = (int)pos.x;
  *j = (int)pos.y;
  *u = pos.x - *i;
  *v = pos.y - *j;
}
)""
