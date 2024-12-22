R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void kernel thinning(read_only image2d_t  in,
                     write_only image2d_t out,
                     const int            nx,
                     const int            ny,
                     const int            iter)
{
  const int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                            CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

  float marker = 0.f;

  int i = g.x;
  int j = g.y;

  int a = (read_imagef(in, sampler, (int2)(i - 1, j)).x == 0.f &&
           read_imagef(in, sampler, (int2)(i - 1, j + 1)).x == 1.f) +
          (read_imagef(in, sampler, (int2)(i - 1, j + 1)).x == 0.f &&
           read_imagef(in, sampler, (int2)(i, j + 1)).x == 1.f) +
          (read_imagef(in, sampler, (int2)(i, j + 1)).x == 0.f &&
           read_imagef(in, sampler, (int2)(i + 1, j + 1)).x == 1.f) +
          (read_imagef(in, sampler, (int2)(i + 1, j + 1)).x == 0.f &&
           read_imagef(in, sampler, (int2)(i + 1, j)).x == 1.f) +
          (read_imagef(in, sampler, (int2)(i + 1, j)).x == 0.f &&
           read_imagef(in, sampler, (int2)(i + 1, j - 1)).x == 1.f) +
          (read_imagef(in, sampler, (int2)(i + 1, j - 1)).x == 0.f &&
           read_imagef(in, sampler, (int2)(i, j - 1)).x == 1.f) +
          (read_imagef(in, sampler, (int2)(i, j - 1)).x == 0.f &&
           read_imagef(in, sampler, (int2)(i - 1, j - 1)).x == 1.f) +
          (read_imagef(in, sampler, (int2)(i - 1, j - 1)).x == 0.f &&
           read_imagef(in, sampler, (int2)(i - 1, j)).x == 1.f);

  int b = read_imagef(in, sampler, (int2)(i - 1, j)).x +
          read_imagef(in, sampler, (int2)(i - 1, j + 1)).x +
          read_imagef(in, sampler, (int2)(i, j + 1)).x +
          read_imagef(in, sampler, (int2)(i + 1, j + 1)).x +
          read_imagef(in, sampler, (int2)(i + 1, j)).x +
          read_imagef(in, sampler, (int2)(i + 1, j - 1)).x +
          read_imagef(in, sampler, (int2)(i, j - 1)).x +
          read_imagef(in, sampler, (int2)(i - 1, j - 1)).x;

  int m1 = iter == 0 ? (read_imagef(in, sampler, (int2)(i - 1, j)).x *
                        read_imagef(in, sampler, (int2)(i, j + 1)).x *
                        read_imagef(in, sampler, (int2)(i + 1, j)).x)
                     : (read_imagef(in, sampler, (int2)(i - 1, j)).x *
                        read_imagef(in, sampler, (int2)(i, j + 1)).x *
                        read_imagef(in, sampler, (int2)(i, j - 1)).x);

  int m2 = iter == 0 ? (read_imagef(in, sampler, (int2)(i, j + 1)).x *
                        read_imagef(in, sampler, (int2)(i + 1, j)).x *
                        read_imagef(in, sampler, (int2)(i, j - 1)).x)
                     : (read_imagef(in, sampler, (int2)(i - 1, j)).x *
                        read_imagef(in, sampler, (int2)(i + 1, j)).x *
                        read_imagef(in, sampler, (int2)(i, j - 1)).x);

  if (a == 1 && (b >= 2 && b <= 6) && m1 == 0 && m2 == 0) marker = 1.f;

  float val = read_imagef(in, sampler, g).x;

  write_imagef(out, g, val * (1.f - marker));
}

void kernel relative_distance_from_skeleton(read_only image2d_t  array,
                                            read_only image2d_t  sk,
                                            read_only image2d_t  border,
                                            write_only image2d_t rdist,
                                            const int            nx,
                                            const int            ny,
                                            const int            ir)
{
  const int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                            CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

  if (read_imagef(array, sampler, g).x == 0.f) return;

  float dmax_sk = FLT_MAX;
  float dmax_bd = FLT_MAX;

  for (int p = g.x - ir; p < g.x + ir + 1; p++)
    for (int q = g.y - ir; q < g.y + ir + 1; q++)
    {
      // distance to skeleton
      if (read_imagef(sk, sampler, (int2)(p, q)).x == 1.f)
      {
        float d2 = (float)((g.x - p) * (g.x - p) + (g.y - q) * (g.y - q));
        if (d2 < dmax_sk) dmax_sk = d2;
      }

      // distance to border
      if (read_imagef(border, sampler, (int2)(p, q)).x == 1.f)
      {
        float d2 = (float)((g.x - p) * (g.x - p) + (g.y - q) * (g.y - q));
        if (d2 < dmax_bd) dmax_bd = d2;
      }
    }

  // relative distance (from 1.f on the skeleton to 0.f and
  // the border)
  float sum = dmax_bd + dmax_sk;
  float val = sum > 0.f ? dmax_bd / sum : 0.f;

  write_imagef(rdist, g, val);
}
)""
