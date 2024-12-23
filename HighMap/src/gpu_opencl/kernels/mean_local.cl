R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void kernel mean_local(read_only image2d_t  in,
                       write_only image2d_t out,
                       const int            nx,
                       const int            ny,
                       const int            ir)
{
  const int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                            CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

  float sum = 0.f;

  for (int k = -ir; k < ir + 1; k++)
    for (int r = -ir; r < ir + 1; r++)
    {
      sum += read_imagef(in, sampler, (int2)(g.x + k, g.y + r)).x;
    }

  float nvalues = (2.f * ir + 1.f) * (2.f * ir + 1.f);
  sum /= nvalues;

  write_imagef(out, g, sum);
}
)""
