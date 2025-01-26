R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
float pow_float(float base, float x)
{
  return exp(x * log(base));
}

float pow_int(float base, int exp)
{
  float result = 1.f;
  for (int i = 0; i < abs(exp); ++i)
  {
    result *= base;
  }
  return exp < 0 ? 1.f / result : result;
}

float2 angle_to_dir(float angle)
{
  return (float2)(cos(angle / 180.f * 3.14159f), sin(angle / 180.f * 3.14159f));
}

float cubic_interp(float p0, float p1, float p2, float p3, float t)
{
  float a = p3 - p2 - p0 + p1;
  float b = p0 - p1 - a;
  float c = p2 - p0;
  float d = p1;
  return a * t * t * t + b * t * t + c * t + d;
}

float lerp(float a, float b, float t)
{
  return (1.f - t) * a + t * b;
}

float max_smooth(float a, float b, float k)
{
  float h = max(k - fabs(a - b), 0.f) / k;
  return max(a, b) + pow_int(h, 3) * k / 6.f;
}

float min_smooth(float a, float b, float k)
{
  float h = max(k - fabs(a - b), 0.f) / k;
  return min(a, b) - pow_int(h, 3) * k / 6.f;
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

float2 smoothstep3_f2(float2 x)
{
  return x * x * (3.f - 2.f * x);
}

float smoothstep5(float x)
{
  return x * x * x * (x * (6.f * x - 15.f) + 10.f);
}

float2 smoothstep5_f2(float2 x)
{
  return x * x * x * (x * (6.f * x - 15.f) + 10.f);
}

float smoothstep3_gl(float x, float vmin, float vmax)
{
  x = clamp((x - vmin) / (vmax - vmin), 0.f, 1.f);
  return x * x * (3.f - 2.f * x);
}

float smoothstep3_lower(const float x)
{
  return x * (2.f * x - x * x);
}

float smoothstep3_upper(const float x)
{
  return x * (1.f + x - x * x);
}

float smoothstep_rational(float x, float n)
{
  // https://iquilezles.org/articles/smoothsteps/
  return pow_float(x, n) / (pow_float(x, n) + pow_float(1.f - x, n));
}
)""
