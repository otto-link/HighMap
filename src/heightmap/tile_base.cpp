#include "highmap/heightmap.hpp"

namespace hmap
{

Tile::Tile() : Array({0, 0}), shift({0.f, 0.f}){};

Tile::Tile(std::vector<int> shape, std::vector<float> shift)
    : Array(shape), shift(shift)
{
}

void Tile::operator=(const Array &array)
{
  this->vector = array.vector;
}

void Tile::infos() const
{
  std::cout << "Tile, ";
  std::cout << "address: " << this << ", ";
  std::cout << "shape: {" << this->shape[0] << ", " << this->shape[1] << "}, ";
  std::cout << "shift: {" << this->shift[0] << ", " << this->shift[1] << "}, ";
  std::cout << "bbox: {" << this->bbox[0] << ", " << this->bbox[1] << ", "
            << this->bbox[2] << ", " << this->bbox[3] << "}, ";
  std::cout << std::endl;
}

} // namespace hmap
