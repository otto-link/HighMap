R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void kernel flow_accumulation_d8(global const float *z,
                                 global const float *d8,
                                 global int         *facc,
                                 const int           nx,
                                 const int           ny)
{
  // TODO not functional

  int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  const int di[8] = {1, 1, 0, -1, -1, -1, 0, 1};
  const int dj[8] = {0, -1, -1, -1, 0, 1, 1, 1};

  for (int k = 0; k < 8; k++)
  {
    int p = g.x + di[k];
    int q = g.y + dj[k];

    int index_ij = linear_index(g.x, g.y, nx);
    int index_pq = linear_index(p, q, nx);

    if (is_inside(p, q, nx, ny))
      if (d8[index_pq] == (k + 4) % 8)
      {
        int facc_pq = facc[index_pq];
        if (facc_pq > 0)
        {
          atomic_add(&facc[index_ij], 1);
          // atomic_add(&facc[index_pq], -1);
        }
      }
  }
}

void kernel flow_direction_d8(global const float *z,
                              global float       *d8,
                              const int           nx,
                              const int           ny)
{
  int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  const int   di[8] = {1, 1, 0, -1, -1, -1, 0, 1};
  const int   dj[8] = {0, -1, -1, -1, 0, 1, 1, 1};
  const float c[8] = {1.f, 0.707f, 1.f, 0.707f, 1.f, 0.707f, 1.f, 0.707f};

  float dmax = 0.f;
  int   kn = 0;

  for (int k = 0; k < 8; k++)
  {
    int p = g.x + di[k];
    int q = g.y + dj[k];

    if (is_inside(p, q, nx, ny))
    {
      float delta = z[linear_index(g.x, g.y, nx)] - z[linear_index(p, q, nx)];
      delta *= c[k];

      if (delta > dmax)
      {
        dmax = delta;
        kn = k;
      }
    }
  }

  d8[linear_index(g.x, g.y, nx)] = kn;
}
)""
