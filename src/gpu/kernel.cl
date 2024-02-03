R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void kernel maximum_local_weighted(global const float   *input,
                                   global float         *output,
                                   constant const float *weights,
                                   const int             size_x,
                                   const int             size_y,
                                   const int             nx,
                                   const int             ny)
{
  int2 p = {get_local_id(0), get_local_id(1)};
  int2 g = {get_global_id(0), get_global_id(1)};
  int  index = g.x * ny + g.y;

  // 'times 2' to take into account buffers of size 'size / 2' at
  // every border of the kernel
  const int2 size = {size_x, size_y};
  const int2 half_size = size / 2;
  const int2 ntiles = 2 * size / BLOCK_SIZE;

  // declare cache subarray, fill it and wait other threads
  __local float sub_in[BLOCK_SIZE][BLOCK_SIZE];

  float val = input[index];

  for (int tx = 0; tx < ntiles.x; tx++)
    for (int ty = 0; ty < ntiles.y; ty++)
    {

      // shift global index by tile size
      int2 sh = {tx * BLOCK_SIZE - half_size.x, ty * BLOCK_SIZE - half_size.y};
      int2 gs = {g.x + sh.x, g.y + sh.y};

      if (gs.x >= 0 && gs.x < nx && gs.y >= 0 && gs.y < ny)
      {
        int km = gs.x * ny + gs.y;
        sub_in[p.x][p.y] = input[km];
      }
      else
        sub_in[p.x][p.y] = FLT_MIN;

      barrier(CLK_LOCAL_MEM_FENCE);

      int r1 = max(0, p.x - half_size.x - sh.x);
      int r2 = min(BLOCK_SIZE, p.x + half_size.x - sh.x + 1);

      int s1 = max(0, p.y - half_size.y - sh.y);
      int s2 = min(BLOCK_SIZE, p.y + half_size.y - sh.y + 1);

      for (int r = r1; r < r2; r++)
        for (int s = s1; s < s2; s++)
        {
          // find corresponding kernel index
          int iw = r - p.x + sh.x + half_size.x;
          int jw = s - p.y + sh.y + half_size.y;
          int km = iw * size.y + jw;

          val = max(val, sub_in[r][s] * weights[km]);
        }

      barrier(CLK_LOCAL_MEM_FENCE);
    }

  output[index] = val;
}
)""
