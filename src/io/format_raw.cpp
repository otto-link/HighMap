/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"

namespace hmap
{

void write_raw_16bit(std::string fname, Array &array)
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

  for (auto &v : array.vector)
  {
    uint16_t d = (uint32_t)(a * v + b);
    f.write(reinterpret_cast<const char *>(&d), sizeof(uint16_t));
  }

  f.close();
}

} // namespace hmap
