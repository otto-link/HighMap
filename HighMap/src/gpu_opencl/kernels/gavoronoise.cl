R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
float helper_gavoronoise_base_fbm(const float2 p, const float fseed)
{
  float v = gabor_wave_scalar_fbm(p, 8, 1.f, 0.5f, 2.f, fseed);
  return v;
}

// from https://www.shadertoy.com/view/llsGWl

// from https://www.shadertoy.com/view/MtGcWh
float3 gavoronoise_eroder(const float2 p, const float2 dir, const float fseed)
{
  float2 ip = floor(p);
  float2 fi;
  float2 fp = fract(p, &fi);
  float  f = 2.f * 3.1415f;
  float3 va = 0.f;
  float  wt = 0.f;
  for (int i = -2; i <= 2; i++)
    for (int j = -2; j <= 2; j++)
    {
      float2 o = (float2)(i, j);
      float2 h = hash2f(ip - o + (float2)(fseed, fseed)) * 0.5f;
      float2 pp = fp + o - h;
      float  d = dot(pp, pp);
      float  w = exp(-d * 2.f);
      wt += w;
      float mag = dot(pp, dir);
      va += (float3)(cos(mag * f), -sin(mag * f) * (pp * 0.f + dir)) * w;
    }
  return va / wt;
}

float gavoronoise_eroder_fbm(const float2 p,
                             const float  base,
                             const float2 kw_multiplier,
                             const float2 dir,
                             const float  branch_strength,
                             const float  amplitude,
                             const float  z_cut_min,
                             const float  z_cut_max,
                             const int    octaves,
                             const float  persistence,
                             const float  lacunarity,
                             const float  fseed)
{
  float3 h = 0.f;
  float  a = 0.6f * smoothstep3_gl(base * 0.5f + 0.5f, z_cut_min, z_cut_max);

  float f = 1.f;
  for (int i = 0; i < octaves; i++)
  {
    float2 new_dir = h.zy * (float2)(1.f, -1.f);

    h += a * gavoronoise_eroder(p * kw_multiplier * f, dir + new_dir, fseed) *
         (float3)(1.f, f, f) * branch_strength;
    a *= persistence;
    f *= lacunarity;
  }

  return base + h.x * amplitude;
}

void kernel gavoronoise(global float *output,
                        global float *ctrl_param,
                        global float *noise_x,
                        global float *noise_y,
                        const int     nx,
                        const int     ny,
                        const float   kx,
                        const float   ky,
                        const uint    seed,
                        const float   amplitude,
                        const float2  kw_multiplier,
                        const float   slope_strength,
                        const float   branch_strength,
                        const float   z_cut_min,
                        const float   z_cut_max,
                        const int     octaves,
                        const float   persistence,
                        const float   lacunarity,
                        const int     has_ctrl_param,
                        const int     has_noise_x,
                        const int     has_noise_y,
                        const float4  bbox)
{
  int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  int index = linear_index(g.x, g.y, ny);

  uint  rng_state = wang_hash(seed);
  float fseed = rand(&rng_state);

  float ct = has_ctrl_param > 0 ? ctrl_param[index] : 1.f;
  float dx = has_noise_x > 0 ? noise_x[index] : 0.f;
  float dy = has_noise_y > 0 ? noise_y[index] : 0.f;

  float2 pos = g_to_xy(g, nx, ny, kx, ky, dx, dy, bbox);

  float base = helper_gavoronoise_base_fbm(pos, fseed);

  float eps = 0.1f;
  float mx = helper_gavoronoise_base_fbm(pos + (float2)(eps, 0.0), fseed) -
             helper_gavoronoise_base_fbm(pos - (float2)(eps, 0.0), fseed);
  float my = helper_gavoronoise_base_fbm(pos + (float2)(0.f, eps), fseed) -
             helper_gavoronoise_base_fbm(pos - (float2)(0.f, eps), fseed);

  float2 dir = (float2)(my / eps * 0.5f, -mx / eps * 0.5f) * slope_strength;

  output[index] = gavoronoise_eroder_fbm(pos,
                                         base,
                                         kw_multiplier,
                                         dir,
                                         branch_strength,
                                         amplitude,
                                         z_cut_min,
                                         z_cut_max * ct,
                                         octaves,
                                         persistence,
                                         lacunarity,
                                         fseed);

  // output[index] = base;
}
)""
