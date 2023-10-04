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
  {
    f.write(reinterpret_cast<const char *>(&v), sizeof(float));
  }
  f.close();
}

void Array::to_png(std::string fname, int cmap, bool hillshading)
{
  std::vector<uint8_t> data(IMG_CHANNELS * this->shape.x * this->shape.y);
  const float          vmin = this->min();
  const float          vmax = this->max();

  data = colorize(*this, vmin, vmax, cmap, hillshading);

  // row and column are permutted
  stbi_write_png(fname.c_str(),
                 this->shape.x,
                 this->shape.y,
                 IMG_CHANNELS,
                 data.data(),
                 IMG_CHANNELS * this->shape.x);
}

void Array::to_png16bit(std::string fname)
{
  const float vmin = this->min();
  const float vmax = this->max();

  std::vector<uint16_t> data(this->shape.x * this->shape.y);

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
      data[k++] = (uint16_t)(v * 65535.f);
    }

  // ---

  FILE *fp = fopen(fname.c_str(), "wb");
  if (!fp)
    LOG_ERROR("Error opening file for writing: %s", fname.c_str());

  png_structp png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png)
  {
    LOG_ERROR("Error creating PNG write structure");
    fclose(fp);
  }

  png_infop info = png_create_info_struct(png);
  if (!info)
  {
    LOG_ERROR("Error creating PNG info structure");
    png_destroy_write_struct(&png, NULL);
    fclose(fp);
  }

  if (setjmp(png_jmpbuf(png)))
  {
    LOG_ERROR("Error during PNG creation");
    png_destroy_write_struct(&png, &info);
    fclose(fp);
  }

  // Initialize the PNG file
  png_init_io(png, fp);

  // Set the image properties
  png_set_IHDR(png,
               info,
               this->shape.x,
               this->shape.y,
               16,
               PNG_COLOR_TYPE_GRAY,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  // Write the PNG header
  png_write_info(png, info);

  // Allocate memory for row pointers
  int        width = this->shape.x;
  int        height = this->shape.y;
  png_bytep *row_pointers = new png_bytep[height];
  for (int i = 0; i < height; ++i)
  {
    row_pointers[i] = (png_bytep)&data[i * width];
  }

  // Write the image data
  png_write_image(png, row_pointers);

  // End the PNG write process
  png_write_end(png, NULL);

  // Clean up
  png_destroy_write_struct(&png, &info);
  fclose(fp);
  delete[] row_pointers;

  LOG_DEBUG("PNG file saved successfully: %s", fname.c_str());
}

} // namespace hmap
