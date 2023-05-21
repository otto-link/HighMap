#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap
{

void warp(Array &array, Array &dx, Array &dy)
{
  int i1 = std::max(0, -(int)dx.min());
  int i2 = std::max(0, (int)dx.max());
  int j1 = std::max(0, -(int)dy.min());
  int j2 = std::max(0, (int)dy.max());

  Array array_buffered = generate_buffered_array(array, {i1, i2, j1, j2});

  const int nbi = array_buffered.shape[0];
  const int nbj = array_buffered.shape[1];

  for (int i = 0; i < array.shape[0]; i++)
  {
    for (int j = 0; j < array.shape[1]; j++)
    {
      // warped position
      float x = (float)i + dx(i, j);
      float y = (float)j + dy(i, j);

      // nearest grid point (periodic domain)
      int ip = ((int)x + i1) % nbi;
      int jp = ((int)y + j1) % nbj;

      array(i, j) = array_buffered(ip, jp);
    }
  }
}

} // namespace hmap
