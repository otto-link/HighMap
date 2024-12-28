R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void kernel ruggedness(read_only image2d_t  array,
                       write_only image2d_t out,
                       const int            nx,
                       const int            ny,
                       const int            ir)
{
  const int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                            CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

  float val = 0.f;

  for (int p = g.x - ir; p < g.x + ir + 1; p++)
    for (int q = g.y - ir; q < g.y + ir + 1; q++)
    {
      float delta = read_imagef(array, sampler, g).x -
                    read_imagef(array, sampler, (int2)(p, q)).x;
      val += delta * delta;
    }

  write_imagef(out, g, sqrt(val));
}
)""
