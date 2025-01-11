R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

void kernel sdf_2d_polyline(global float   *sdf2,
                            global float   *noise_x,
                            global float   *noise_y,
                            constant float *xp,
                            constant float *yp,
                            const int       nx,
                            const int       ny,
                            const int       np,
                            const int       has_noise_x,
                            const int       has_noise_y,
                            const float4    bbox)
{
  int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  int index = linear_index(g.x, g.y, nx);

  float dx = has_noise_x > 0 ? noise_x[index] : 0.f;
  float dy = has_noise_y > 0 ? noise_y[index] : 0.f;

  // (x, y) position
  float2 pos = g_to_xy(g, nx, ny, 1.f, 1.f, dx, dy, bbox);

  // sdf
  float d = FLT_MAX;

  for (int k = 0; k < np - 1; ++k)
  {
    int    p = k + 1;
    float2 e = (float2)(xp[p] - xp[k], yp[p] - yp[k]);
    float2 w = (float2)(pos.x - xp[k], pos.y - yp[k]);
    float  t = clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
    float2 b = w - e * t;
    d = min(d, dot(b, b));
  }

  sdf2[index] = sqrt(d);
}
)""
