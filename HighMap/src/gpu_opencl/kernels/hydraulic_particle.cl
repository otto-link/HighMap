R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#define VELOCITY_MIN 0.005f
#define VOLUME_MIN 0.001f

void kernel hydraulic_particle2(read_only image2d_t  z_in,
                                write_only image2d_t z_out,
                                global float        *mask,
                                global float        *bedrock,
                                global float        *moisture_map,
                                const int            nx,
                                const int            ny,
                                const int            nparticles,
                                const uint           seed,
                                const float          c_capacity,
                                const float          c_erosion,
                                const float          c_deposition,
                                const float          drag_rate,
                                const float          evap_rate,
                                const int            has_mask,
                                const int            has_bedrock,
                                const int            has_moisture_map)
{
  float dt = 1.f;

  int id = get_global_id(0); // particle id
  if (id > nparticles) return;

  uint rng_state = wang_hash(seed + id);

  const sampler_t sampler_itp = CLK_NORMALIZED_COORDS_FALSE |
                                CLK_ADDRESS_MIRRORED_REPEAT | CLK_FILTER_LINEAR;

  const sampler_t sampler_ij = CLK_NORMALIZED_COORDS_FALSE |
                               CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

  /* float mask_value = has_mask > 0 ? mask[index] : 1.f; */
  /* float bedrock_value = has_bedrock > 0 ? bedrock[index] : -FLT_MAX; */
  /* float moisture_value = has_moisture > 0 ? moisture_map[index] : 1.f; */

  for (int it = 0; it < 1; it++)
  {

    float2 pos = {(nx - 2) * rand(&rng_state), (ny - 2) * rand(&rng_state)};

    float2 vel = {0.f, 0.f};
    float  volume = 1.f;
    float  s = 0.f;

    float evap_factor = 1.f - evap_rate;

    int   i, j;
    float u, v;

    int count = 0;

    while (volume > 1e-3f && count < 10000)
    {
      count++;

      update_interp_param(pos, &i, &j, &u, &v);

      // stop if the particle reaches the domain limits
      if (i < 1 || i > nx - 2 || j < 1 || j > ny - 2) break;

      /* // retrieve neighborhood elevation values */
      /* float f_0_0 = z_in[linear_index(i, j, nx)]; */
      /* float f_p1_0 = z_in[linear_index(i + 1, j, nx)]; */
      /* float f_m1_0 = z_in[linear_index(i - 1, j, nx)]; */
      /* float f_0_p1 = z_in[linear_index(i, j + 1, nx)]; */
      /* float f_0_m1 = z_in[linear_index(i, j - 1, nx)]; */
      /* float f_p1_p1 = z_in[linear_index(i + 1, j + 1, nx)]; */
      /* float f_m1_p1 = z_in[linear_index(i - 1, j + 1, nx)]; */
      /* float f_p1_m1 = z_in[linear_index(i + 1, j - 1, nx)]; */
      /* float f_m1_m1 = z_in[linear_index(i - 1, j - 1, nx)]; */

      // local gradient (Sobel)
      float dzx = 0.5f * (read_imagef(z_in, sampler_ij, (int2)(i + 1, j)).x -
                          read_imagef(z_in, sampler_ij, (int2)(i - 1, j)).x);
      float dzy = 0.5f * (read_imagef(z_in, sampler_ij, (int2)(i, j + 1)).x -
                          read_imagef(z_in, sampler_ij, (int2)(i, j - 1)).x);

      /* float dx = 0.25f * read_imagef(z_in, sampler_ij, (int2)(i + 1, j)).x -
       */
      /*            0.25f * read_imagef(z_in, sampler_ij, (int2)(i - 1, j)).x;
       */
      /* dx += 0.125f * read_imagef(z_in, sampler_ij, (int2)(i + 1, j - 1)).x -
       */
      /*       0.125f * read_imagef(z_in, sampler_ij, (int2)(i - 1, j - 1)).x;
       */
      /* dx += 0.125f * read_imagef(z_in, sampler_ij, (int2)(i + 1, j + 1)).x -
       */
      /*       0.125f * read_imagef(z_in, sampler_ij, (int2)(i - 1, j + 1)).x;
       */

      /* float dy = 0.25f * read_imagef(z_in, sampler_ij, (int2)(i, j + 1)).x -
       */
      /*            0.25f * read_imagef(z_in, sampler_ij, (int2)(i, j - 1)).x;
       */
      /* dy += 0.125f * read_imagef(z_in, sampler_ij, (int2)(i - 1, j + 1)).x -
       */
      /*       0.125f * read_imagef(z_in, sampler_ij, (int2)(i - 1, j - 1)).x;
       */
      /* dy += 0.125f * read_imagef(z_in, sampler_ij, (int2)(i + 1, j + 1)).x -
       */
      /*       0.125f * read_imagef(z_in, sampler_ij, (int2)(i + 1, j - 1)).x;
       */

      // particle goes downhill, opposite local gradient
      vel += dt * (float2)(-dzx, -dzy);
      // vel *= (1.f - dt * drag_rate);

      float vnorm = length(vel);

      if (vnorm < VELOCITY_MIN) break;

      float2 shift = {0.5f, 0.5f};
      float  z_prev = read_imagef(z_in, sampler_itp, pos + shift).x;

      /* float2 pos_n = (float2)(pos.x /(nx - 1.f), pos.y / (ny - 1.f)); */
      /* float z_prev = read_imagef(z_in, sampler_itp, pos_n).x; */

      int   i_prev = i;
      int   j_prev = j;
      float u_prev = u;
      float v_prev = v;

      // float  z_prev = read_imagef(z_in, sampler_ij, (int2)(i, j)).x;

      pos += dt * vel;

      // elevation at new position
      update_interp_param(pos, &i, &j, &u, &v);
      if (i < 1 || i > nx - 2 || j < 1 || j > ny - 2) break;

      float z = read_imagef(z_in, sampler_itp, pos + shift).x;

      /* pos_n = (float2)(pos.x /(nx - 1.f), pos.y / (ny - 1.f)); */
      /* float z = read_imagef(z_in, sampler_itp, pos_n).x; */

      // z_in[linear_index(i, j, nx)]; // image_bilinear_interp(z_in, i, j, u,
      // v);

      float dz = z_prev - z;
      float sc = c_capacity * volume * vnorm * dz;
      float delta_sc = dt * (sc - s);
      float amount = 0.f;

      if (delta_sc > 0.f)
        amount = c_erosion * delta_sc; // erosion
      else
        amount = c_deposition * delta_sc; // deposition

      s += amount;

      // z_in[linear_index(i_prev, j_prev, nx)] -= amount;

      // float val = count;
      // float val = c_erosion;
      // write_imagef(z_out, (int2)(i_prev, j_prev), val);

      /* { */
      /*   amount *= -1.f; */

      /*   int p = i_prev; */
      /*   int q = j_prev; */
      /* 	u = u_prev; */
      /* 	v = v_prev; */

      /*   float f00 = read_imagef(z_in, sampler_ij, (int2)(p, q)).x; */
      /*   float f10 = read_imagef(z_in, sampler_ij, (int2)(p + 1, q)).x; */
      /*   float f01 = read_imagef(z_in, sampler_ij, (int2)(p, q + 1)).x; */
      /*   float f11 = read_imagef(z_in, sampler_ij, (int2)(p + 1, q + 1)).x; */

      /*   write_imagef(z_out, (int2)(p, q), f00 + amount * (1.f - u) * (1.f -
       * v)); */
      /*   write_imagef(z_out, (int2)(p + 1, q + 1), f10 + amount * u * (1.f -
       * v)); */
      /*   write_imagef(z_out, (int2)(p, q + 1), f01 + amount * (1.f - u) * v);
       */
      /*   write_imagef(z_out, (int2)(p + 1, q + 1), f11 + amount * u * v); */
      /* } */

      write_imagef(z_out, (int2)(i_prev, j_prev), z_prev - amount);

      volume *= evap_factor;
    }
  }
}

