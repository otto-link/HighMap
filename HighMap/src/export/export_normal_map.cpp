/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/tensor.hpp"
#include "highmap/export.hpp"
#include "highmap/gradient.hpp"
#include "highmap/operator.hpp"

namespace hmap
{

Tensor compute_nmap(const Array &array)
{
  Tensor nmap = Tensor(array.shape, 3);

  Array dx = gradient_x(array) * array.shape.x;
  Array dy = gradient_y(array) * array.shape.y;

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      Vec3<float> n = Vec3<float>(-dx(i, j), -dy(i, j), 1.f);
      n /= std::hypot(n.x, n.y, n.z);

      nmap(i, j, 0) = 0.5f * (n.x + 1.f);
      nmap(i, j, 1) = 0.5f * (n.y + 1.f);
      nmap(i, j, 2) = 0.5f * (n.z + 1.f);
    }
  return nmap;
}

void export_normal_map_png_8bit(std::string fname, const Array &array)
{
  Tensor nmap = compute_nmap(array);
  nmap.to_png_8bit(fname);
}

void export_normal_map_png_16bit(std::string fname, const Array &array)
{
  Tensor nmap = compute_nmap(array);
  nmap.to_png_16bit(fname);
}

} // namespace hmap
