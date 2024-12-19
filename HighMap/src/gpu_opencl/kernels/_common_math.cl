R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
float lerp(float a, float b, float t)
{
  return (1.f - t) * a + t * b;
}

float max_smooth(float a, float b, float k)
{
  float h = max(k - fabs(a - b), 0.f) / k;
  return max(a, b) + pow(h, 3) * k / 6.f;
}

float min_smooth(float a, float b, float k)
{
  float h = max(k - fabs(a - b), 0.f) / k;
  return min(a, b) - pow(h, 3) * k / 6.f;
}

float2 normed2(float2 vec)
{
  float d = length(vec);
  if (d)
    return vec / d;
  else
    return 0.f;
}

// assumes x in [0, 1]
float remap(float x, float vmin, float vmax)
{
  return x * (vmax - vmin) + vmin;
}

float remap_from(float x,
                 float vmin,
                 float vmax,
                 float from_min,
                 float from_max)
{
  return (x - from_min) / (from_max - from_min) * (vmax - vmin) + vmin;
}

float smoothstep3(float x)
{
  return x * x * (3.f - 2.f * x);
}

float smoothstep5(float x)
{
  return x * x * x * (x * (6.f * x - 15.f) + 10.f);
}

float smoothstep3_gl(float x, float vmin, float vmax)
{
  x = clamp((x - vmin) / (vmax - vmin), 0.f, 1.f);
  return x * x * (3.f - 2.f * x);
}
)""
