R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

float base_perlin(float2 p, float fseed)
{
  // lattice points
  float2 i = floor(p);
  float2 pi;
  float2 f = fract(p, &pi);

  // gradients at lattice corners
  float2 g00 = grad22f(i + (float2)(0.0f, 0.0f), fseed);
  float2 g10 = grad22f(i + (float2)(1.0f, 0.0f), fseed);
  float2 g01 = grad22f(i + (float2)(0.0f, 1.0f), fseed);
  float2 g11 = grad22f(i + (float2)(1.0f, 1.0f), fseed);

  // offset vectors to corners
  float2 d00 = f - (float2)(0.0f, 0.0f);
  float2 d10 = f - (float2)(1.0f, 0.0f);
  float2 d01 = f - (float2)(0.0f, 1.0f);
  float2 d11 = f - (float2)(1.0f, 1.0f);

  // dot products between gradients and offset vectors
  float n00 = dot(g00, d00);
  float n10 = dot(g10, d10);
  float n01 = dot(g01, d01);
  float n11 = dot(g11, d11);

  // fade curve for interpolation
  float2 u = smoothstep5_f2(f);

  // bilinear interpolation
  float nx0 = lerp(n00, n10, u.x);
  float nx1 = lerp(n01, n11, u.x);
  float nxy = lerp(nx0, nx1, u.y);

  // return noise value
  return 1.42857f * nxy;
}

float base_simplex2(float2 p, float fseed)
{
  const float K1 = 0.366025403f; // (sqrt(3)-1)/2
  const float K2 = 0.211324865f; // (3-sqrt(3))/6

  // skew the input space to find the simplex cell
  float2 i = floor(p + dot(p, (float2)(K1, K1)));
  float2 a = p - i + dot(i, (float2)(K2, K2));

  // determine which simplex we are in
  float  m = step(a.y, a.x);
  float2 o = (float2)(m, 1.0f - m);
  float2 b = a - o + (float2)(K2, K2);
  float2 c = a - 1.0f + 2.0f * (float2)(K2, K2);

  // compute gradients at each vertex
  float2 g0 = grad22f(i, fseed);
  float2 g1 = grad22f(i + o, fseed);
  float2 g2 = grad22f(i + (float2)(1.0f, 1.0f), fseed);

  // compute contributions from each vertex
  float n0 = max(0.5f - dot(a, a), 0.0f);
  float n1 = max(0.5f - dot(b, b), 0.0f);
  float n2 = max(0.5f - dot(c, c), 0.0f);

  n0 = n0 * n0 * n0 * n0 * dot(g0, a);
  n1 = n1 * n1 * n1 * n1 * dot(g1, b);
  n2 = n2 * n2 * n2 * n2 * dot(g2, c);

  // sum contributions and scale result (100.0 = 70.0 / 0.7)
  return 100.f * (n0 + n1 + n2);
}

float base_value(float2 p, float fseed)
{
  float2 i = floor(p);
  float2 pi;
  float2 f = fract(p, &pi);

  float v00 = hash12f(i + (float2)(0.0f, 0.0f), fseed);
  float v10 = hash12f(i + (float2)(1.0f, 0.0f), fseed);
  float v01 = hash12f(i + (float2)(0.0f, 1.0f), fseed);
  float v11 = hash12f(i + (float2)(1.0f, 1.0f), fseed);

  float2 u = smoothstep3_f2(f);

  float nx0 = lerp(v00, v10, u.x);
  float nx1 = lerp(v01, v11, u.x);
  float nxy = lerp(nx0, nx1, u.y);

  return 2.f * nxy - 1.f;
}

float base_value_cubic(float2 p, float fseed)
{
  float2 i = floor(p);
  float2 pi;
  float2 f = fract(p, &pi);

  // collect 16 lattice values around the point
  float v[4][4];
  for (int dy = -1; dy <= 2; dy++)
    for (int dx = -1; dx <= 2; dx++)
      v[dy + 1][dx + 1] = hash12f(i + (float2)(dx, dy), fseed);

  // cubic interpolation in the x-direction for each row
  float interp_row[4];
  for (int dy = 0; dy < 4; dy++)
    interp_row[dy] = cubic_interp(v[dy][0], v[dy][1], v[dy][2], v[dy][3], f.x);

  // cubic interpolation in the y-direction
  float value = cubic_interp(interp_row[0],
                             interp_row[1],
                             interp_row[2],
                             interp_row[3],
                             f.y);

  return 1.43f * (value - 0.5f);
}

float base_value_linear(float2 p, float fseed)
{
  float2 i = floor(p);
  float2 pi;
  float2 f = fract(p, &pi);

  float v00 = hash12f(i + (float2)(0.0f, 0.0f), fseed);
  float v10 = hash12f(i + (float2)(1.0f, 0.0f), fseed);
  float v01 = hash12f(i + (float2)(0.0f, 1.0f), fseed);
  float v11 = hash12f(i + (float2)(1.0f, 1.0f), fseed);

  float nx0 = lerp(v00, v10, f.x);
  float nx1 = lerp(v01, v11, f.x);
  float nxy = lerp(nx0, nx1, f.y);

  return 2.f * nxy - 1.f;
}

float base_worley(float2 p, float fseed)
{
  float2 i = floor(p);
  float2 pi;
  float2 f = fract(p, &pi);

  float min_dist = FLT_MAX;

  for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++)
    {
      float2 neighbor = i + (float2)(dx, dy);
      float2 feature_point = neighbor +
                             (float2)(hash12f(neighbor, fseed),
                                      hash12f(neighbor + (float2)(0.1f, 0.1f),
                                              fseed));

      float2 diff = p - feature_point;
      float  dist = dot(diff, diff);
      min_dist = min(min_dist, dist);
    }

  // NB - squared distance
  return 1.66f * min_dist - 1.f;
}

void kernel noise(global float *output,
                  global float *noise_x,
                  global float *noise_y,
                  const int     nx,
                  const int     ny,
                  const int     noise_id,
                  float         kx,
                  float         ky,
                  const uint    seed,
                  const int     has_noise_x,
                  const int     has_noise_y,
                  const float4  bbox)
{
  int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  int index = linear_index(g.x, g.y, ny);

  uint  rng_state = wang_hash(seed);
  float fseed = rand(&rng_state);

  float dx = has_noise_x > 0 ? noise_x[index] : 0.f;
  float dy = has_noise_y > 0 ? noise_y[index] : 0.f;

  // for SIMPLEX noises
  if (noise_id == 4 || noise_id == 5)
  {
    kx *= 0.5f;
    ky *= 0.5f;
  }

  float2 pos = g_to_xy(g, nx, ny, kx, ky, dx, dy, bbox);

  if (noise_id == 1)
  {
    output[index] = base_perlin(pos, fseed);
  }
  else if (noise_id == 2)
  {
    float value = base_perlin(pos, fseed);
    output[index] = 2.f * fabs(value) - 1.f;
  }
  else if (noise_id == 3)
  {
    float value = base_perlin(pos, fseed);
    output[index] = max_smooth(value, 0.f, 0.5f);
  }
  else if (noise_id == 4)
  {
    output[index] = base_simplex2(pos, fseed);
  }
  else if (noise_id == 6)
  {
    output[index] = base_value(pos, fseed);
  }
  else if (noise_id == 7)
  {
    output[index] = base_value_cubic(pos, fseed);
  }
  else if (noise_id == 9)
  {
    output[index] = base_value_linear(pos, fseed);
  }
  else if (noise_id == 10)
  {
    output[index] = base_worley(pos, fseed);
  }
  else
  {
    output[index] = 0.f;
  }
}
)""
