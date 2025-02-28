/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/math.hpp"

#include "highmap/internal/particles.hpp"

namespace hmap
{
Particle::Particle(float c_capacity,
                   float c_erosion,
                   float c_deposition,
                   float c_inertia,
                   float drag_rate)
    : c_capacity(c_capacity),
      c_erosion(c_erosion),
      c_deposition(c_deposition),
      c_inertia(c_inertia),
      drag_rate(drag_rate)
{
}

void Particle::move(const Array &h, float dt)
{
  float nx = -h.get_gradient_x_bilinear_at(this->pos.i,
                                           this->pos.j,
                                           this->pos.u,
                                           this->pos.v);
  float ny = -h.get_gradient_y_bilinear_at(this->pos.i,
                                           this->pos.j,
                                           this->pos.u,
                                           this->pos.v);

  // kill the particle on very flat surfaces
  float grad_norm = approx_hypot(nx, ny);
  if (grad_norm < HMAP_PARTICLES_GRADIENT_MIN)
  {
    this->is_active = false;
    return;
  }

  // classical mechanics (with gravity = 1)
  this->vel.x += dt * nx / c_inertia;
  this->vel.y += dt * ny / c_inertia;

  // drag rate
  float coeff = (1.f - dt * drag_rate);
  this->vel.x *= coeff;
  this->vel.y *= coeff;

  // kill the particle if it's too slow
  this->vnorm = approx_hypot(this->vel.x, this->vel.y);
  if (vnorm < HMAP_PARTICLES_VELOCITY_MIN)
  {
    this->is_active = false;
    return;
  }

  // clip velocity to ensure stability
  if (vnorm > vlim / dt)
  {
    this->vel = this->vel * (vlim / dt / vnorm);
  }

  // eventually move particle
  float new_x = this->pos.x + dt * this->vel.x;
  float new_y = this->pos.y + dt * this->vel.y;
  this->set_xy(new_x, new_y);
}

void Particle::set_xy(float x, float y)
{
  this->pos.x = x;
  this->pos.y = y;
  this->pos.i = (int)this->pos.x;
  this->pos.j = (int)this->pos.y;
  this->pos.u = this->pos.x - (float)this->pos.i;
  this->pos.v = this->pos.y - (float)this->pos.j;
}

} // namespace hmap
