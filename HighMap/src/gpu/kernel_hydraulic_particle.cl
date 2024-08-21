R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#define VELOCITY_MIN 0.005f

bool is_outside(int i, int j, int2 shape)
{
  return i < 1 || i > shape.x - 2 || j < 1 || j > shape.y - 2;
}

void kernel hydraulic_particle(read_write image2d_t img,
                               const uint           seed,
                               const float          c_capacity,
                               const float          c_erosion,
                               const float          c_deposition,
                               const float          drag_rate,
                               const float          evap_rate,
                               const float          dt)
{
  int  id = get_global_id(0); // particle id
  int2 shape = {get_image_height(img), get_image_width(img)};

  uint rng_state = wang_hash(seed + id);

  for (int it = 0; it < 1; it++)
  {

    float2 pos = {(shape.x - 2) * rand(&rng_state),
                  (shape.y - 2) * rand(&rng_state)};

    float2 vel = {0.f, 0.f};
    float  volume = 1.f;
    float  s = 0.f;

    float evap_factor = 1.f - evap_rate;

    int   i, j;
    float u, v;

    while (volume > 1e-3f)
    {
      update_interp_param(pos, shape, &i, &j, &u, &v);

      // stop if the particle reaches the domain limits
      if (is_outside(i, j, shape)) break;

      /* float dzx = -image_gradient_x_bilinear_interp(img, i, j, u, v); */
      /* float dzy = -image_gradient_y_bilinear_interp(img, i, j, u, v); */

      /* float dzx = -image_gradient_x(img, i, j); */
      /* float dzy = -image_gradient_y(img, i, j); */

      float dzx = -image_gradient_x_sobel(img, i, j);
      float dzy = -image_gradient_y_sobel(img, i, j);

      /* float dzx = -image_gradient_x_scharr(img, i, j); */
      /* float dzy = -image_gradient_y_scharr(img, i, j); */

      /* float dzx = -image_gradient_x_prewitt(img, i, j); */
      /* float dzy = -image_gradient_y_prewitt(img, i, j); */

      vel.x += dt * dzx;
      vel.y += dt * dzy;

      vel.x *= (1.f - dt * drag_rate);
      vel.y *= (1.f - dt * drag_rate);

      float vnorm = hypot(vel.x, vel.y);

      if (vnorm < VELOCITY_MIN) break;

      float2 pos_prev = pos;
      int    i_prev = i;
      int    j_prev = j;
      float  u_prev = u;
      float  v_prev = v;
      float  z_prev = image_bilinear_interp(img, i, j, u, v);

      pos.x += dt * vel.x;
      pos.y += dt * vel.y;

      // elevation at new position
      update_interp_param(pos, shape, &i, &j, &u, &v);
      if (is_outside(i, j, shape)) break;

      float z = image_bilinear_interp(img, i, j, u, v);

      float dz = z_prev - z;
      float sc = c_capacity * volume * vnorm * dz;
      float delta_sc = dt * (sc - s);
      float amount = 0.f;

      if (delta_sc > 0.f)
        amount = c_erosion * delta_sc; // erosion
      else
        amount = c_deposition * delta_sc; // deposition

      s += amount;

      image_depose_amount_bilinear(img,
                                   i_prev,
                                   j_prev,
                                   u_prev,
                                   v_prev,
                                   -amount);

      volume *= evap_factor;
    }
  }
}
)""
