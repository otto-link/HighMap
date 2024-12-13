R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void insertion_sort(float *arr, const int n)
{
  for (int i = 1; i < n; i++)
  {
    float key = arr[i];
    int   j = i - 1;
    while ((j >= 0) && (key < arr[j]))
    {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

void kernel median_3x3(read_only image2d_t  img_in,
                       write_only image2d_t img_out,
                       int                  nx,
                       int                  ny)
{
  const int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                            CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

  float neighborhood[9];

  neighborhood[0] = read_imagef(img_in, sampler, (int2)(g.x - 1, g.y - 1)).x;
  neighborhood[1] = read_imagef(img_in, sampler, (int2)(g.x, g.y - 1)).x;
  neighborhood[2] = read_imagef(img_in, sampler, (int2)(g.x + 1, g.y - 1)).x;
  neighborhood[3] = read_imagef(img_in, sampler, (int2)(g.x - 1, g.y)).x;
  neighborhood[4] = read_imagef(img_in, sampler, (int2)(g.x, g.y)).x;
  neighborhood[5] = read_imagef(img_in, sampler, (int2)(g.x + 1, g.y)).x;
  neighborhood[6] = read_imagef(img_in, sampler, (int2)(g.x - 1, g.y + 1)).x;
  neighborhood[7] = read_imagef(img_in, sampler, (int2)(g.x, g.y + 1)).x;
  neighborhood[8] = read_imagef(img_in, sampler, (int2)(g.x + 1, g.y + 1)).x;
  insertion_sort(neighborhood, 9);

  const float val = neighborhood[4];

  write_imagef(img_out, (int2)(g.x, g.y), val);
}

void kernel avg(read_only image2d_t  img_in,
                write_only image2d_t img_out,
                int                  nx,
                int                  ny,
                int                  ir)
{
  const int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                            CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

  float val = 0.f;

  for (int p = -ir; p < ir + 1; p++)
    for (int q = -ir; q < ir + 1; q++)
      {
	float w = 1.f - hypot((float)p, (float)q) / (float)ir;
	val = max(val, w * read_imagef(img_in, sampler, (int2)(g.x + p, g.y + q)).x);
      }
  
  // val /= (float)(2.f * ir + 1.f) * (2.f * ir + 1.f);

  write_imagef(img_out, (int2)(g.x, g.y), val);
}
)""
