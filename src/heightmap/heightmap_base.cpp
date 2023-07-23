#include "highmap/heightmap.hpp"

namespace hmap
{

HeightMap::HeightMap(std::vector<int> shape) : shape(shape)
{
  this->array.set_shape(shape);
}

HeightMap::HeightMap(std::vector<int> shape, std::vector<float> bbox)
    : shape(shape), bbox(bbox)
{
  this->array.set_shape(shape);
}

} // namespace hmap