float bilinear_interp(const float f00,
                      const float f10,
                      const float f01,
                      const float f11,
                      const float u,
                      const float v)
{
  float a10 = f10 - f00;
  float a01 = f01 - f00;
  float a11 = f11 - f10 - f01 + f00;

  return f00 + a10 * u + a01 * v + a11 * u * v;
}

void kernel hydraulic_particle(global float *z_in,
                               global float *bedrock,
                               global float *moisture_map,
                               const int     nx,
                               const int     ny,
                               const int     nparticles,
                               const uint    seed,
                               const float   c_capacity,
                               const float   c_erosion,
                               const float   c_deposition,
                               const float   drag_rate,
                               const float   evap_rate,
                               const int     has_bedrock,
                               const int     has_moisture_map)
{
  float dt = 3.f;

  int id = get_global_id(0); // particle id
  if (id > nparticles) return;

  uint rng_state = wang_hash(seed + id);

  float2 pos = {(nx - 2) * rand(&rng_state), (ny - 2) * rand(&rng_state)};
  int    i, j;
  float  u, v;

  update_interp_param(pos, &i, &j, &u, &v);

  float2 vel = {0.f, 0.f};
  float  volume = has_moisture_map > 0 ? moisture_map[linear_index(i, j, nx)]
                                       : 1.f;

  float s = 0.f;

  float evap_factor = 1.f - evap_rate;

  while (volume > 1e-3f)
  {
    update_interp_param(pos, &i, &j, &u, &v);

    // stop if the particle reaches the domain limits
    if (i < 1 || i > nx - 2 || j < 1 || j > ny - 2) break;

    // retrieve neighborhood elevation values
    float f_0_0 = z_in[linear_index(i, j, nx)];
    float f_p1_0 = z_in[linear_index(i + 1, j, nx)];
    float f_0_p1 = z_in[linear_index(i, j + 1, nx)];
    float f_p1_p1 = z_in[linear_index(i + 1, j + 1, nx)];
    float f_m1_0 = z_in[linear_index(i - 1, j, nx)];
    float f_0_m1 = z_in[linear_index(i, j - 1, nx)];

    // local gradient
    float dzx = 0.5f * (f_p1_0 - f_m1_0);
    float dzy = 0.5f * (f_0_p1 - f_0_m1);

    // particle goes downhill, opposite local gradient
    vel += dt * (float2)(-dzx, -dzy);
    vel *= (1.f - dt * drag_rate);

    float vnorm = length(vel);

    float vlim = 1.f / dt;

    if (vnorm > vlim)
    {
      vel /= vnorm;
      vel *= vlim;
    }

    if (vnorm < VELOCITY_MIN) break;

    float z_p = bilinear_interp(f_0_0, f_p1_0, f_0_p1, f_p1_p1, u, v);

    // backup previous position
    float2 pos_p = pos;
    int    i_p = i;
    int    j_p = j;
    float  u_p = u;
    float  v_p = v;

    // move particle
    pos += dt * vel;

    // elevation at new position
    update_interp_param(pos, &i, &j, &u, &v);
    if (i < 1 || i > nx - 2 || j < 1 || j > ny - 2) break;

    float z = bilinear_interp(z_in[linear_index(i, j, nx)],
                              z_in[linear_index(i + 1, j, nx)],
                              z_in[linear_index(i, j + 1, nx)],
                              z_in[linear_index(i + 1, j + 1, nx)],
                              u,
                              v);

    float dz = z_p - z;
    float sc = c_capacity * volume * vnorm * dz;
    float delta_sc = dt * (sc - s);
    float amount = 0.f;

    if (delta_sc > 0.f)
      amount = c_erosion * delta_sc; // erosion
    else
      amount = c_deposition * delta_sc; // deposition

    s += amount;

    if (has_bedrock == 0)
    {
      z_in[linear_index(i_p, j_p, nx)] -= amount * (1.f - u_p) * (1.f - v_p);
      z_in[linear_index(i_p + 1, j_p, nx)] -= amount * u_p * (1.f - v_p);
      z_in[linear_index(i_p, j_p + 1, nx)] -= amount * (1.f - u_p) * v_p;
      z_in[linear_index(i_p + 1, j_p + 1, nx)] -= amount * u_p * v_p;
    }
    else
    {
      z_in[linear_index(i_p, j_p, nx)] = max(
          bedrock[linear_index(i_p, j_p, nx)],
          z_in[linear_index(i_p, j_p, nx)] -
              amount * (1.f - u_p) * (1.f - v_p));

      z_in[linear_index(i_p + 1, j_p, nx)] = max(
          bedrock[linear_index(i_p + 1, j_p, nx)],
          z_in[linear_index(i_p + 1, j_p, nx)] - amount * u_p * (1.f - v_p));

      z_in[linear_index(i_p, j_p + 1, nx)] = max(
          bedrock[linear_index(i_p, j_p + 1, nx)],
          z_in[linear_index(i_p, j_p + 1, nx)] - amount * (1.f - u_p) * v_p);

      z_in[linear_index(i_p + 1, j_p + 1, nx)] = max(
          bedrock[linear_index(i_p + 1, j_p + 1, nx)],
          z_in[linear_index(i_p + 1, j_p + 1, nx)] - amount * u_p * v_p);
    }

    /* if (amount < 0.f) // depose */
    /* { */

    /* } */
    /* else */
    /* { */
    /*   z_in[linear_index(i_p, j_p, nx)] -= amount * (1.f - u_p) *
     */
    /*                                             (1.f - v_p); */
    /*   z_in[linear_index(i_p + 1, j_p, nx)] -= amount * u_p * */
    /*                                                 (1.f - v_p); */
    /*   z_in[linear_index(i_p, j_p + 1, nx)] -= amount * (1.f - u_p)
     * * */
    /*                                                 v_p; */
    /*   z_in[linear_index(i_p + 1, j_p + 1, nx)] -= amount * u_p *
     */
    /*                                                     v_p; */

    /*   amount *= -1.f; */
    /*   float sum = 0.f; */

    /*   if (false) */
    /*   { */
    /*     float w[3][3]; // weights */
    /*     float rmax = 2.f; */

    /*     for (int p = -1; p < 2; p++) */
    /*       for (int q = -1; q < 2; q++) */
    /*       { */
    /*         float2 pos_cell = (float2)(i + p, j + q); */
    /*         w[p + 1][q + 1] = max(0.f, */
    /*                               1.f - length(pos_p - pos_cell) /
     * rmax); */
    /*         sum += w[p + 1][q + 1]; */
    /*       } */

    /*     for (int p = -1; p < 2; p++) */
    /*       for (int q = -1; q < 2; q++) */
    /*         z_in[linear_index(i_p + p, j_p + q, nx)] += */
    /*             amount * w[p + 1][q + 1] / sum; */
    /*   } */

    /*   if (false) */
    /*   { */
    /*     float w[5][5]; // weights */
    /*     float rmax = 3.f; */

    /*     for (int p = -2; p < 3; p++) */
    /*       for (int q = -2; q < 3; q++) */
    /*       { */
    /*         float2 pos_cell = (float2)(i + p, j + q); */
    /*         w[p + 2][q + 2] = max(0.f, */
    /*                               1.f - length(pos_p - pos_cell) /
     * rmax); */
    /*         sum += w[p + 2][q + 2]; */
    /*       } */

    /*     for (int p = -2; p < 3; p++) */
    /*       for (int q = -2; q < 3; q++) */
    /*         z_in[linear_index(i_p + p, j_p + q, nx)] += */
    /*             amount * w[p + 2][q + 2] / sum; */
    /*   } */

    /*   /\* z_in[linear_index(i_p, j_p, nx)] += amount * (1.f - u_p)
     * * */
    /*    *\/ */
    /*   /\*                                           (1.f - v_p); *\/ */
    /*   /\* z_in[linear_index(i_p + 1, j_p, nx)] += amount * u_p *
     * *\/ */
    /*   /\*                                               (1.f - v_p); *\/
     */
    /*   /\* z_in[linear_index(i_p, j_p + 1, nx)] += amount * (1.f -
     * u_p) */
    /*    *\/ */
    /*   /\* * *\/ */
    /*   /\*                                               v_p; *\/ */
    /*   /\* z_in[linear_index(i_p + 1, j_p + 1, nx)] += amount * u_p
     * * */
    /*    *\/ */
    /*   /\*                                                   v_p; *\/ */
    /* } */

    volume *= evap_factor;
  }
}
)""
