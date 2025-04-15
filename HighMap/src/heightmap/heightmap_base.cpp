/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <future>
#include <iostream>
#include <thread>

#include "macrologger.h"

#include "highmap/heightmap.hpp"
#include "highmap/interpolate2d.hpp"
#include "highmap/operator.hpp"
#include "highmap/range.hpp"

#include "highmap/internal/vector_utils.hpp"

namespace hmap
{

Heightmap::Heightmap(Vec2<int> shape, Vec2<int> tiling, float overlap)
    : shape(shape), tiling(tiling), overlap(overlap)
{
  this->update_tile_parameters();
}

Heightmap::Heightmap(Vec2<int> shape,
                     Vec2<int> tiling,
                     float     overlap,
                     float     fill_value)
    : shape(shape), tiling(tiling), overlap(overlap)
{
  this->update_tile_parameters();

  transform(
      {this},
      [fill_value](std::vector<hmap::Array *> p_arrays,
                   hmap::Vec2<int>,
                   hmap::Vec4<float>)
      {
        hmap::Array *pa_out = p_arrays[0];
        *pa_out = fill_value;
      },
      TransformMode::DISTRIBUTED);
}

Heightmap::Heightmap(Vec2<int> shape, Vec2<int> tiling)
    : shape(shape), tiling(tiling)
{
  this->update_tile_parameters();
}

Heightmap::Heightmap(Vec2<int> shape) : shape(shape)
{
  this->update_tile_parameters();
}

Heightmap::Heightmap() : shape(0, 0)
{
  this->update_tile_parameters();
}

size_t Heightmap::get_ntiles() const
{
  return this->tiles.size();
}

int Heightmap::get_tile_index(int i, int j) const
{
  return i + j * this->tiling.x;
}

void Heightmap::set_overlap(float new_overlap)
{
  this->overlap = new_overlap;
  this->update_tile_parameters();
}

void Heightmap::set_shape(Vec2<int> new_shape)
{
  this->shape = new_shape;
  this->update_tile_parameters();
}

void Heightmap::set_sto(Vec2<int> new_shape,
                        Vec2<int> new_tiling,
                        float     new_overlap)
{
  if ((this->shape != new_shape) || (this->tiling != new_tiling) ||
      (this->overlap != new_overlap))
  {
    this->shape = new_shape;
    this->tiling = new_tiling;
    this->overlap = new_overlap;
    this->update_tile_parameters();
  }
}

void Heightmap::set_tiling(Vec2<int> new_tiling)
{
  this->tiling = new_tiling;
  this->update_tile_parameters();
}

void Heightmap::from_array_interp(Array &array)
{
  this->from_array_interp_bilinear(array);
}

void Heightmap::from_array_interp_bicubic(Array &array)
{
  std::vector<std::future<void>> futures(this->get_ntiles());

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i] = std::async(&Tile::from_array_interp_bicubic,
                            std::ref(tiles[i]),
                            std::ref(array));

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i].get();
}

void Heightmap::from_array_interp_bilinear(Array &array)
{
  std::vector<std::future<void>> futures(this->get_ntiles());

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i] = std::async(&Tile::from_array_interp,
                            std::ref(tiles[i]),
                            std::ref(array));

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i].get();
}

void Heightmap::from_array_interp_nearest(Array &array)
{
  std::vector<std::future<void>> futures(this->get_ntiles());

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i] = std::async(&Tile::from_array_interp_nearest,
                            std::ref(tiles[i]),
                            std::ref(array));

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i].get();
}

float Heightmap::get_value_bilinear(float x, float y) const
{
  // find corresponding tile
  int it = static_cast<int>(x * this->tiling.x);
  int jt = static_cast<int>(y * this->tiling.y);

  int k = this->get_tile_index(it, jt);

  // coordinates with respect to the tile
  float xt = x - this->tiles[k].bbox.a;
  float yt = y - this->tiles[k].bbox.c;

  float lxt = this->tiles[k].bbox.b - this->tiles[k].bbox.a;
  float lyt = this->tiles[k].bbox.d - this->tiles[k].bbox.c;

  // NB - end points of the bounding box are not included in the grid
  float xgrid = xt / lxt * this->tiles[k].shape.x;
  float ygrid = yt / lyt * this->tiles[k].shape.y;

  int i = static_cast<int>(xgrid);
  int j = static_cast<int>(ygrid);

  float u = xgrid - i;
  float v = ygrid - j;

  int i1 = (i == this->tiles[k].shape.x - 1) ? i - 1 : i + 1;
  int j1 = (j == this->tiles[k].shape.y - 1) ? j - 1 : j + 1;

  float value = bilinear_interp(this->tiles[k](i, j),
                                this->tiles[k](i1, j),
                                this->tiles[k](i, j1),
                                this->tiles[k](i1, j1),
                                u,
                                v);

  return value;
}

