/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <future>
#include <thread>

#include "Interpolate.hpp"
#include "macrologger.h"

#include "highmap/heightmap.hpp"
#include "highmap/op.hpp"

#include "op/vector_utils.hpp"

namespace hmap
{

HeightMap::HeightMap(Vec2<int>   shape,
                     Vec4<float> bbox,
                     Vec2<int>   tiling,
                     float       overlap)
    : shape(shape), bbox(bbox), tiling(tiling), overlap(overlap)
{
  this->update_tile_parameters();
}

HeightMap::HeightMap(Vec2<int> shape, Vec2<int> tiling, float overlap)
    : shape(shape), tiling(tiling), overlap(overlap)
{
  this->update_tile_parameters();
}

HeightMap::HeightMap(Vec2<int> shape,
                     Vec2<int> tiling,
                     float     overlap,
                     float     fill_value)
    : shape(shape), tiling(tiling), overlap(overlap)
{
  this->update_tile_parameters();

  transform(*this, [&fill_value](Array &x) { x = fill_value; });
}

HeightMap::HeightMap(Vec2<int> shape, Vec4<float> bbox, Vec2<int> tiling)
    : shape(shape), bbox(bbox), tiling(tiling)
{
  this->update_tile_parameters();
}

HeightMap::HeightMap(Vec2<int> shape, Vec2<int> tiling)
    : shape(shape), tiling(tiling)
{
  this->update_tile_parameters();
}

HeightMap::HeightMap(Vec2<int> shape, Vec4<float> bbox)
    : shape(shape), bbox(bbox)
{
  this->update_tile_parameters();
}

HeightMap::HeightMap(Vec2<int> shape) : shape(shape)
{
  this->update_tile_parameters();
}

HeightMap::HeightMap() : shape(0, 0)
{
  this->update_tile_parameters();
}

size_t HeightMap::get_ntiles()
{
  return this->tiles.size();
}

int HeightMap::get_tile_index(int i, int j)
{
  return i + j * this->tiling.x;
}

void HeightMap::set_overlap(float new_overlap)
{
  this->overlap = new_overlap;
  this->update_tile_parameters();
}

void HeightMap::set_shape(Vec2<int> new_shape)
{
  this->shape = new_shape;
  this->update_tile_parameters();
}

void HeightMap::set_sto(Vec2<int> new_shape,
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

void HeightMap::set_tiling(Vec2<int> new_tiling)
{
  this->tiling = new_tiling;
  this->update_tile_parameters();
}

void HeightMap::from_array_interp(Array &array)
{
  std::vector<std::future<void>> futures(this->get_ntiles());

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i] = std::async(&Tile::from_array_interp,
                            std::ref(tiles[i]),
                            std::ref(array));

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i].get();
}

void HeightMap::from_array_interp_nearest(Array &array)
{
  std::vector<std::future<void>> futures(this->get_ntiles());

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i] = std::async(&Tile::from_array_interp_nearest,
                            std::ref(tiles[i]),
                            std::ref(array));

  for (decltype(futures)::size_type i = 0; i < this->get_ntiles(); ++i)
    futures[i].get();
}

void HeightMap::infos()
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

void HeightMap::inverse()
{
  float hmax = this->max();
  transform(*this,
            [hmax](Array &x)
            {
              x *= -1.f;
              x += hmax;
            });
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

float HeightMap::mean()
{
  float mean = this->sum() / (float)(this->shape.x * this->shape.y);
  return mean;
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

float HeightMap::sum()
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

Array HeightMap::to_array()
{
  return this->to_array(this->shape);
}

Array HeightMap::to_array(Vec2<int> shape_export)
{
  // TODO: stepping not robust with overlapping
  Vec2<int> step = {this->shape.x / shape_export.x,
                    this->shape.y / shape_export.y};

  Array array = Array(shape_export);

  for (int it = 0; it < tiling.x; it++)
    for (int jt = 0; jt < tiling.y; jt++)
    {
      // tile array position within the global array
      int k = this->get_tile_index(it, jt);

      int i1 = (int)(tiles[k].shift.x * this->shape.x);
      int j1 = (int)(tiles[k].shift.y * this->shape.y);

      for (int p = 0; p < tiles[k].shape.x; p += step.x)
        for (int q = 0; q < tiles[k].shape.y; q += step.y)
          array((p + i1) / step.x, (q + j1) / step.y) = tiles[k](p, q);
    }

  return array;
}

void HeightMap::update_tile_parameters()
{
  tiles.resize(this->tiling.x * this->tiling.y);

  float lx = this->bbox.b - this->bbox.a;
  float ly = this->bbox.d - this->bbox.c;

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

      if (it > 0)
        buffer_i += delta_buffer_i;
      if (it < this->tiling.x - 1)
        buffer_i += delta_buffer_i;
      if (jt > 0)
        buffer_j += delta_buffer_j;
      if (jt < this->tiling.y - 1)
        buffer_j += delta_buffer_j;

      // geometry: shape, shift and scale
      Vec2<int> tile_shape = Vec2<int>(
          this->shape.x / this->tiling.x + buffer_i,
          this->shape.y / this->tiling.y + buffer_j);

      Vec2<float> shift = Vec2<float>((float)it / (float)this->tiling.x,
                                      (float)jt / (float)this->tiling.y);

      // take into account buffers
      if (it > 0)
        shift.x -= (float)delta_buffer_i / (float)this->shape.x;
      if (jt > 0)
        shift.y -= (float)delta_buffer_j / (float)this->shape.y;

      Vec2<float> scale = Vec2((float)tile_shape.x / (float)this->shape.x,
                               (float)tile_shape.y / (float)this->shape.y);

      Vec4<float> tile_bbox = Vec4(this->bbox.a + shift.x * lx,
                                   this->bbox.a + (shift.x + scale.x) * lx,
                                   this->bbox.c + shift.y * ly,
                                   this->bbox.c + (shift.y + scale.y) * ly);

      tiles[k] = Tile(tile_shape, shift, scale, tile_bbox);
    }
}

std::vector<float> HeightMap::unique_values()
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
