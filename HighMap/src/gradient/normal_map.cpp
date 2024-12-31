/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "highmap/gradient.hpp"

namespace hmap
{

Tensor normal_map(const Array &array)
{
  Tensor nmap = Tensor(array.shape, 3);

  Array dx = gradient_x(array) * array.shape.x;
  Array dy = gradient_y(array) * array.shape.y;

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      Vec3<float> n = Vec3<float>(-dx(i, j), -dy(i, j), 1.f);
      n /= std::hypot(n.x, n.y, n.z);

      nmap(i, j, 0) = 0.5f * (n.x + 1.f);
      nmap(i, j, 1) = 0.5f * (n.y + 1.f);
      nmap(i, j, 2) = 0.5f * (n.z + 1.f);
    }
  return nmap;
}

Array normal_map_to_heightmap(const Tensor &nmap)
{
  Vec2<int> shape(nmap.shape.x, nmap.shape.y);
  Array     z1(shape);
  Array     z2(shape);
  Array     dx(shape);
  Array     dy(shape);

  for (int j = 1; j < shape.y; ++j)
    for (int i = 1; i < shape.x; ++i)
    {
      float nz = std::max(1e-6f, 2.f * nmap(i, j, 2) - 1.f);
      dx(i, j) = -(2.f * nmap(i, j, 0) - 1.f) / nz;
      dy(i, j) = -(2.f * nmap(i, j, 1) - 1.f) / nz;
    }

  for (int j = 1; j < shape.y; ++j)
    for (int i = 1; i < shape.x; ++i)
    {
      z1(i, j) = z1(i, j - 1) + dy(i, j);
      z1(i, j) = z1(i - 1, j) + dx(i, j);
    }

  for (int j = 1; j < shape.y; ++j)
    for (int i = 1; i < shape.x; ++i)
    {
      z2(i, j) = z2(i - 1, j) + dx(i, j);
      z2(i, j) = z2(i, j - 1) + dy(i, j);
    }

  return z1 + z2;
}

} // namespace hmap
