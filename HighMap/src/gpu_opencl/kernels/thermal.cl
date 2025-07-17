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

  z[index] = max(val, z_bedrock);
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
)""
