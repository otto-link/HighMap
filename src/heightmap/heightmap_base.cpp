#include "macrologger.h"

#include "highmap/heightmap.hpp"
#include "highmap/op.hpp"

namespace hmap
{

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
  std::cout << "min: " << this->min() << ", ";
  std::cout << "max: " << this->max();
  std::cout << std::endl;

  for (auto &t : this->tiles)
    t.infos();
}

float HeightMap::min()
{
  // retrieve the min for each tile
  std::vector<float> min_tiles;
  // transform(*this, [&min_tiles](Array &x) { min_tiles.push_back(x.min()); });
  // // TODO mem issue
  for (auto &t : this->tiles)
    min_tiles.push_back(t.min());
  return *std::min_element(min_tiles.begin(), min_tiles.end());
}

float HeightMap::max()
{
  // retrieve the max for each tile
  std::vector<float> max_tiles;
  for (auto &t : this->tiles)
    max_tiles.push_back(t.max());
  return *std::max_element(max_tiles.begin(), max_tiles.end());
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
  std::vector<int> step = {this->shape[0] / shape_export[0],
                           this->shape[1] / shape_export[1]};

  Array array = Array(shape_export);

  for (int it = 0; it < tiling[0]; it++)
    for (int jt = 0; jt < tiling[1]; jt++)
    {
      // tile array position within the global array
      int i1 = it * this->shape_tile[0];
      int j1 = jt * this->shape_tile[1];
      int k = this->get_tile_index(it, jt);

      for (int p = 0; p < this->shape_tile[0]; p += step[0])
        for (int q = 0; q < this->shape_tile[1]; q += step[1])
          array((p + i1) / step[0], (q + j1) / step[1]) = tiles[k](p, q);
    }

  return array;
}

void HeightMap::update_tile_parameters()
{
  tiles.resize(this->tiling[0] * this->tiling[1]);

  // tile shapes
  this->shape_tile = {this->shape[0] / this->tiling[0],
                      this->shape[1] / this->tiling[1]};

  this->tile_scale = {1.f / this->tiling[0], 1.f / this->tiling[1]};

  float lx_tile = (this->bbox[1] - this->bbox[0]) / (float)this->tiling[0];
  float ly_tile = (this->bbox[3] - this->bbox[2]) / (float)this->tiling[1];

  for (int it = 0; it < tiling[0]; it++)
    for (int jt = 0; jt < tiling[1]; jt++)
    {
      int   k = this->get_tile_index(it, jt);
      float shift_x = (float)it / this->tiling[0];
      float shift_y = (float)jt / this->tiling[1];
      tiles[k] = Tile(this->shape_tile, {shift_x, shift_y});

      tiles[k].bbox = {this->bbox[0] + (float)it * lx_tile,
                       this->bbox[0] + (float)(it + 1) * lx_tile,
                       this->bbox[2] + (float)jt * ly_tile,
                       this->bbox[2] + (float)(jt + 1) * ly_tile};
    }
}

} // namespace hmap