float Heightmap::get_value_nearest(float x, float y) const
{
  // find corresponding tile
  int it = static_cast<int>(x * this->tiling.x);
  int jt = static_cast<int>(y * this->tiling.y);

  int k = this->get_tile_index(it, jt);

  // coordinates with respect to the tile
  float xt = x - this->tiles[k].bbox.a;
  float yt = y - this->tiles[k].bbox.c;

  float lxt = this->tiles[k].bbox.b - this->tiles[k].bbox.a;
  float lyt = this->tiles[k].bbox.d - this->tiles[k].bbox.c;

  // NB - end points of the bounding box are not included in the grid
  int i = static_cast<int>(xt / lxt * this->tiles[k].shape.x);
  int j = static_cast<int>(yt / lyt * this->tiles[k].shape.y);

  return this->tiles[k](i, j);
}

void Heightmap::infos()
{
  std::cout << "Heightmap, ";
  std::cout << "address: " << this << ", ";
  std::cout << "shape: {" << this->shape.x << ", " << this->shape.y << "}, ";
  std::cout << "tiling: {" << this->tiling.x << ", " << this->tiling.y << "}, ";
  std::cout << "overlap: " << this->overlap << ", ";
  std::cout << "min: " << this->min() << ", ";
  std::cout << "max: " << this->max();
  std::cout << std::endl;

  for (auto &t : this->tiles)
    t.infos();
}

void Heightmap::inverse()
{
  float hmax = this->max();

  transform(
      {this},
      [hmax](std::vector<hmap::Array *> p_arrays,
             hmap::Vec2<int>,
             hmap::Vec4<float>)
      {
        hmap::Array *pa_out = p_arrays[0];
        *pa_out *= -1.f;
        *pa_out += hmax;
      },
      TransformMode::DISTRIBUTED);
}

float Heightmap::max()
{
  std::vector<float>              max_tiles(this->get_ntiles());
  std::vector<std::future<float>> futures(this->get_ntiles());

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i] = std::async(&Tile::max, tiles[i]);

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    max_tiles[i] = futures[i].get();

  return *std::max_element(max_tiles.begin(), max_tiles.end());
}

void Heightmap::smooth_overlap_buffers()
{
  int delta_buffer_i = (int)(this->overlap * this->shape.x / this->tiling.x);
  int delta_buffer_j = (int)(this->overlap * this->shape.y / this->tiling.y);

  // i-direction pass
  for (int it = 0; it < tiling.x - 1; it++)
    for (int jt = 0; jt < tiling.y; jt++)
    {
      int k = this->get_tile_index(it, jt);
      int kn = this->get_tile_index(it + 1, jt);

      for (int p = 0; p < delta_buffer_i; p++)
        for (int q = 0; q < tiles[k].shape.y; q++)
        {
          float r = (float)p / (float)(delta_buffer_i - 1);
          r = (r * (r * 6.f - 15.f) + 10.f) * r * r * r;
          int pbuf = tiles[k].shape.x - 2 * delta_buffer_i + p;
          tiles[kn](p, q) = (1.f - r) * tiles[k](pbuf, q) + r * tiles[kn](p, q);
          tiles[k](pbuf, q) = tiles[kn](p, q);
        }
    }

  // j-direction
  for (int it = 0; it < tiling.x; it++)
    for (int jt = 0; jt < tiling.y - 1; jt++)
    {
      int k = this->get_tile_index(it, jt);
      int kn = this->get_tile_index(it, jt + 1);

      for (int p = 0; p < tiles[k].shape.x; p++)
        for (int q = 0; q < delta_buffer_j; q++)
        {
          float r = (float)q / (float)(delta_buffer_j - 1);
          r = (r * (r * 6.f - 15.f) + 10.f) * r * r * r;
          int qbuf = tiles[k].shape.y - 2 * delta_buffer_j + q;
          tiles[kn](p, q) = (1.f - r) * tiles[k](p, qbuf) + r * tiles[kn](p, q);
          tiles[k](p, qbuf) = tiles[kn](p, q);
        }
    }
}

float Heightmap::min()
{
  std::vector<float>              min_tiles(this->get_ntiles());
  std::vector<std::future<float>> futures(this->get_ntiles());

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i] = std::async(&Tile::min, tiles[i]);

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    min_tiles[i] = futures[i].get();

  return *std::min_element(min_tiles.begin(), min_tiles.end());
}

