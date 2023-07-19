/**
 * @file erosion.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <fstream>

#define PACK_RGBA(r, g, b) (uint32_t)(r << 16 | g << 8 | b)

#define UNPACK_COLOR(color, r, g, b)                                           \
  r = (color >> 16) & 0xff;                                                    \
  g = (color >> 8) & 0xff;                                                     \
  b = color & 0xff;

namespace hmap
{

struct Clut
{
  std::vector<int>      shape;
  std::vector<uint32_t> data;

  Clut(std::vector<int> shape, std::vector<uint32_t> data)
      : shape(shape), data(data)
  {
  }

  Clut(std::vector<int> shape, std::string fname) : shape(shape)
  {
    int size = 1;
    for (auto &v : shape)
      size *= v;

    this->data.resize(size);
    this->read_file(fname);
  }

  inline int get_index(float x, int idim)
  {
    return (int)(x * (shape[idim] - 1));
  }

  inline int get_index(float x, int idim, float &u)
  {
    float xn = x * (shape[idim] - 1);
    u = xn - (int)xn;
    return (int)xn;
  }

  void read_file(std::string fname)
  {
    std::ifstream f(fname, std::ios::in | std::ios::binary);
    if (f.is_open())
      for (auto &v : this->data)
        f.read((char *)&v, sizeof(v));
  }
};

struct Clut1D : public Clut
{
public:
  Clut1D(std::vector<int> shape, std::vector<uint32_t> data) : Clut(shape, data)
  {
  }

  inline uint32_t value_at(int i)
  {
    return this->data[i];
  }
};

struct Clut2D : public Clut
{
public:
  Clut2D(std::vector<int> shape, std::vector<uint32_t> data) : Clut(shape, data)
  {
  }

  inline uint32_t value_at(int i, int j)
  {
    return this->data[i + j * this->shape[0]];
  }
};

struct Clut3D : public Clut
{
public:
  Clut3D(std::vector<int> shape, std::vector<uint32_t> data) : Clut(shape, data)
  {
  }

  Clut3D(std::vector<int> shape, std::string fname) : Clut(shape, fname)
  {
  }

  inline uint32_t value_at(int i, int j, int k)
  {
    return this
        ->data[i + j * this->shape[0] + k * this->shape[0] * this->shape[1]];
  }
};

} // namespace hmap
