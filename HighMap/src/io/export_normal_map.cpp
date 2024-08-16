/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/gradient.hpp"
#include "highmap/io.hpp"
#include "highmap/operator.hpp"

namespace hmap
{

void export_normal_map_png_8bit(std::string fname, const Array &array)
{
  std::vector<uint8_t> img(array.shape.x * array.shape.y * 3);

  Array dx = gradient_x(array) * array.shape.x;
  Array dy = gradient_y(array) * array.shape.y;

  int k = 0;
  for (int j = array.shape.y - 1; j > -1; j -= 1)
    for (int i = 0; i < array.shape.x; i++)
    {
      Vec3<float> n = Vec3<float>(-dx(i, j), -dy(i, j), 1.f);
      n /= std::hypot(n.x, n.y, n.z);

      img[k++] = (uint8_t)(127.5f * (n.x + 1.f)); // R
      img[k++] = (uint8_t)(127.5f * (n.y + 1.f)); // G
      img[k++] = (uint8_t)(127.5f * (n.z + 1.f)); // B
    }
  write_png_rgb_8bit(fname, img, array.shape);
}

void export_normal_map_png_16bit(std::string fname, const Array &array)
{
  std::vector<uint16_t> img(array.shape.x * array.shape.y * 3);

  Array dx = gradient_x(array) * array.shape.x;
  Array dy = gradient_y(array) * array.shape.y;

  int k = 0;
  for (int j = array.shape.y - 1; j > -1; j -= 1)
    for (int i = 0; i < array.shape.x; i++)
    {
      Vec3<float> n = Vec3<float>(-dx(i, j), -dy(i, j), 1.f);
      n /= std::hypot(n.x, n.y, n.z);

      img[k++] = (uint16_t)(32767.5f * (n.x + 1.f)); // R
      img[k++] = (uint16_t)(32767.5f * (n.y + 1.f)); // G
      img[k++] = (uint16_t)(32767.5f * (n.z + 1.f)); // B
    }
  write_png_rgb_16bit(fname, img, array.shape);
}

} // namespace hmap
