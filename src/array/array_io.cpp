/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdint.h>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "macrologger.h"
#include "stb_image_write.h"

#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"

namespace hmap
{

void Array::infos(std::string msg) const
{
  std::cout << "Array: " << msg << " ";
  std::cout << "address: " << this << ", ";
  std::cout << "shape: {" << this->shape[0] << ", " << this->shape[1] << "}"
            << ", ";
  std::cout << "min: " << this->min() << ", ";
  std::cout << "max: " << this->max();
  std::cout << std::endl;
}

void Array::print()
{
  std::cout << std::fixed << std::setprecision(6) << std::setfill('0');
  for (int j = shape[1] - 1; j > -1; j--)
  {
    for (int i = 0; i < shape[0]; i++)
    {
      std::cout << std::setw(5) << (*this)(i, j) << " ";
    }
    std::cout << std::endl;
  }
}

void Array::to_file(std::string fname)
{
  LOG_DEBUG("writing binary file");
  std::ofstream f;
  f.open(fname, std::ios::binary);

  for (auto &v : this->vector)
  {
    f.write(reinterpret_cast<const char *>(&v), sizeof(float));
  }
  f.close();
}

void Array::to_png(std::string fname, int cmap, bool hillshading)
{
  std::vector<uint8_t> data(IMG_CHANNELS * this->shape[0] * this->shape[1]);
  const float          vmin = this->min();
  const float          vmax = this->max();

  data = colorize(*this, vmin, vmax, cmap, hillshading);

  // row and column are permutted
  stbi_write_png(fname.c_str(),
                 this->shape[0],
                 this->shape[1],
                 IMG_CHANNELS,
                 data.data(),
                 IMG_CHANNELS * this->shape[0]);
}

} // namespace hmap
