/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file coord_frame.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2023
 */
#pragma once
#include <cmath>
#include <map>
#include <vector>

#include "highmap/algebra.hpp"
#include "highmap/heightmap.hpp"

namespace hmap
{

class CoordFrame
{
public:
  CoordFrame();

  CoordFrame(Vec2<float> origin, Vec2<float> size, float rotation_angle);

  // Getters
  Vec2<float> get_origin() const
  {
    return this->origin;
  }

  Vec2<float> get_size() const
  {
    return this->size;
  }

  float get_rotation_angle() const;

  // Setters
  void set_origin(Vec2<float> new_origin)
  {
    this->origin = new_origin;
  }

  void set_size(Vec2<float> new_size)
  {
    this->size = new_size;
  }

  void set_rotation_angle(float new_angle);

  // Methods

  Vec4<float> compute_bounding_box() const;

  float get_heightmap_value_bilinear(const Heightmap &h,
                                     float            gx,
                                     float            gy,
                                     float            fill_value = 0.f) const;

  float get_heightmap_value_nearest(const Heightmap &h,
                                    float            gx,
                                    float            gy,
                                    float            fill_value = 0.f) const;

  bool is_point_within(float gx, float gy) const;

  Vec2<float> map_to_global_coords(float rx, float ry) const;

  // relative coords always in [0, 1] x [0, 1] (unit square)
  Vec2<float> map_to_relative_coords(float gx, float gy) const;

  float normalized_distance_to_edges(float gx, float gy) const;

  float normalized_shape_factor(float gx, float gy) const;

private:
  Vec2<float> origin;
  Vec2<float> size;
  float       rotation_angle;

  float cos_angle;
  float sin_angle;
};

} // namespace hmap
