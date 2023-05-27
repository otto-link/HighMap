#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

void warp(Array &array, const Array &dx, const Array &dy)
{
  int i1 = std::max(0, -(int)dx.min());
  int i2 = std::max(0, (int)dx.max());
  int j1 = std::max(0, -(int)dy.min());
  int j2 = std::max(0, (int)dy.max());

  Array array_buffered = generate_buffered_array(array, {i1, i2, j1, j2});

  for (int i = 0; i < array.shape[0]; i++)
  {
    for (int j = 0; j < array.shape[1]; j++)
    {
      // warped position
      float x = (float)i + dx(i, j);
      float y = (float)j + dy(i, j);

      // nearest grid point
      int ip = (int)x + i1;
      int jp = (int)y + j1;

      array(i, j) = array_buffered(ip, jp);
    }
  }
}

void warp_fbm(Array &array, float scale, std::vector<float> kw, uint seed)
{
  Array dx = fbm_perlin(array.shape, kw, seed);
  Array dy = fbm_perlin(array.shape, kw, seed++);
  remap(dx, -scale, scale);
  remap(dy, -scale, scale);
  warp(array, dx, dy);
}

} // namespace hmap
