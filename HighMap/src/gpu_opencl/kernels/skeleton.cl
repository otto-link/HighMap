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

  if (g.x < 1 || g.x >= nx || g.y < 1 || g.y >= ny) return;

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
)""
