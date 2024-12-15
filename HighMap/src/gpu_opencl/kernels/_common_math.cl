R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
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

float lerp(float a, float b, float t)
{
  return (1.f - t) * a + t * b;
}
)""
