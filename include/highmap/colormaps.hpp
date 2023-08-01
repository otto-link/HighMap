/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file colormaps.hpp
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

#include "highmap/vector.hpp"

#define PACK_RGBA(r, g, b) (uint32_t)(r << 16 | g << 8 | b)

#define UNPACK_COLOR(color, r, g, b)                                           \
  r = (color >> 16) & 0xff;                                                    \
  g = (color >> 8) & 0xff;                                                     \
  b = color & 0xff;

namespace hmap
{

/**
 * @brief Colormaps.
 *
 * @note Use the inverse value of the cmap to reverse it. For instance
 * `hmap::cmap::gray` and `-hmap::cmap::gray`.
 *
 * **Example**
 * @include ex_colormaps.cpp
 *
 * **Result**
 * @image html ex_colormaps0.png
 * @image html ex_colormaps1.png
 * @image html ex_colormaps2.png
 * @image html ex_colormaps3.png
 * @image html ex_colormaps4.png
 * @image html ex_colormaps5.png
 * @image html ex_colormaps6.png
 * @image html ex_colormaps7.png
 * @image html ex_colormaps8.png
 * @image html ex_colormaps9.png
 * @image html ex_colormaps10.png
 */
enum cmap : int
{
  blues = 1,         ///< bone
  bone = 2,          ///< blues
  gray = 3,          ///< gray
  hot = 4,           ///< hot
  inferno = 5,       ///< inferno
  jet = 6,           ///< jet
  magma = 7,         ///< magma
  nipy_spectral = 8, ///< nipy_spectral
  seismic = 9,       ///< seismic
  terrain = 10,      ///< terrain
  viridis = 11       ///< viridis
};

#define CMAP_SIZE 12
#define CMAP_BLUES                                                             \
  {                                                                            \
    536683, 542862, 1269671, 2586296, 4165572, 6071248, 8239322, 10603490,     \
        12507116, 13886451, 15003641, 16251903                                 \
  }
#define CMAP_BONE                                                              \
  {                                                                            \
    0, 1315868, 2631736, 3947604, 5329264, 6646917, 7964825, 9282733,          \
        10666178, 12572374, 14674666, 16777215                                 \
  }
#define CMAP_GRAY                                                              \
  {                                                                            \
    0, 1513239, 3026478, 4539717, 6052956, 7566195, 9145227, 10658466,         \
        12171705, 13684944, 15198183, 16777215                                 \
  }
#define CMAP_HOT                                                               \
  {                                                                            \
    655360, 4653056, 8650752, 12648448, 16646144, 16726784, 16742400,          \
        16758016, 16773632, 16777032, 16777123, 16777215                       \
  }
#define CMAP_INFERNO                                                           \
  {                                                                            \
    3, 1247796, 3737954, 6230638, 8724586, 11087454, 13320265, 15096878,       \
        16220944, 16494098, 16046923, 16580260                                 \
  }
#define CMAP_JET                                                               \
  {                                                                            \
    127, 232, 14847, 38655, 783339, 5636000, 10551125, 15466251, 16755712,     \
        16733696, 15204352, 8323072                                            \
  }
#define CMAP_MAGMA                                                             \
  {                                                                            \
    3, 1117233, 3280999, 5772670, 8266881, 10694782, 13122930, 15225953,       \
        16350557, 16689267, 16634517, 16514239                                 \
  }
#define CMAP_NIPY_SPECTRAL                                                     \
  {                                                                            \
    0, 8650901, 202, 34525, 43680, 39936, 55552, 8912640, 16243456, 16736512,  \
        14221312, 13421772                                                     \
  }
#define CMAP_SEISMIC                                                           \
  {                                                                            \
    76, 141, 206, 1513471, 7566335, 13684991, 16765136, 16741235, 16717591,    \
        14417920, 11337728, 8323072                                            \
  }
#define CMAP_TERRAIN                                                           \
  {                                                                            \
    3355545, 1339606, 43470, 1560682, 7594877, 13694351, 15196556, 12166515,   \
        9136730, 10651778, 13681600, 16777215                                  \
  }
#define CMAP_VIRIDIS                                                           \
  {                                                                            \
    4456788, 4727154, 4341124, 3692428, 2977678, 2393485, 2005641, 2797694,    \
        5358696, 8836169, 12771106, 16639780                                   \
  }

/**
 * @brief Abstract Color Lookup Table class.
 *
 */
struct Clut
{
  /**
   * @brief Data shape.
   *
   */
  std::vector<int> shape;

