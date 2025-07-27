R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

float base_voronoi_f1(const float2 p,
                      const float2 jitter,
                      const float  k_smoothing,
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

      min_dist = smin(min_dist, dist, k_smoothing);
    }

  // NB - squared distance
  return 1.66f * min_dist - 1.f;
}

float base_voronoi_f2(const float2 p,
                      const float2 jitter,
                      const float  k_smoothing,
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

      float new_min1 = smin(min1, dist, k_smoothing);
      float new_min2 = smin(min2, smax(min1, dist, k_smoothing), k_smoothing);
      min1 = new_min1;
      min2 = new_min2;
    }

  return min2 - 1.f;
}

float base_voronoi_f1tf2(const float2 p,
                         const float2 jitter,
                         const float  k_smoothing,
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

      float new_min1 = smin(min1, dist, k_smoothing);
      float new_min2 = smin(min2, smax(min1, dist, k_smoothing), k_smoothing);
      min1 = new_min1;
      min2 = new_min2;
    }

  return min1 * min2 - 1.f;
}

float base_voronoi_f1df2(const float2 p,
                         const float2 jitter,
                         const float  k_smoothing,
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

      float new_min1 = smin(min1, dist, k_smoothing);
      float new_min2 = smin(min2, smax(min1, dist, k_smoothing), k_smoothing);
      min1 = new_min1;
      min2 = new_min2;
    }

  return min1 / min2 - 1.f;
}

float base_voronoi_f2mf1(const float2 p,
                         const float2 jitter,
                         const float  k_smoothing,
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

      float new_min1 = smin(min1, dist, k_smoothing);
      float new_min2 = smin(min2, smax(min1, dist, k_smoothing), k_smoothing);
      min1 = new_min1;
      min2 = new_min2;
    }

  return min2 - min1 - 1.f;
}

// https://iquilezles.org/articles/voronoilines/
float base_voronoi_edge_distance(const float2 x,
                                 const float2 jitter,
                                 const float  k_smoothing,
                                 const float  fseed)
{
  float2 p = floor(x);
  float2 pi;
  float2 f = fract(x, &pi);

  float2 mb;
  float2 mr;
  float2 df = (float2)(0.1f, 0.1f);

  float res = 8.f;
  for (int j = -1; j <= 1; j++)
    for (int i = -1; i <= 1; i++)
    {
      float2 b = (float2)(i, j);
      float2 r = b - f +
                 jitter * (float2)(hash12f(p + b, fseed),
                                   hash12f(p + b + df, fseed));
      float d = dot(r, r);

      if (d < res)
      {
        res = d;
        mr = r;
        mb = b;
      }
    }

  res = 8.f;
  for (int j = -2; j <= 2; j++)
    for (int i = -2; i <= 2; i++)
    {
      float2 b = mb + (float2)(i, j);
      float2 r = b - f +
                 jitter * (float2)(hash12f(p + b, fseed),
                                   hash12f(p + b + df, fseed));
      if (dot(mr - r, mr - r) > 1e-5f)
      {
        float d = dot(0.5f * (mr + r), normalize(r - mr));
        res = smin(res, d, k_smoothing);
      }
    }

  return res;
}

float base_voronoi_edge_distance_exp(const float2 x,
                                     const float2 jitter,
                                     const float  k_smoothing,
                                     const float  exp_sigma,
                                     const float  fseed)
{
  float res = base_voronoi_edge_distance(x, jitter, k_smoothing, fseed);
  return exp(-0.5f * res * res / (exp_sigma * exp_sigma));
}

float base_voronoi_constant(const float2 p,
                            const float2 jitter,
                            const float  k_smoothing,
                            const float  fseed)
{
  float2 i = floor(p);
  float2 pi;
  float2 f = fract(p, &pi);

  float min_dist = FLT_MAX;
  float res = 0.f;

  for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++)
    {
      float2 neighbor = i + (float2)(dx, dy);
      float2 df = (float2)(0.1f, 0.1f);
      float  rx = hash12f(neighbor, fseed);
      float2 feature_point = neighbor +
                             jitter *
                                 (float2)(rx, hash12f(neighbor + df, fseed));

      float2 diff = p - feature_point;
      float  dist = dot(diff, diff);

      // https://www.shadertoy.com/view/ldB3zc
      float h = smoothstep(-1.f, 1.f, (min_dist - dist) / k_smoothing);
      res = lerp(res, rx, h) -
            h * (1.f - h) * k_smoothing / (1.f + 3.f * k_smoothing);

      min_dist = min(dist, min_dist);
    }

  return res;
}

float base_voronoi_constant_f2mf1(const float2 p,
                                  const float2 jitter,
                                  const float  k_smoothing,
                                  const float  fseed)
{
  float2 i = floor(p);
  float2 pi;
  float2 f = fract(p, &pi);

  float min1 = FLT_MAX;
  float min2 = FLT_MAX;

  float min_dist = FLT_MAX;
  float res = 0.f;

  for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++)
    {
      float2 neighbor = i + (float2)(dx, dy);
      float2 df = (float2)(0.1f, 0.1f);
      float  rx = hash12f(neighbor, fseed);
      float2 feature_point = neighbor +
                             jitter *
                                 (float2)(rx, hash12f(neighbor + df, fseed));

      float2 diff = p - feature_point;
      float  dist = dot(diff, diff);

      float new_min1 = smin(min1, dist, k_smoothing);
      float new_min2 = smin(min2, smax(min1, dist, k_smoothing), k_smoothing);
      min1 = new_min1;
      min2 = new_min2;

      // smooth cell value
      float h = smoothstep(-1.f, 1.f, (min_dist - dist) / k_smoothing);
      res = lerp(res, rx, h) -
            h * (1.f - h) * k_smoothing / (1.f + 3.f * k_smoothing);
      min_dist = min(dist, min_dist);
    }

  return res * (min2 - min1 - 1.f);
}
)""
