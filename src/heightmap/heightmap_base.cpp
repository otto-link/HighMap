/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <future>
#include <thread>

#include "macrologger.h"

#include "highmap/heightmap.hpp"
#include "highmap/op.hpp"

namespace hmap
{

HeightMap::HeightMap(std::vector<int>   shape,
                     std::vector<float> bbox,
                     std::vector<int>   tiling,
                     float              overlap)
    : shape(shape), bbox(bbox), tiling(tiling), overlap(overlap)
{
  this->update_tile_parameters();
}

HeightMap::HeightMap(std::vector<int> shape,
                     std::vector<int> tiling,
                     float            overlap)
    : shape(shape), tiling(tiling), overlap(overlap)
{
  this->update_tile_parameters();
}

HeightMap::HeightMap(std::vector<int>   shape,
                     std::vector<float> bbox,
                     std::vector<int>   tiling)
    : shape(shape), bbox(bbox), tiling(tiling)
{
  this->update_tile_parameters();
}

HeightMap::HeightMap(std::vector<int> shape, std::vector<int> tiling)
    : shape(shape), tiling(tiling)
{
  this->update_tile_parameters();
}

HeightMap::HeightMap(std::vector<int> shape, std::vector<float> bbox)
    : shape(shape), bbox(bbox)
{
  this->update_tile_parameters();
}

HeightMap::HeightMap(std::vector<int> shape) : shape(shape)
{
  this->update_tile_parameters();
}

HeightMap::HeightMap() : shape({0, 0})
{
  this->update_tile_parameters();
}

size_t HeightMap::get_ntiles()
{
  return this->tiles.size();
}

int HeightMap::get_tile_index(int i, int j)
{
  return i + j * this->tiling[0];
}

void HeightMap::set_shape(std::vector<int> new_shape)
{
  this->shape = new_shape;
  this->update_tile_parameters();
}

void HeightMap::set_tiling(std::vector<int> new_tiling)
{
  this->tiling = new_tiling;
  this->update_tile_parameters();
}

void HeightMap::infos()
{
  std::cout << "Heightmap, ";
  std::cout << "address: " << this << ", ";
  std::cout << "shape: {" << this->shape[0] << ", " << this->shape[1] << "}, ";
  std::cout << "tiling: {" << this->tiling[0] << ", " << this->tiling[1]
            << "}, ";
  std::cout << "overlap: " << this->overlap << ", ";
  std::cout << "min: " << this->min() << ", ";
  std::cout << "max: " << this->max();
  std::cout << std::endl;

  for (auto &t : this->tiles)
    t.infos();
}

float HeightMap::max()
{
  std::vector<float>              max_tiles(this->get_ntiles());
  std::vector<std::future<float>> futures(this->get_ntiles());

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i] = std::async(&Tile::max, tiles[i]);

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    max_tiles[i] = futures[i].get();

  return *std::max_element(max_tiles.begin(), max_tiles.end());
}

void HeightMap::smooth_overlap_buffers()
{
  int delta_buffer_i = (int)(this->overlap * this->shape[0] / this->tiling[0]);
  int delta_buffer_j = (int)(this->overlap * this->shape[1] / this->tiling[1]);

  // i-direction pass
  for (int it = 0; it < tiling[0] - 1; it++)
    for (int jt = 0; jt < tiling[1]; jt++)
    {
      int k = this->get_tile_index(it, jt);
      int kn = this->get_tile_index(it + 1, jt);

      for (int p = 0; p < delta_buffer_i; p++)
        for (int q = 0; q < tiles[k].shape[1]; q++)
        {
          float r = (float)p / (float)(delta_buffer_i - 1);
          r = (r * (r * 6.f - 15.f) + 10.f) * r * r * r;
          int pbuf = tiles[k].shape[0] - 2 * delta_buffer_i + p;
          tiles[kn](p, q) = (1.f - r) * tiles[k](pbuf, q) + r * tiles[kn](p, q);
          tiles[k](pbuf, q) = tiles[kn](p, q);
        }
    }

  // j-direction
  for (int it = 0; it < tiling[0]; it++)
    for (int jt = 0; jt < tiling[1] - 1; jt++)
    {
      int k = this->get_tile_index(it, jt);
      int kn = this->get_tile_index(it, jt + 1);

      for (int p = 0; p < tiles[k].shape[0]; p++)
        for (int q = 0; q < delta_buffer_j; q++)
        {
          float r = (float)q / (float)(delta_buffer_j - 1);
          r = (r * (r * 6.f - 15.f) + 10.f) * r * r * r;
          int qbuf = tiles[k].shape[1] - 2 * delta_buffer_j + q;
          tiles[kn](p, q) = (1.f - r) * tiles[k](p, qbuf) + r * tiles[kn](p, q);
          tiles[k](p, qbuf) = tiles[kn](p, q);
        }
    }
}

