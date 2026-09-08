R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
float helper_thermal_exchange(float self, float other, float dist, float talus)
{
  float max_dif = dist * talus;
  float rate = 0.2f;

  if (self > other)
  {
    if (self - other > max_dif)
      return -rate * ((self - other) - max_dif) / dist;
    else
      return 0.f;
  }
  else
  {
    if (other - self > max_dif)
      return rate * ((other - self) - max_dif) / dist;
    else
      return 0.f;
  }
}

void kernel thermal(global float       *z,
                    global const float *talus,
                    const int           nx,
                    const int           ny,
                    const int           it)
{
  // https://www.shadertoy.com/view/XtKSWh
  int2 g = {get_global_id(0), get_global_id(1)};
  int  index = linear_index(g.x, g.y, nx);

  if (g.x >= nx || g.y >= ny) return;
  if (apply_boundaries(z, g.x, g.y, nx, ny)) return;

  // --- thermal erosion

  const float talus_val = talus[index];
  const int   di[8] = {-1, 0, 0, 1, -1, -1, 1, 1};
  const int   dj[8] = {0, 1, -1, 0, -1, 1, -1, 1};
  const float c[8] = {1.f, 1.f, 1.f, 1.f, 1.414f, 1.414f, 1.414f, 1.414f};

  float dz[8];
  float amount = 0.f;
  float val = z[index];

  for (int k = 0; k < 8; k++)
    amount += helper_thermal_exchange(
        val,
        z[linear_index(g.x + di[k], g.y + dj[k], nx)],
        c[k],
        talus_val);

  z[index] += amount;
}

void kernel thermal_with_bedrock(global float       *z,
                                 global const float *talus,
                                 global const float *bedrock,
                                 const int           nx,
                                 const int           ny,
                                 const int           it)
{
  // https://www.shadertoy.com/view/XtKSWh
  int2 g = {get_global_id(0), get_global_id(1)};
  int  index = linear_index(g.x, g.y, nx);

  if (g.x >= nx || g.y >= ny) return;
  if (apply_boundaries(z, g.x, g.y, nx, ny)) return;

  // --- thermal erosion

  float val = z[index];
  float z_bedrock = bedrock[index];

  if (val >= z_bedrock)
  {
    const float talus_val = talus[index];
    const int   di[8] = {-1, 0, 0, 1, -1, -1, 1, 1};
    const int   dj[8] = {0, 1, -1, 0, -1, 1, -1, 1};
    const float c[8] = {1.f, 1.f, 1.f, 1.f, 1.414f, 1.414f, 1.414f, 1.414f};

    float dz[8];
    float amount = 0.f;

    for (int k = 0; k < 8; k++)
      amount += helper_thermal_exchange(
          val,
          z[linear_index(g.x + di[k], g.y + dj[k], nx)],
          c[k],
          talus_val);

    z[index] += amount;
  }

  z[index] = max(z[index], z_bedrock);
}

void kernel thermal_auto_bedrock(global float       *z,
                                 global const float *talus,
                                 global float       *bedrock,
                                 global const float *z0,
                                 const int           nx,
                                 const int           ny,
                                 const int           it)
{
  int2 g = {get_global_id(0), get_global_id(1)};
  int  index = linear_index(g.x, g.y, nx);

  if (g.x >= nx || g.y >= ny) return;
  if (apply_boundaries(z, g.x, g.y, nx, ny)) return;

  // --- thermal erosion

  if (it == 0) bedrock[index] = -FLT_MAX;

  float val = z[index];
  float z_bedrock = bedrock[index];

  if (val >= z_bedrock)
  {
    float z_init = z0[index];

    const float talus_val = talus[index];
    const int   di[8] = {-1, 0, 0, 1, -1, -1, 1, 1};
    const int   dj[8] = {0, 1, -1, 0, -1, 1, -1, 1};
    const float c[8] = {1.f, 1.f, 1.f, 1.f, 1.414f, 1.414f, 1.414f, 1.414f};

    float dz[8];
    float amount = 0.f;

    for (int k = 0; k < 8; k++)
      amount += helper_thermal_exchange(
          val,
          z[linear_index(g.x + di[k], g.y + dj[k], nx)],
          c[k],
          talus_val);

    z[index] = max(max(val + amount, z_init), z_bedrock);
    bedrock[index] = max(z_init, z[index]);
  }
}

