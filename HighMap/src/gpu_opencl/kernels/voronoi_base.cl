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

float base_voronoi_f1tf2(const float2 p,
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

// https://iquilezles.org/articles/voronoilines/
float base_voronoi_edge_distance(const float2 x,
                                 const float2 jitter,
                                 const bool   return_sqrt,
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
        res = min(res, d);
      }
    }

  if (return_sqrt)
    return sqrt(res);
  else
    return res;
}

float base_voronoi_f1df2(const float2 p,
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
    return sqrt(min1 / min2) - 1.f;
  else
    return min1 / min2 - 1.f;
}

float base_voronoi_f2mf1(const float2 p,
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
    return sqrt(min2 - min1) - 1.f;
  else
    return min2 - min1 - 1.f;
}
)""
