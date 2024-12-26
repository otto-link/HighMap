R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

// taken from https://www.shadertoy.com/view/llG3zy
float3 voronoi_edge_distance_fct(const float2 x,
                                 const float2 jitter,
                                 const float  fseed)
{
  float2 n = floor(x);
  float2 fi;
  float2 f = fract(x, &fi);

  // first pass: regular voronoi
  float2 mg, mr;

  float md = 8.f;
  for (int j = -1; j <= 1; j++)
    for (int i = -1; i <= 1; i++)
    {
      float2 g = (float2)(i, j);
      float2 o = jitter * hash22f(n + g, fseed);
      float2 r = g + o - f;
      float  d = dot(r, r);

      if (d < md)
      {
        md = d;
        mr = r;
        mg = g;
      }
    }

  // second pass: distance to borders
  md = 8.f;
  for (int j = -2; j <= 2; j++)
    for (int i = -2; i <= 2; i++)
    {
      float2 g = mg + (float2)(i, j);
      float2 o = jitter * hash22f(n + g, fseed);
      float2 r = g + o - f;

      if (dot(mr - r, mr - r) > 0.00001f)
        md = min(md, dot(0.5f * (mr + r), normalize(r - mr)));
    }

  return (float3)(md, mr);
}

void kernel voronoi_edge_distance(global float *output,
                                  global float *ctrl_param,
                                  global float *noise_x,
                                  global float *noise_y,
                                  const int     nx,
                                  const int     ny,
                                  const float   kx,
                                  const float   ky,
                                  const uint    seed,
                                  const float2  jitter,
                                  const int     has_ctrl_param,
                                  const int     has_noise_x,
                                  const int     has_noise_y,
                                  const float4  bbox)
{
  float width_ratio = 0.1f;

  int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  int index = linear_index(g.x, g.y, nx);

  uint  rng_state = wang_hash(seed);
  float fseed = rand(&rng_state);

  float ct = has_ctrl_param > 0 ? ctrl_param[index] : 1.f;
  float dx = has_noise_x > 0 ? noise_x[index] : 0.f;
  float dy = has_noise_y > 0 ? noise_y[index] : 0.f;

  float2 pos = g_to_xy(g, nx, ny, kx, ky, dx, dy, bbox);

  float d = voronoi_edge_distance_fct(pos, ct * jitter, fseed).x;

  output[index] = d;
}
)""
