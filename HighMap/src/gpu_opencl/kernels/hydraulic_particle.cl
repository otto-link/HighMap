R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#define VELOCITY_MIN 0.005f
#define VOLUME_MIN 0.001f

float helper_bilinear_interp(const float f00,
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
                               const float   c_inertia,
                               const float   drag_rate,
                               const float   evap_rate,
                               const int     has_bedrock,
                               const int     has_moisture_map)
{
  float dt = 1.f;

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
    vel += dt * (float2)(-dzx, -dzy) / c_inertia;
    vel *= (1.f - dt * drag_rate);

    float vnorm = length(vel);

    if (vnorm < VELOCITY_MIN) break;

    float z_p = helper_bilinear_interp(f_0_0, f_p1_0, f_0_p1, f_p1_p1, u, v);

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

    float z = helper_bilinear_interp(z_in[linear_index(i, j, nx)],
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

    if (amount > 0.f) // erosion
    {
      float d1 = length((float2)(1.f - u_p, 1.f - v_p));
      float d2 = length((float2)(u_p, 1.f - v_p));
      float d3 = length((float2)(1.f - u_p, v_p));
      float d4 = length((float2)(u_p, v_p));

      float dsum = d1 + d2 + d3 + d4;

      z_in[linear_index(i_p, j_p, nx)] -= amount * d1 / dsum;
      z_in[linear_index(i_p + 1, j_p, nx)] -= amount * d2 / dsum;
      z_in[linear_index(i_p, j_p + 1, nx)] -= amount * d3 / dsum;
      z_in[linear_index(i_p + 1, j_p + 1, nx)] -= amount * d4 / dsum;

      if (has_bedrock != 0)
      {
        z_in[linear_index(i_p, j_p, nx)] = max(
            bedrock[linear_index(i_p, j_p, nx)],
            z_in[linear_index(i_p, j_p, nx)]);

        z_in[linear_index(i_p + 1, j_p, nx)] = max(
            bedrock[linear_index(i_p + 1, j_p, nx)],
            z_in[linear_index(i_p + 1, j_p, nx)]);

        z_in[linear_index(i_p, j_p + 1, nx)] = max(
            bedrock[linear_index(i_p, j_p + 1, nx)],
            z_in[linear_index(i_p, j_p + 1, nx)]);

        z_in[linear_index(i_p + 1, j_p + 1, nx)] = max(
            bedrock[linear_index(i_p + 1, j_p + 1, nx)],
            z_in[linear_index(i_p + 1, j_p + 1, nx)]);
      }
    }
    else if (amount < 0.f) // deposition
    {
      z_in[linear_index(i_p, j_p, nx)] -= amount;

      // local laplace filter
      float delta = -4.f * z_in[linear_index(i_p, j_p, nx)] +
                    z_in[linear_index(i_p + 1, j_p, nx)] +
                    z_in[linear_index(i_p - 1, j_p, nx)] +
                    z_in[linear_index(i_p, j_p - 1, nx)] +
                    z_in[linear_index(i_p, j_p + 1, nx)];

      z_in[linear_index(i_p, j_p, nx)] += 0.25f *
                                          min(10.f * c_deposition, 1.f) * delta;
    }

    volume *= evap_factor;
  }
}
)""
