R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void kernel thermal(read_only image2d_t  z,
                    read_only image2d_t  talus,
                    write_only image2d_t out,
                    int                  nx,
                    int                  ny)
{
  const int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                            CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

  const float z_val = read_imagef(z, sampler, (int2)(g.x, g.y)).x;
  const float talus_val = read_imagef(talus, sampler, (int2)(g.x, g.y)).x;

  write_imagef(out, (int2)(g.x, g.y), z_val);

  if (g.x == 0 || g.x == nx - 1 || g.y == 0 || g.y == ny - 1) return;

  int   di[8] = {-1, 0, 0, 1, -1, -1, 1, 1};
  int   dj[8] = {0, 1, -1, 0, -1, 1, -1, 1};
  float c[8] = {1.f, 1.f, 1.f, 1.f, 1.414f, 1.414f, 1.414f, 1.414f};

  /* shuffle_array_i(di, 8, 0); */
  /* shuffle_array_i(dj, 8, 0); */
  /* shuffle_array_f(c, 8, 0); */

  float neighborhood[8];
  for (int k = 0; k < 8; k++)
    neighborhood[k] = read_imagef(z, sampler, (int2)(g.x + di[k], g.y + dj[k]))
                          .x;

  float dz[8];
  float dmax = 0.f;
  float dsum = 0.f;

  for (int k = 0; k < 8; k++)
  {
    dz[k] = z_val - neighborhood[k];
    if (dz[k] > talus_val * c[k])
    {
      dsum += dz[k];
      dmax = max(dmax, dz[k]);
    }
  }

  if (dmax > 0.f)
    for (int k = 0; k < 8; k++)
    {
      float amount = 0.5f * (dmax - talus_val * c[k]) * dz[k] / dsum;
      write_imagef(out,
                   (int2)(g.x + di[k], g.y + dj[k]),
                   neighborhood[k] + amount);

      /* write_imagef(out, */
      /*              (int2)(g.x + di[k], g.y + dj[k]), */
      /*              talus_val); */
    }
}

void kernel thermal_bf(global float       *z,
                       global const float *talus,
                       const int           nx,
                       const int           ny,
                       const int           it)
{
  int2 g = {get_global_id(0), get_global_id(1)};

  if (it % 4 == 0)
    g.x = nx - 1 - g.x;
  else if (it % 4 == 1)
    g.y = ny - 1 - g.y;
  else if (it % 4 == 2)
  {
    g.x = nx - 1 - g.x;
    g.y = ny - 1 - g.y;
  }

  int step = 4;

  uint rng = wang_hash((uint)it);
  int  d1 = (int)(get_local_size(0) * rand(&rng));
  int  d2 = (int)(get_local_size(1) * rand(&rng));

  int2 stride = {floor((float)(step * g.x) / nx + d1),
                 floor((float)(step * g.y) / ny + d2)};

  g.x = (step * g.x + stride.x) % nx;
  g.y = (step * g.y + stride.y) % ny;

  if (g.x >= nx || g.y >= ny) return;

  // --- boundaries

  int index = linear_index(g.x, g.y, nx);

  if (g.x == 0)
  {
    z[index] = z[linear_index(1, g.y, nx)];
    return;
  }
  if (g.x == nx - 1)
  {
    z[index] = z[linear_index(nx - 2, g.y, nx)];
    return;
  }
  if (g.y == 0)
  {
    z[index] = z[linear_index(g.x, 1, nx)];
    return;
  }
  if (g.y == ny - 1)
  {
    z[index] = z[linear_index(g.x, ny - 2, nx)];
    return;
  }

  // --- thermal erosion

  float talus_val = talus[index];

  int   di[8] = {-1, 0, 0, 1, -1, -1, 1, 1};
  int   dj[8] = {0, 1, -1, 0, -1, 1, -1, 1};
  float c[8] = {1.f, 1.f, 1.f, 1.f, 1.414f, 1.414f, 1.414f, 1.414f};

  float dmax = 0.f;
  float dsum = 0.f;

  for (int k = 0; k < 8; k++)
  {
    float dz = z[index] - z[linear_index(g.x + di[k], g.y + dj[k], nx)];
    if (dz > talus_val * c[k])
    {
      dsum += dz;
      dmax = max(dmax, dz);
    }
  }

  barrier(CLK_LOCAL_MEM_FENCE);

  if (dmax > 0.f)
    for (int k = 0; k < 8; k++)
    {
      float dz = z[index] - z[linear_index(g.x + di[k], g.y + dj[k], nx)];
      float amount = 0.5f * (dmax - talus_val * c[k]) * dz / dsum;
      // amount = min(amount, 0.5f * talus_val);
      z[linear_index(g.x + di[k], g.y + dj[k], nx)] += amount;
    }
}
)""
