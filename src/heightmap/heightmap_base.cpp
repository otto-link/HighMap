#include "highmap/heightmap.hpp"

namespace hmap
{

HeightMap::HeightMap(std::vector<int> shape) : shape(shape)
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

int HeightMap::get_tile_index(int i, int j)
{
  return i + j * this->tiling[0];
}

void HeightMap::infos() const
{
  std::cout << "Heightmap, ";
  std::cout << "address: " << this << ", ";
  std::cout << "shape: {" << this->shape[0] << ", " << this->shape[1] << "}, ";
  std::cout << "tiling: {" << this->tiling[0] << ", " << this->tiling[1]
            << "}, ";
  std::cout << std::endl;

  for (auto &t : tiles)
    t.infos();
}

Array HeightMap::to_array()
{
  Array array = Array(shape);

  for (int i = 0; i < tiling[0]; i++)
    for (int j = 0; j < tiling[1]; j++)
    {
      // tile array position within the global array
      int i1 = i * this->shape_tile[0];
      int j1 = j * this->shape_tile[1];
      int k = this->get_tile_index(i, j);

      for (int p = 0; p < this->shape_tile[0]; p++)
        for (int q = 0; q < this->shape_tile[1]; q++)
          array(p + i1, q + j1) = tiles[k](p, q);
    }

  return array;
}

void HeightMap::update_tile_parameters()
{
  tiles.resize(this->tiling[0] * this->tiling[1]);

  // tile shapes
  this->shape_tile = {shape[0] / tiling[0], shape[1] / tiling[1]};

  for (int i = 0; i < tiling[0]; i++)
    for (int j = 0; j < tiling[1]; j++)
    {
      int   k = this->get_tile_index(i, j);
      float shift_x = (float)i * this->shape_tile[0];
      float shift_y = (float)j * this->shape_tile[1];
      tiles[k] = Tile(this->shape_tile, {shift_x, shift_y});
    }
}

} // namespace hmap
