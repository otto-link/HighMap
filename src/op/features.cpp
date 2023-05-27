#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array curvature_gaussian(const Array &z)
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

Array rugosity(const Array &z, int ir)
{
  hmap::Array z_avg = Array(z.shape);
  hmap::Array z_std = Array(z.shape);
  hmap::Array z_skw = Array(z.shape); // output

  z_avg = mean_local(z, ir);

  // standard deviation
  {
    hmap::Array z2 = pow(z, 2.f);
    z_std = pow(mean_local(z2, ir) - pow(z_avg, 2.f), 0.5f);
  }

  // skewness
  {
    hmap::Array z3 = pow(z, 3.f);
    z_skw = (mean_local(z3, ir) - pow(z_avg, 3.f)) / pow(z_std, 3.f);
  }

  return z_skw;
}

} // namespace hmap
