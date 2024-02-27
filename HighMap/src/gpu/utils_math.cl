R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
float almost_unit_identity_c1(float x)
{
  return (2.f - x) * x * x;
}

float almost_unit_identity_c2(float x)
{
  // second-order derivative equals 0 at x = 1 also to avoid
  // discontinuities in some cases
  return x * x * (x * x - 3.f * x + 3.f);
}

float gain(float x, float k)
{
  float a = 0.5f * pow(2.f * ((x < 0.5f) ? x : 1.f - x), k);
  return (x < 0.5f) ? a : 1.f - a;
}

float smoothstep_upper(float x, float k)
{
  // k in [0, 0.5] to avoid overshoot
  // return x + k * x * x * (1.f - x * x);
  return x + k * x * (1.f - x * x);
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

float smoothstep3(float x)
{
  return x * x * (3.f - 2.f * x);
}

float smoothstep5(float x)
{
  return x * x * x * (x * (6.f * x - 15.f) + 10.f);
}
)""
