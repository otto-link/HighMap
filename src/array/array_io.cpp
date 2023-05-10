#include <cstdint>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"

void hmap::Array::to_png(std::string fname, int cmap)
{
  std::vector<uint8_t> data(IMG_CHANNELS * this->shape[0] * this->shape[1]);
  const float          vmin = this->min();
  const float          vmax = this->max();

  data = hmap::colorize(*this, vmin, vmax, cmap);

  // row and column are permutted
  stbi_write_png(fname.c_str(),
                 this->shape[0],
                 this->shape[1],
                 IMG_CHANNELS,
                 data.data(),
                 IMG_CHANNELS * this->shape[0]);
}