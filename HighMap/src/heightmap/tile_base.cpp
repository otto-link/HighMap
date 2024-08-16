/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/operator.hpp"

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

void Tile::from_array_interp(Array &array)
{
  std::vector<float> x = linspace(this->shift.x,
                                  this->shift.x + this->scale.x,
                                  this->shape.x,
                                  false);
  std::vector<float> y = linspace(this->shift.y,
                                  this->shift.y + this->scale.y,
                                  this->shape.y,
                                  false);

  for (auto &v : x)
    v *= array.shape.x - 1;

  for (auto &v : y)
    v *= array.shape.y - 1;

  for (int i = 0; i < shape.x; i++)
    for (int j = 0; j < shape.y; j++)
    {
      int ip = std::clamp((int)x[i], 0, array.shape.x - 1);
      int jp = std::clamp((int)y[j], 0, array.shape.y - 1);

      float u = x[i] - ip;
      float v = y[j] - jp;
      (*this)(i, j) = array.get_value_bilinear_at(ip, jp, u, v);
    }
}

void Tile::from_array_interp_nearest(Array &array)
{
  std::vector<float> x = linspace(this->shift.x,
                                  this->shift.x + this->scale.x,
                                  this->shape.x,
                                  false);
  std::vector<float> y = linspace(this->shift.y,
                                  this->shift.y + this->scale.y,
                                  this->shape.y,
                                  false);

  for (auto &v : x)
    v *= array.shape.x - 1;

  for (auto &v : y)
    v *= array.shape.y - 1;

  for (int i = 0; i < shape.x; i++)
    for (int j = 0; j < shape.y; j++)
    {
      int ip = (int)x[i];
      int jp = (int)y[j];
      (*this)(i, j) = array(ip, jp);
    }
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
