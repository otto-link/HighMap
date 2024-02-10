R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void kernel ridgelines_slope(global float   *output,
                             constant float *x,
                             constant float *y,
                             constant float *z,
                             const int       xyz_size,
                             const float     slope,
                             const int       nx,
                             const int       ny)
{
  int2 g = {get_global_id(0), get_global_id(1)};
  int  index = linear_index(g.x, g.y, ny);

  float2 pos = (float2)((float)g.x / nx, (float)g.y / ny);
  float  d = -FLT_MAX;

  for (int i = 0; i < xyz_size - 1; i += 2)
  {
    int    j = i + 1;
    float2 e = (float2)(x[j] - x[i], y[j] - y[i]);
    float2 w = pos - (float2)(x[i], y[i]);
    float  coeff = clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
    float2 b = w - coeff * e;
    float  dtmp = (1.f - coeff) * z[i] + coeff * z[j] - slope * sqrt(dot(b, b));
    d = max(d, dtmp);
  }

  output[index] = d;
}
)""