float Heightmap::mean()
{
  float mean = this->sum() / (float)(this->shape.x * this->shape.y);
  return mean;
}

void Heightmap::remap(float vmin, float vmax)
{
  float hmin = this->min();
  float hmax = this->max();

  transform(
      {this},
      [vmin, vmax, hmin, hmax](std::vector<hmap::Array *> p_arrays,
                               hmap::Vec2<int>,
                               hmap::Vec4<float>)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::remap(*pa_out, vmin, vmax, hmin, hmax);
      },
      TransformMode::DISTRIBUTED);
}

void Heightmap::remap(float vmin, float vmax, float from_min, float from_max)
{
  transform(
      {this},
      [vmin, vmax, from_min, from_max](std::vector<hmap::Array *> p_arrays,
                                       hmap::Vec2<int>,
                                       hmap::Vec4<float>)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::remap(*pa_out, vmin, vmax, from_min, from_max);
      },
      TransformMode::DISTRIBUTED);
}

float Heightmap::sum()
{
  std::vector<float>              sum_tiles(this->get_ntiles());
  std::vector<std::future<float>> futures(this->get_ntiles());

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i] = std::async(&Tile::sum, tiles[i]);

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    sum_tiles[i] = futures[i].get();

  float sum = 0.f;
  for (auto &v : sum_tiles)
    sum += v;

  return sum;
}

Array Heightmap::to_array()
{
  Array array = Array(this->shape);

  for (int it = 0; it < tiling.x; it++)
    for (int jt = 0; jt < tiling.y; jt++)
    {
      // tile array position within the global array
      int k = this->get_tile_index(it, jt);

      int i1 = (int)(tiles[k].shift.x * this->shape.x);
      int j1 = (int)(tiles[k].shift.y * this->shape.y);

      for (int q = 0; q < tiles[k].shape.y; ++q)
        for (int p = 0; p < tiles[k].shape.x; ++p)
          array(p + i1, q + j1) = tiles[k](p, q);
    }

  return array;
}

Array Heightmap::to_array(Vec2<int> shape_export)
{
  Array array = Array(shape_export);

  // interpolation grid points
  bool endpoint = false;

  std::vector<float> x = linspace(0.f, 1.f, shape_export.x, endpoint);

  std::vector<float> y = linspace(0.f, 1.f, shape_export.y, endpoint);

  // nearest neighbor interpolation since the export is likely to be
  // smaller than the original one
  for (int j = 0; j < shape_export.y; ++j)
    for (int i = 0; i < shape_export.x; ++i)
      array(i, j) = this->get_value_nearest(x[i], y[j]);

  return array;
}

std::vector<uint8_t> Heightmap::to_grayscale_image_8bit()
{
  std::vector<uint8_t> img(this->shape.x * this->shape.y);

  float vmin = this->min();
  float vmax = this->max();
  float inv_vptp = vmin != vmax ? 1.f / (this->max() - vmin) : 0.f;

  for (int it = 0; it < tiling.x; it++)
    for (int jt = 0; jt < tiling.y; jt++)
    {
      // tile array position within the global array
      int k = this->get_tile_index(it, jt);

      // bottom-left indices of the current tile
      int i1 = (int)(tiles[k].shift.x * this->shape.x);
      int j1 = (int)(tiles[k].shift.y * this->shape.y);

      for (int p = 0; p < tiles[k].shape.x; p++)
        for (int q = 0; q < tiles[k].shape.y; q++)
        {
          // linear index for the global image array (flip y axis and
          // change to row/col major)
          int r = (this->shape.y - 1 - q - j1) * this->shape.x + (p + i1);

          // int r = (p + i1) * this->shape.y + (q + j1);

          // remap to [0, 1)
          float v = (tiles[k](p, q) - vmin) * inv_vptp;

          img[r] = static_cast<uint8_t>(v * 255.f);
        }
    }

  return img;
}

std::vector<uint16_t> Heightmap::to_grayscale_image_16bit()
{
  std::vector<uint16_t> img(this->shape.x * this->shape.y);

  float vmin = this->min();
  float vmax = this->max();
  float inv_vptp = vmin != vmax ? 1.f / (this->max() - vmin) : 0.f;

  for (int it = 0; it < tiling.x; it++)
    for (int jt = 0; jt < tiling.y; jt++)
    {
      // tile array position within the global array
      int k = this->get_tile_index(it, jt);

      // bottom-left indices of the current tile
      int i1 = (int)(tiles[k].shift.x * this->shape.x);
      int j1 = (int)(tiles[k].shift.y * this->shape.y);

      for (int p = 0; p < tiles[k].shape.x; p++)
        for (int q = 0; q < tiles[k].shape.y; q++)
        {
          // linear index for the global image array (flip y axis and
          // change to row/col major)
          int r = (this->shape.y - 1 - q - j1) * this->shape.x + (p + i1);

          // int r = (p + i1) * this->shape.y + (q + j1);

          // remap to [0, 1)
          float v = (tiles[k](p, q) - vmin) * inv_vptp;

          img[r] = static_cast<uint16_t>(v * 65535.f);
        }
    }

  return img;
}

