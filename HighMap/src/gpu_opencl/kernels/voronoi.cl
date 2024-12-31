R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

float base_voronoi_f1(const float2 p,
                      const float2 jitter,
                      const bool   return_sqrt,
                      const float  fseed)
{
  float2 i = floor(p);
  float2 pi;
  float2 f = fract(p, &pi);

  float min_dist = FLT_MAX;

  for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++)
    {
      float2 neighbor = i + (float2)(dx, dy);
      float2 df = (float2)(0.1f, 0.1f);
      float2 feature_point = neighbor +
                             jitter * (float2)(hash12f(neighbor, fseed),
                                               hash12f(neighbor + df, fseed));

      float2 diff = p - feature_point;
      float  dist = dot(diff, diff);
      min_dist = min(min_dist, dist);
    }

  if (return_sqrt) min_dist = sqrt(min_dist);

  // NB - squared distance
  return 1.66f * min_dist - 1.f;
}

float base_voronoi_f2(const float2 p,
                      const float2 jitter,
                      const bool   return_sqrt,
                      const float  fseed)
{
  float2 i = floor(p);
  float2 pi;
  float2 f = fract(p, &pi);

  float min1 = FLT_MAX;
  float min2 = FLT_MAX;

  for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++)
    {
      float2 neighbor = i + (float2)(dx, dy);
      float2 df = (float2)(0.1f, 0.1f);
      float2 feature_point = neighbor +
                             jitter * (float2)(hash12f(neighbor, fseed),
                                               hash12f(neighbor + df, fseed));

      float2 diff = p - feature_point;
      float  dist = dot(diff, diff);

      if (dist < min1)
      {
        min2 = min1;
        min1 = dist;
      }
      else if (dist < min2)
      {
        min2 = dist;
      }
    }

  if (return_sqrt) min2 = sqrt(min2);

  return min2 - 1.f;
}

float base_voronoi_f1f2(const float2 p,
                        const float2 jitter,
                        const bool   return_sqrt,
                        const float  fseed)
{
  float2 i = floor(p);
  float2 pi;
  float2 f = fract(p, &pi);

  float min1 = FLT_MAX;
  float min2 = FLT_MAX;

  for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++)
    {
      float2 neighbor = i + (float2)(dx, dy);
      float2 df = (float2)(0.1f, 0.1f);
      float2 feature_point = neighbor +
                             jitter * (float2)(hash12f(neighbor, fseed),
                                               hash12f(neighbor + df, fseed));

      float2 diff = p - feature_point;
      float  dist = dot(diff, diff);

      if (dist < min1)
      {
        min2 = min1;
        min1 = dist;
      }
      else if (dist < min2)
      {
        min2 = dist;
      }
    }

  if (return_sqrt)
    return sqrt(min1 * min2) - 1.f;
  else
    return min1 * min2 - 1.f;
}

float base_voronoi_f1_fbm(const float2 p,
                          const float2 jitter,
                          const bool   return_sqrt,
                          const int    octaves,
                          const float  weight,
                          const float  persistence,
                          const float  lacunarity,
                          const float  fseed)
{
  float n = 0.f;
  float nf = 1.f;
  float na = 0.6f;
  for (int i = 0; i < octaves; i++)
  {
    float v = base_voronoi_f1(p * nf, jitter, return_sqrt, fseed);
    n += v * na;
    na *= (1.f - weight) + weight * min(v + 1.f, 2.f) * 0.5f;
    na *= persistence;
    nf *= lacunarity;
  }
  return n;
}

float base_voronoi_f2_fbm(const float2 p,
                          const float2 jitter,
                          const bool   return_sqrt,
                          const int    octaves,
                          const float  weight,
                          const float  persistence,
                          const float  lacunarity,
                          const float  fseed)
{
  float n = 0.f;
  float nf = 1.f;
  float na = 0.6f;
  for (int i = 0; i < octaves; i++)
  {
    float v = base_voronoi_f2(p * nf, jitter, return_sqrt, fseed);
    n += v * na;
    na *= (1.f - weight) + weight * min(v + 1.f, 2.f) * 0.5f;
    na *= persistence;
    nf *= lacunarity;
  }
  return n;
}

