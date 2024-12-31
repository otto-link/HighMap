R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void kernel maximum_local(read_only image2d_t  in,
                          write_only image2d_t out,
                          const int            nx,
                          const int            ny,
                          const int            ir,
                          const int            pass_number)
{
  const int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                            CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

  float max = read_imagef(in, sampler, g).x;

  if (pass_number == 0)
  { // row
    for (int u = g.x - ir; u < g.x + ir + 1; u++)
    {
      float value = read_imagef(in, sampler, (int2)(u, g.y)).x;
      if (value > max) max = value;
    }
  }
  else
  { // col
    for (int v = g.y - ir; v < g.y + ir + 1; v++)
    {
      float value = read_imagef(in, sampler, (int2)(g.x, v)).x;
      if (value > max) max = value;
    }
  }

  write_imagef(out, g, max);
}
)""
