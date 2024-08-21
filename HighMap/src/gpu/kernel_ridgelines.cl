R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void kernel ridgelines(global float   *output,
                       constant float *x,
                       constant float *y,
                       constant float *z,
                       const int       xyz_size,
                       const float     slope,
                       const float     k_smoothing,
                       const float     width,
                       const float     vmin,
                       const int       nx,
                       const int       ny)
{
  int2 g = {get_global_id(0), get_global_id(1)};
  int  index = linear_index(g.x, g.y, ny);

  float2 pos = (float2)((float)g.x / nx, (float)g.y / ny);
  float  d;

  if (slope > 0.f)
  {
    d = -FLT_MAX;
    for (int i = 0; i < xyz_size - 1; i += 2)
    {
      int    j = i + 1;
      float2 e = (float2)(x[j] - x[i], y[j] - y[i]);
      float2 w = pos - (float2)(x[i], y[i]);
      float  coeff = clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
      float2 b = w - coeff * e;

      float dist = sqrt(dot(b, b));
      if (dist <= width) dist = width * almost_unit_identity_c2(dist / width);

      float coeff_z = smoothstep3(coeff);
      float dtmp = (1.f - coeff_z) * z[i] + coeff_z * z[j] - slope * dist;

      d = max_smooth(d, dtmp, k_smoothing);
    }
  }
  else
  {
    d = +FLT_MAX;
    for (int i = 0; i < xyz_size - 1; i += 2)
    {
      int    j = i + 1;
      float2 e = (float2)(x[j] - x[i], y[j] - y[i]);
      float2 w = pos - (float2)(x[i], y[i]);
      float  coeff = clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
      float2 b = w - coeff * e;

      float dist = sqrt(dot(b, b));
      if (dist <= width) dist = width * almost_unit_identity_c2(dist / width);

      float coeff_z = smoothstep3(coeff);
      float dtmp = (1.f - coeff_z) * z[i] + coeff_z * z[j] - slope * dist;

      d = min_smooth(d, dtmp, k_smoothing);
    }
  }

  // add fbm noise

  float fseed = 0.f;
  float amp = 0.5f;
  float sum = d;
  float kw = 2.f;
  float weight = 0.7f;
  for (int k = 0; k < 8; k++)
  {
    float value = noise_simplex(kw * pos, fseed);
    sum += value * amp;
    amp *= (1.f - weight) + weight * min(value + 1.f, 2.f) * 0.5f;

    amp /= 2.f;
    kw *= 2.f;
  }
  d += sum;

  d = max_smooth(d, vmin, k_smoothing);

  output[index] = d;
}
)""
