/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file primitives.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "highmap/algebra.hpp"
#include "highmap/array.hpp"

#define HMAP_PARTICLES_GRADIENT_MIN 0.0001f
#define HMAP_PARTICLES_VELOCITY_MIN 0.001f

namespace hmap
{

struct Pos
{
  float x;
  float y;
  int   i;
  int   j;
  float u;
  float v;
};

struct Particle
{
  // parameters
  float c_capacity;
  float c_erosion;
  float c_deposition;
  float c_inertia;
  float drag_rate;

  // features
  float sediment = 0.f;
  float volume = 0.f; // water

  // dynamic
  Pos         pos;
  Vec2<float> vel;
  float       vnorm;
  bool        vlim = 1.f;
  bool        is_active = true;

  // constructor
  Particle(float c_capacity,
           float c_erosion,
           float c_deposition,
           float c_inertia,
           float drag_rate);

  // methods
  void move(const Array &h, float dt);

  void set_xy(float x, float y);
};

} // namespace hmap