#include <algorithm>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

void hydraulic_diffusion(Array &z,
                         float  c_diffusion,
                         float  talus,
                         int    iterations)
{
  Array dx = Array(z.shape);
  Array dy = Array(z.shape);
  Array qx = Array(z.shape);
  Array qy = Array(z.shape);
  Array c = Array(z.shape);

  for (int it = 0; it < iterations; it++)
  {
    gradient_x(z, dx);
    gradient_y(z, dy);

    c = 1.f / (1.f - dx * dx / (talus * talus));
    qx = c_diffusion * c * dx;

    c = 1.f / (1.f - dy * dy / (talus * talus));
    qy = c_diffusion * c * dy;

    gradient_x(qx, dx);
    gradient_y(qy, dy);

    z += dx + dy;
  }
}

} // namespace hmap
