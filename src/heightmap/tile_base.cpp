/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"

namespace hmap
{

Tile::Tile()
    : Array(), shift(0.f, 0.f), scale(0.f, 0.f), bbox(0.f, 0.f, 0.f, 0.f){};

Tile::Tile(Vec2<int>   shape,
           Vec2<float> shift,
           Vec2<float> scale,
           Vec4<float> bbox)
    : Array(shape), shift(shift), scale(scale), bbox(bbox)
{
}

void Tile::operator=(const Array &array)
{
  this->vector = array.vector;
}

void Tile::infos() const
{
  std::cout << "Tile, ";
  std::cout << "address: " << this << ", ";
  std::cout << "shape: {" << this->shape.x << ", " << this->shape.y << "}, ";
  std::cout << "shift: {" << this->shift.x << ", " << this->shift.y << "}, ";
  std::cout << "scale: {" << this->scale.x << ", " << this->scale.y << "}, ";
  std::cout << "bbox: {" << this->bbox.a << ", " << this->bbox.b << ", "
            << this->bbox.c << ", " << this->bbox.d << "}, ";
  std::cout << std::endl;
}

} // namespace hmap