std::vector<uint16_t> Heightmap::to_grayscale_image_16bit_multithread()
{
  std::vector<uint16_t> img(this->shape.x * this->shape.y);

  float vmin = this->min();
  float vmax = this->max();
  float inv_vptp = vmin != vmax ? 1.f / (this->max() - vmin) : 0.f;

  std::vector<std::future<void>> futures;

  // --- function to compute for each tile

  auto lambda = [this, &img, vmin, inv_vptp](int it, int jt)
  {
    // tile array position within the global array
    int k = this->get_tile_index(it, jt);

    // bottom-left indices of the current tile
    int i1 = (int)(tiles[k].shift.x * this->shape.x);
    int j1 = (int)(tiles[k].shift.y * this->shape.y);

    for (int p = 0; p < tiles[k].shape.x; p++)
      for (int q = 0; q < tiles[k].shape.y; q++)
      {
        // linear index for the global image array (flip y axis and
        // change to row/col major)
        int r = (this->shape.y - 1 - q - j1) * this->shape.x + (p + i1);

        // remap to [0, 1)
        float v = (tiles[k](p, q) - vmin) * inv_vptp;

        img[r] = static_cast<uint16_t>(v * 65535.f);
      }
  };

  // --- distribute

  for (int it = 0; it < tiling.x; it++)
    for (int jt = 0; jt < tiling.y; jt++)
      futures.push_back(std::async(lambda, it, jt));

  for (auto &f : futures)
    f.get();

  return img;
}

void Heightmap::update_tile_parameters()
{
  tiles.resize(this->tiling.x * this->tiling.y);

  // what the buffers extent to the tile domain at both frontiers
  // (added two times for the tile surrounded by other tiles)
  int delta_buffer_i = (int)(this->overlap * this->shape.x / this->tiling.x);
  int delta_buffer_j = (int)(this->overlap * this->shape.y / this->tiling.y);

  for (int it = 0; it < tiling.x; it++)
    for (int jt = 0; jt < tiling.y; jt++)
    {
      int k = this->get_tile_index(it, jt);

      // overlap extent
      int buffer_i = 0;
      int buffer_j = 0;

      if (it > 0) buffer_i += delta_buffer_i;
      if (it < this->tiling.x - 1) buffer_i += delta_buffer_i;
      if (jt > 0) buffer_j += delta_buffer_j;
      if (jt < this->tiling.y - 1) buffer_j += delta_buffer_j;

      // geometry: shape, shift and scale
      Vec2<int> tile_shape = Vec2<int>(
          this->shape.x / this->tiling.x + buffer_i,
          this->shape.y / this->tiling.y + buffer_j);

      Vec2<float> shift = Vec2<float>((float)it / (float)this->tiling.x,
                                      (float)jt / (float)this->tiling.y);

      // take into account buffers
      if (it > 0) shift.x -= (float)delta_buffer_i / (float)this->shape.x;
      if (jt > 0) shift.y -= (float)delta_buffer_j / (float)this->shape.y;

      Vec2<float> scale = Vec2((float)tile_shape.x / (float)this->shape.x,
                               (float)tile_shape.y / (float)this->shape.y);

      Vec4<float> tile_bbox = Vec4(shift.x,
                                   shift.x + scale.x,
                                   shift.y,
                                   shift.y + scale.y);

      tiles[k] = Tile(tile_shape, shift, scale, tile_bbox);
    }
}

std::vector<float> Heightmap::unique_values()
{
  std::vector<std::vector<float>> tile_unique_values(this->get_ntiles());
  std::vector<std::future<std::vector<float>>> futures(this->get_ntiles());
  std::vector<float>                           hmap_unique_values = {};

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i] = std::async(&Tile::unique_values, tiles[i]);

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    tile_unique_values[i] = futures[i].get();

  // fill heightmap vector values
  for (size_t i = 0; i < this->get_ntiles(); ++i)
    for (auto &v : tile_unique_values[i])
      hmap_unique_values.push_back(v);

  vector_unique_values(hmap_unique_values);
  return hmap_unique_values;
}

} // namespace hmap