float HeightMap::min()
{
  std::vector<float>              min_tiles(this->get_ntiles());
  std::vector<std::future<float>> futures(this->get_ntiles());

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i] = std::async(&Tile::min, tiles[i]);

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    min_tiles[i] = futures[i].get();

  return *std::min_element(min_tiles.begin(), min_tiles.end());
}

void HeightMap::remap(float vmin, float vmax)
{
  float hmin = this->min();
  float hmax = this->max();
  transform(*this,
            [vmin, vmax, hmin, hmax](Array &x)
            { hmap::remap(x, vmin, vmax, hmin, hmax); });
}

void HeightMap::remap(float vmin, float vmax, float from_min, float from_max)
{
  transform(*this,
            [vmin, vmax, from_min, from_max](Array &x)
            { hmap::remap(x, vmin, vmax, from_min, from_max); });
}

Array HeightMap::to_array()
{
  return this->to_array(this->shape);
}

Array HeightMap::to_array(std::vector<int> shape_export)
{
  // TODO: stepping not robust with overlapping
  std::vector<int> step = {this->shape[0] / shape_export[0],
                           this->shape[1] / shape_export[1]};

  Array array = Array(shape_export);

  for (int it = 0; it < tiling[0]; it++)
    for (int jt = 0; jt < tiling[1]; jt++)
    {
      // tile array position within the global array
      int k = this->get_tile_index(it, jt);

      int i1 = (int)(tiles[k].shift[0] * this->shape[0]);
      int j1 = (int)(tiles[k].shift[1] * this->shape[1]);

      for (int p = 0; p < tiles[k].shape[0]; p += step[0])
        for (int q = 0; q < tiles[k].shape[1]; q += step[1])
          array((p + i1) / step[0], (q + j1) / step[1]) = tiles[k](p, q);
    }

  return array;
}

void HeightMap::update_tile_parameters()
{
  tiles.resize(this->tiling[0] * this->tiling[1]);

  float lx = this->bbox[1] - this->bbox[0];
  float ly = this->bbox[3] - this->bbox[2];

  // what the buffers extent to the tile domain at both frontiers
  // (added two times for the tile surrounded by other tiles)
  int delta_buffer_i = (int)(this->overlap * this->shape[0] / this->tiling[0]);
  int delta_buffer_j = (int)(this->overlap * this->shape[1] / this->tiling[1]);

  for (int it = 0; it < tiling[0]; it++)
    for (int jt = 0; jt < tiling[1]; jt++)
    {
      int k = this->get_tile_index(it, jt);

      // overlap extent
      int buffer_i = 0;
      int buffer_j = 0;

      if (it > 0)
        buffer_i += delta_buffer_i;
      if (it < this->tiling[0] - 1)
        buffer_i += delta_buffer_i;
      if (jt > 0)
        buffer_j += delta_buffer_j;
      if (jt < this->tiling[1] - 1)
        buffer_j += delta_buffer_j;

      // geometry: shape, shift and scale
      std::vector<int> tile_shape = {
          this->shape[0] / this->tiling[0] + buffer_i,
          this->shape[1] / this->tiling[1] + buffer_j};

      std::vector<float> shift = {(float)it / (float)this->tiling[0],
                                  (float)jt / (float)this->tiling[1]};

      // take into account buffers
      if (it > 0)
        shift[0] -= (float)delta_buffer_i / (float)this->shape[0];
      if (jt > 0)
        shift[1] -= (float)delta_buffer_j / (float)this->shape[1];

      std::vector<float> scale = {(float)tile_shape[0] / (float)this->shape[0],
                                  (float)tile_shape[1] / (float)this->shape[1]};

      std::vector<float> tile_bbox = {
          this->bbox[0] + shift[0] * lx,
          this->bbox[0] + (shift[0] + scale[0]) * lx,
          this->bbox[2] + shift[1] * ly,
          this->bbox[2] + (shift[1] + scale[1]) * ly};

      tiles[k] = Tile(tile_shape, shift, scale, tile_bbox);
    }
}

} // namespace hmap
