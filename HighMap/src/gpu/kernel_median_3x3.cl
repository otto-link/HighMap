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

void kernel median_3x3(global const float *input,
                       global float       *output,
                       const int           nx,
                       const int           ny)
{
  int2 g = {get_global_id(0), get_global_id(1)};
  int  index = linear_index(g.x, g.y, ny);

  float neighborhood[9];

  int im1 = max(g.x - 1, 0);
  int ip1 = min(g.x + 1, nx - 1);
  int jm1 = max(g.y - 1, 0);
  int jp1 = min(g.y + 1, ny - 1);

  neighborhood[0] = input[linear_index(im1, jm1, ny)];
  neighborhood[1] = input[linear_index(im1, g.y, ny)];
  neighborhood[2] = input[linear_index(im1, jp1, ny)];
  neighborhood[3] = input[linear_index(g.x, jm1, ny)];
  neighborhood[4] = input[linear_index(g.x, g.y, ny)];
  neighborhood[5] = input[linear_index(g.x, jp1, ny)];
  neighborhood[6] = input[linear_index(ip1, jm1, ny)];
  neighborhood[7] = input[linear_index(ip1, g.y, ny)];
  neighborhood[8] = input[linear_index(ip1, jp1, ny)];

  insertion_sort(neighborhood, 9);

  output[index] = neighborhood[4];
}

void kernel median_3x3_img(read_only image2d_t  img_in,
                           write_only image2d_t img_out)
{
  const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                            CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
  const int2 g = {get_global_id(0), get_global_id(1)};

  float neighborhood[9];

  neighborhood[0] = read_imagef(img_in, sampler, (int2)(g.y - 1, g.x - 1)).x;
  neighborhood[1] = read_imagef(img_in, sampler, (int2)(g.y, g.x - 1)).x;
  neighborhood[2] = read_imagef(img_in, sampler, (int2)(g.y + 1, g.x - 1)).x;
  neighborhood[3] = read_imagef(img_in, sampler, (int2)(g.y - 1, g.x)).x;
  neighborhood[4] = read_imagef(img_in, sampler, (int2)(g.y, g.x)).x;
  neighborhood[5] = read_imagef(img_in, sampler, (int2)(g.y + 1, g.x)).x;
  neighborhood[6] = read_imagef(img_in, sampler, (int2)(g.y - 1, g.x + 1)).x;
  neighborhood[7] = read_imagef(img_in, sampler, (int2)(g.y, g.x + 1)).x;
  neighborhood[8] = read_imagef(img_in, sampler, (int2)(g.y + 1, g.x + 1)).x;
  insertion_sort(neighborhood, 9);

  const float val = neighborhood[4];

  write_imagef(img_out, (int2)(g.y, g.x), val);
}
)""
