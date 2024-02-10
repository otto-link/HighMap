R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

// --- Voronoise, taken from https://www.shadertoy.com/view/Xd23Dh
// >>>

// The MIT License https://www.youtube.com/c/InigoQuilez
// https://iquilezles.org/ Copyright © 2014 Inigo Quilez Permission is
// hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without
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

float3 hash3(float2 p)
{
  float3 q = (float3)(dot(p, (float2)(127.1f, 311.7f)),
                      dot(p, (float2)(269.5f, 183.3f)),
                      dot(p, (float2)(419.2f, 371.9f)));
  float3 qi;
  return fract(sin(q) * 43758.5453f, &qi);
}

float noise_voronoise(float2 p, float u, float v, float fseed)
{
  float k = 1.f + 63.f * pow(1.f - v, 6.f);

  float2 i = floor(p);
  float2 pi;
  float2 f = fract(p, &pi);

  float2 a = (float2)(0.f, 0.f);
  for (int y = -2; y <= 2; y++)
    for (int x = -2; x <= 2; x++)
    {
      float2 g = (float2)(x, y);
      float3 o = hash3(i + g + fseed) * (float3)(u, u, 1.f);
      float2 d = g - f + o.xy;
      float  w = pow(1.f - smoothstep(0.f, 1.414f, length(d)), k);
      a += (float2)(o.z * w, w);
    }

  return a.x / a.y;
}
// <<<

void kernel voronoise(global float *output,
                      const float   kx,
                      const float   ky,
                      const float   u_param,
                      const float   v_param,
                      const uint    seed,
                      const int     nx,
                      const int     ny)
{
  int2 g = {get_global_id(0), get_global_id(1)};
  int  index = linear_index(g.x, g.y, ny);

  uint  rng_state = wang_hash(seed);
  float fseed = rand(&rng_state);

  float2 pos = (float2)(kx * (float)g.x / nx, ky * (float)g.y / ny);

  output[index] = noise_voronoise(pos, u_param, v_param, fseed);
}
)""