  /**
   * @brief Raw uint8 RGB color data packed as uint32
   *
   */
  std::vector<uint32_t> data;

  /**
   * @brief Construct a new Clut object from a vector.
   *
   * @param shape Shape.
   * @param data Data.
   */
  Clut(std::vector<int> shape, std::vector<uint32_t> data)
      : shape(shape), data(data)
  {
  }

  /**
   * @brief Construct a new Clut object from a binary file.
   *
   * @param shape Shape.
   * @param fname Filename.
   */
  Clut(std::vector<int> shape, std::string fname) : shape(shape)
  {
    int size = 1;
    for (auto &v : shape)
      size *= v;

    this->data.resize(size);
    this->read_file(fname);
  }

  /**
   * @brief Get the index corresponding to a float x in [0, 1]
   *
   * @param x
   * @param idim
   * @return int
   */
  inline int get_index(float x, int idim)
  {
    return (int)(x * (shape[idim] - 1));
  }

  /**
   * @brief Get the index object to a float x in [0, 1] and the offset u in [0,
   * 1] (for linear interoplation).
   *
   * @param x[in] position in [0, 1]
   * @param idim[in] Dimension where the index is requested.
   * @param u[out] Offset.
   * @return int Index.
   */
  inline int get_index(float x, int idim, float &u)
  {
    float xn = x * (shape[idim] - 1);
    u = xn - (int)xn;
    return (int)xn;
  }

  /**
   * @brief Read binary file into a vector.
   *
   * @param fname File name.
   */
  void read_file(std::string fname)
  {
    std::ifstream f(fname, std::ios::in | std::ios::binary);
    if (f.is_open())
      for (auto &v : this->data)
        f.read((char *)&v, sizeof(v));
  }
};

/**
 * @brief 1D Color Lookup Table class.
 *
 */
struct Clut1D : public Clut
{
public:
  Clut1D(std::vector<int> shape, std::vector<uint32_t> data) : Clut(shape, data)
  {
  }

  /**
   * @brief Return the table value at i.
   *
   * @param i Index.
   * @return uint32_t Packed color.
   */
  inline uint32_t value_at(int i)
  {
    return this->data[i];
  }
};

/**
 * @brief 2D Color Lookup Table class.
 *
 */
struct Clut2D : public Clut
{
public:
  Clut2D(std::vector<int> shape, std::vector<uint32_t> data) : Clut(shape, data)
  {
  }

  /**
   * @brief Return the table value at (i, j).
   *
   * @param i Index.
   * @param j Index.
   * @return uint32_t Packed color.
   */
  inline uint32_t value_at(int i, int j)
  {
    return this->data[i + j * this->shape[0]];
  }
};

/**
 * @brief 3D Color Lookup Table class.
 *
 */
struct Clut3D : public Clut
{
public:
  Clut3D(std::vector<int> shape, std::vector<uint32_t> data) : Clut(shape, data)
  {
  }

  Clut3D(std::vector<int> shape, std::string fname) : Clut(shape, fname)
  {
  }

  /**
   * @brief Return the table value at (i, j, k).
   *
   * @param i Index.
   * @param j Index.
   * @param k Index.
   * @return uint32_t Packed color.
   */
  inline uint32_t value_at(int i, int j, int k)
  {
    return this
        ->data[i + j * this->shape[0] + k * this->shape[0] * this->shape[1]];
  }
};

} // namespace hmap
