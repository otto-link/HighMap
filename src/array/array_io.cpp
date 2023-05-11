#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"

namespace hmap
{

void Array::infos(std::string msg)
{
  std::cout << "array info: " << msg << std::endl;
  std::cout << " - address: " << this << std::endl;
  std::cout << " - shape: {" << this->shape[0] << ", " << this->shape[1] << "}"
            << std::endl;
  std::cout << " - min: " << this->min() << std::endl;
  std::cout << " - max: " << this->max() << std::endl;
  std::cout << " - ptp: " << this->ptp() << std::endl;
}

void Array::to_png(std::string fname, int cmap)
{
  std::vector<uint8_t> data(IMG_CHANNELS * this->shape[0] * this->shape[1]);
  const float          vmin = this->min();
  const float          vmax = this->max();

  data = colorize(*this, vmin, vmax, cmap);

  // row and column are permutted
  stbi_write_png(fname.c_str(),
                 this->shape[0],
                 this->shape[1],
                 IMG_CHANNELS,
                 data.data(),
                 IMG_CHANNELS * this->shape[0]);
}

} // namespace hmap