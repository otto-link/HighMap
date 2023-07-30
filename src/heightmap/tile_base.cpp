/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"

namespace hmap
{

Tile::Tile()
    : Array({0, 0}), shift({0.f, 0.f}), scale({0.f, 0.f}),
      bbox({0.f, 0.f, 0.f, 0.f}){};

Tile::Tile(std::vector<int>   shape,
           std::vector<float> shift,
           std::vector<float> scale,
           std::vector<float> bbox)
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
  std::cout << "shape: {" << this->shape[0] << ", " << this->shape[1] << "}, ";
  std::cout << "shift: {" << this->shift[0] << ", " << this->shift[1] << "}, ";
  std::cout << "scale: {" << this->scale[0] << ", " << this->scale[1] << "}, ";
  std::cout << "bbox: {" << this->bbox[0] << ", " << this->bbox[1] << ", "
            << this->bbox[2] << ", " << this->bbox[3] << "}, ";
  std::cout << std::endl;
}

} // namespace hmap
