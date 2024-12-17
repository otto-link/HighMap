R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void kernel normal_displacement(read_only image2d_t  array,
                                read_only image2d_t  array_f,
                                write_only image2d_t out,
                                const int            nx,
                                const int            ny,
                                const float          amount)
{
  int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  const sampler_t sampler_nearest = CLK_NORMALIZED_COORDS_FALSE |
                                    CLK_ADDRESS_CLAMP_TO_EDGE |
                                    CLK_FILTER_NEAREST;

  const sampler_t sampler_interp = CLK_NORMALIZED_COORDS_TRUE |
                                   CLK_ADDRESS_MIRRORED_REPEAT |
                                   CLK_FILTER_LINEAR;

  // normal vector
  float dx = read_imagef(array_f, sampler_nearest, (int2)(g.x + 1, g.y)).x -
             read_imagef(array_f, sampler_nearest, (int2)(g.x - 1, g.y)).x;
  float dy = read_imagef(array_f, sampler_nearest, (int2)(g.x, g.y + 1)).x -
             read_imagef(array_f, sampler_nearest, (int2)(g.x, g.y - 1)).x;

  float3 n = {dx, dy, 1.0};
  n /= length(n);
  n *= amount;

  float2 pos = {(float)g.x / (nx - 1.f) + n.x, (float)g.y / (ny - 1.f) + n.y};
  float  val = read_imagef(array, sampler_interp, pos).x;
  write_imagef(out, g, val);
}

void kernel normal_displacement_masked(read_only image2d_t  array,
                                       read_only image2d_t  array_f,
                                       read_only image2d_t  mask,
                                       write_only image2d_t out,
                                       const int            nx,
                                       const int            ny,
                                       const float          amount)
{
  int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  const sampler_t sampler_nearest = CLK_NORMALIZED_COORDS_FALSE |
                                    CLK_ADDRESS_CLAMP_TO_EDGE |
                                    CLK_FILTER_NEAREST;

  const sampler_t sampler_interp = CLK_NORMALIZED_COORDS_TRUE |
                                   CLK_ADDRESS_MIRRORED_REPEAT |
                                   CLK_FILTER_LINEAR;

  // normal vector
  float dx = read_imagef(array_f, sampler_nearest, (int2)(g.x + 1, g.y)).x -
             read_imagef(array_f, sampler_nearest, (int2)(g.x - 1, g.y)).x;
  float dy = read_imagef(array_f, sampler_nearest, (int2)(g.x, g.y + 1)).x -
             read_imagef(array_f, sampler_nearest, (int2)(g.x, g.y - 1)).x;

  float3 n = {dx, dy, 1.0};
  n /= length(n);
  n *= amount;

  float2 pos = {(float)g.x / (nx - 1.f) + n.x, (float)g.y / (ny - 1.f) + n.y};
  float  val = read_imagef(array, sampler_interp, pos).x;

  val = lerp(read_imagef(array, sampler_nearest, (int2)(g.x, g.y)).x,
             val,
             read_imagef(mask, sampler_nearest, (int2)(g.x, g.y)).x);

  write_imagef(out, g, val);
}
)""
