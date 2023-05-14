#include "highmap/array.hpp"
#include "highmap/math.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array curvature_gaussian(Array &z)
{
  Array k = Array(z.shape); // output
  Array zx = gradient_x(z);
  Array zy = gradient_y(z);
  Array zxx = gradient_x(zx);
  Array zxy = gradient_y(zx);
  Array zyy = gradient_y(zy);

  k = (zxx * zyy - pow(zxy, 2.f)) / pow(1.f + pow(zx, 2.f) + pow(zy, 2.f), 2.f);
  return k;
}

} // namespace hmap