float base_voronoi_f1f2_fbm(const float2 p,
                            const float2 jitter,
                            const bool   return_sqrt,
                            const int    octaves,
                            const float  weight,
                            const float  persistence,
                            const float  lacunarity,
                            const float  fseed)
{
  float n = 0.f;
  float nf = 1.f;
  float na = 0.6f;
  for (int i = 0; i < octaves; i++)
  {
    float v = base_voronoi_f1f2(p * nf, jitter, return_sqrt, fseed);
    n += v * na;
    na *= (1.f - weight) + weight * min(v + 1.f, 2.f) * 0.5f;
    na *= persistence;
    nf *= lacunarity;
  }
  return n;
}

void kernel voronoi(global float *output,
                    global float *ctrl_param,
                    global float *noise_x,
                    global float *noise_y,
                    const int     nx,
                    const int     ny,
                    const float   kx,
                    const float   ky,
                    const uint    seed,
                    const float2  jitter,
                    const int     return_type,
                    const int     has_ctrl_param,
                    const int     has_noise_x,
                    const int     has_noise_y,
                    const float4  bbox)
{
  int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  int index = linear_index(g.x, g.y, nx);

  uint  rng_state = wang_hash(seed);
  float fseed = rand(&rng_state);

  float ct = has_ctrl_param > 0 ? ctrl_param[index] : 1.f;
  float dx = has_noise_x > 0 ? noise_x[index] : 0.f;
  float dy = has_noise_y > 0 ? noise_y[index] : 0.f;

  float2 pos = g_to_xy(g, nx, ny, kx, ky, dx, dy, bbox);

  float val = 0.f;

  switch (return_type)
  {
  case 0: val = base_voronoi_f1(pos, ct * jitter, true, fseed); break;
  case 1: val = base_voronoi_f1(pos, ct * jitter, false, fseed); break;
  case 2: val = base_voronoi_f2(pos, ct * jitter, true, fseed); break;
  case 3: val = base_voronoi_f2(pos, ct * jitter, false, fseed); break;
  case 4: val = base_voronoi_f1f2(pos, ct * jitter, true, fseed); break;
  case 5: val = base_voronoi_f1f2(pos, ct * jitter, false, fseed); break;
  }

  output[index] = val;
}

void kernel voronoi_fbm(global float *output,
                        global float *ctrl_param,
                        global float *noise_x,
                        global float *noise_y,
                        const int     nx,
                        const int     ny,
                        const float   kx,
                        const float   ky,
                        const uint    seed,
                        const float2  jitter,
                        const int     return_type,
                        const int     octaves,
                        const float   weight,
                        const float   persistence,
                        const float   lacunarity,
                        const int     has_ctrl_param,
                        const int     has_noise_x,
                        const int     has_noise_y,
                        const float4  bbox)
{
  int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  int index = linear_index(g.x, g.y, nx);

  uint  rng_state = wang_hash(seed);
  float fseed = rand(&rng_state);

  float ct = has_ctrl_param > 0 ? ctrl_param[index] : 1.f;
  float dx = has_noise_x > 0 ? noise_x[index] : 0.f;
  float dy = has_noise_y > 0 ? noise_y[index] : 0.f;

  float2 pos = g_to_xy(g, nx, ny, kx, ky, dx, dy, bbox);

  float val = 0.f;

  switch (return_type)
  {
  case 0:
    val = base_voronoi_f1_fbm(pos,
                              ct * jitter,
                              true,
                              octaves,
                              weight,
                              persistence,
                              lacunarity,
                              fseed);
    break;
  case 1:
    val = base_voronoi_f1_fbm(pos,
                              ct * jitter,
                              false,
                              octaves,
                              weight,
                              persistence,
                              lacunarity,
                              fseed);
    break;
  case 2:
    val = base_voronoi_f2_fbm(pos,
                              ct * jitter,
                              true,
                              octaves,
                              weight,
                              persistence,
                              lacunarity,
                              fseed);
    break;
  case 3:
    val = base_voronoi_f2_fbm(pos,
                              ct * jitter,
                              false,
                              octaves,
                              weight,
                              persistence,
                              lacunarity,
                              fseed);
    break;
  case 4:
    val = base_voronoi_f1f2_fbm(pos,
                                ct * jitter,
                                true,
                                octaves,
                                weight,
                                persistence,
                                lacunarity,
                                fseed);
    break;
  case 5:
    val = base_voronoi_f1f2_fbm(pos,
                                ct * jitter,
                                false,
                                octaves,
                                weight,
                                persistence,
                                lacunarity,
                                fseed);
    break;
  }

  output[index] = val;
}
)""
