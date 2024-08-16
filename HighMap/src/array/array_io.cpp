/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/io.hpp"

namespace hmap
{

void Array::from_file(std::string fname)
{
  LOG_DEBUG("reading binary file");
  std::ifstream f;
  f.open(fname, std::ios::binary);

  for (auto &v : this->vector)
    f.read(reinterpret_cast<char *>(&v), sizeof(float));
  f.close();
}

void Array::infos(std::string msg) const
{
  std::cout << "Array: " << msg << " ";
  std::cout << "address: " << this << ", ";
  std::cout << "shape: {" << this->shape.x << ", " << this->shape.y << "}"
            << ", ";
  std::cout << "min: " << this->min() << ", ";
  std::cout << "max: " << this->max();
  std::cout << std::endl;
}

void Array::print()
{
  std::cout << std::fixed << std::setprecision(6) << std::setfill('0');
  for (int j = shape.y - 1; j > -1; j--)
  {
    for (int i = 0; i < shape.x; i++)
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
    f.write(reinterpret_cast<const char *>(&v), sizeof(float));

  f.close();
}

void Array::to_png(std::string fname, int cmap, bool hillshading)
{
  std::vector<uint8_t> img(3 * this->shape.x * this->shape.y);
  const float          vmin = this->min();
  const float          vmax = this->max();

  img = colorize(*this, vmin, vmax, cmap, hillshading);
  write_png_rgb_8bit(fname, img, this->shape);
}

void Array::to_png_grayscale_8bit(std::string fname)
{
  const float vmin = this->min();
  const float vmax = this->max();

  std::vector<uint8_t> img(this->shape.x * this->shape.y);

  float a = 0.f;
  float b = 0.f;

  if (vmin != vmax)
  {
    a = 1.f / (vmax - vmin);
    b = -vmin / (vmax - vmin);
  }

  int k = 0;

  for (int j = this->shape.y - 1; j > -1; j -= 1)
    for (int i = 0; i < this->shape.x; i += 1)
    {
      float v = a * (*this)(i, j) + b;
      img[k++] = (uint8_t)(v * 255.f);
    }

  write_png_grayscale_8bit(fname, img, this->shape);
}

void Array::to_png_grayscale_16bit(std::string fname)
{
  const float vmin = this->min();
  const float vmax = this->max();

  std::vector<uint16_t> img(this->shape.x * this->shape.y);

  float a = 0.f;
  float b = 0.f;

  if (vmin != vmax)
  {
    a = 1.f / (vmax - vmin);
    b = -vmin / (vmax - vmin);
  }

  int k = 0;

  for (int j = this->shape.y - 1; j > -1; j -= 1)
    for (int i = 0; i < this->shape.x; i += 1)
    {
      float v = a * (*this)(i, j) + b;
      img[k++] = (uint16_t)(v * 65535.f);
    }

  write_png_grayscale_16bit(fname, img, this->shape);
}

void Array::to_raw_16bit(std::string fname)
{
  write_raw_16bit(fname, *this);
}

} // namespace hmap
