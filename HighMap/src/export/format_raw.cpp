/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cstdint>
#include <fstream>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"

namespace hmap
{

void write_raw_16bit(const std::string &fname, const Array &array)
{
  const float vmin = array.min();
  const float vmax = array.max();
  float       a = 0.f;
  float       b = 0.f;
  if (vmin != vmax)
  {
    a = 1.f / (vmax - vmin);
    b = -vmin / (vmax - vmin);
  }

  a *= 65535.f;
  b *= 65535.f;

  std::ofstream f;
  f.open(fname, std::ios::binary);

  for (int j = array.shape.y - 1; j > -1; j -= 1)
    for (int i = 0; i < array.shape.x; i++)
    {
      uint16_t v = (uint32_t)(a * array(i, j) + b);
      f.write(reinterpret_cast<const char *>(&v), sizeof(uint16_t));
    }

  // for (auto &v : array.vector)
  // {
  //   uint16_t d = (uint32_t)(a * v + b);
  //   f.write(reinterpret_cast<const char *>(&d), sizeof(uint16_t));
  // }

  f.close();
}

} // namespace hmap