// --- Mass-preserving thermal erosion (double-buffered, pull pattern)

void kernel thermal_conserve(read_only image2d_t  z_in,
                             write_only image2d_t z_out,
                             read_only image2d_t  talus,
                             const int            nx,
                             const int            ny,
                             const float          rate)
{
  const int2 g = {get_global_id(0), get_global_id(1)};

  if (g.x >= nx || g.y >= ny) return;

  const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                            CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

  float z_c = TGET(z_in, g.x, g.y);

  // boundary cells: strictly inactive, unchanged
  if (g.x <= 0 || g.x >= nx - 1 || g.y <= 0 || g.y >= ny - 1)
  {
    TSET(z_out, g.x, g.y, z_c);
    return;
  }

  const int   di[8] = {-1, 0, 0, 1, -1, -1, 1, 1};
  const int   dj[8] = {0, 1, -1, 0, -1, 1, -1, 1};
  const float dist[8] = {1.f,
                         1.f,
                         1.f,
                         1.f,
                         1.414213562f,
                         1.414213562f,
                         1.414213562f,
                         1.414213562f};
  // reverse direction index: from neighbor k back to center
  const int rev[8] = {3, 2, 1, 0, 7, 6, 5, 4};

  float talus_c = TGET(talus, g.x, g.y);

  // --- Compute outflow from this active interior cell to other active cells

  float excess[8];
  float excess_sum = 0.f;
  float excess_max = 0.f;

  for (int k = 0; k < 8; ++k)
  {
    int ni = g.x + di[k];
    int nj = g.y + dj[k];

    // only exchange with other active interior cells (closed boundary
    // condition)
    if (ni <= 0 || ni >= nx - 1 || nj <= 0 || nj >= ny - 1)
    {
      excess[k] = 0.f;
      continue;
    }

    float z_n = TGET(z_in, ni, nj);
    float e = z_c - z_n - dist[k] * talus_c;

    if (e > 0.f)
    {
      excess[k] = e;
      excess_sum += e;
      excess_max = max(excess_max, e);
    }
    else
    {
      excess[k] = 0.f;
    }
  }

  float outflow = 0.f;
  if (excess_sum > 0.f) outflow = rate * 0.5f * excess_max;

  // --- Compute inflow: for each active neighbor, recompute its outflow and
  //     how much of it is directed toward this cell

  float inflow = 0.f;

  for (int k = 0; k < 8; ++k)
  {
    int ni = g.x + di[k];
    int nj = g.y + dj[k];

    // skip boundary neighbors (they are inactive)
    if (ni <= 0 || ni >= nx - 1 || nj <= 0 || nj >= ny - 1) continue;

    float z_n = TGET(z_in, ni, nj);
    float talus_n = TGET(talus, ni, nj);

    // recompute neighbor's excess toward all ITS active neighbors
    float n_excess_sum = 0.f;
    float n_excess_max = 0.f;
    float n_excess_to_c = 0.f; // excess in the direction toward center

    for (int p = 0; p < 8; ++p)
    {
      int qi = ni + di[p];
      int qj = nj + dj[p];

      // only consider active neighbors
      if (qi <= 0 || qi >= nx - 1 || qj <= 0 || qj >= ny - 1) continue;

      float z_q = TGET(z_in, qi, qj);
      float e = z_n - z_q - dist[p] * talus_n;

      if (e > 0.f)
      {
        n_excess_sum += e;
        n_excess_max = max(n_excess_max, e);

        // direction from neighbor back to center is rev[k]
        if (p == rev[k]) n_excess_to_c = e;
      }
    }

    if (n_excess_to_c > 0.f && n_excess_sum > 0.f)
    {
      float n_outflow = rate * 0.5f * n_excess_max;
      inflow += n_outflow * n_excess_to_c / n_excess_sum;
    }
  }

  TSET(z_out, g.x, g.y, z_c - outflow + inflow);
}
)""
