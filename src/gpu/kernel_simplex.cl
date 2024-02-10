R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

// --- simplex 2D, taken from https://www.shadertoy.com/view/Msf3WH
// >>>
// The MIT License Copyright © 2013 Inigo Quilez Permission is hereby
// granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without
// limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject
// to the following conditions: The above copyright notice and this
// permission notice shall be included in all copies or substantial
// portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT
// WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.
// https://www.youtube.com/c/InigoQuilez https://iquilezles.org

float2 hash2(float2 p) // replace this by something better
{
  p = (float2)(dot(p, (float2)(127.1f, 311.7f)),
               dot(p, (float2)(269.5f, 183.3f)));
  float2 r;
  return -1.f + 2.f * fract(sin(p) * 43758.5453123f, &r);
}

float noise_simplex(const float2 p)
{
  const float K1 = 0.366025404f; // (sqrt(3)-1)/2;
  const float K2 = 0.211324865f; // (3-sqrt(3))/6;

  float2 i = floor(p + (p.x + p.y) * K1);
  float2 a = p - i + (i.x + i.y) * K2;
  float  m = step(a.y, a.x);
  float2 o = (float2)(m, 1.f - m);
  float2 b = a - o + K2;
  float2 c = a - 1.f + 2.f * K2;
  float3 h = max(0.5f - (float3)(dot(a, a), dot(b, b), dot(c, c)), 0.f);
  float3 n = h * h * h * h *
             (float3)(dot(a, hash2(i + 0.f)),
                      dot(b, hash2(i + o)),
                      dot(c, hash2(i + 1.f)));
  return dot(n, (float3)(70.f));
}
// <<<

void kernel simplex(global float *output,
                    const float   kx,
                    const float   ky,
                    const uint    seed,
                    const int     nx,
                    const int     ny)
{
  int2 g = {get_global_id(0), get_global_id(1)};
  int  index = linear_index(g.x, g.y, ny);

  float2 pos = (float2)(0.5f * kx * (float)g.x / nx,
                        0.5f * ky * (float)g.y / ny);

  output[index] = noise_simplex(pos);
}
)""
