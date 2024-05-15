/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <algorithm>
#include <map>

#include "dkm.hpp"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/gradient.hpp"
#include "highmap/math.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

// helpers

Array compute_h(Array &zx, Array &zy, Array &zxx, Array &zxy, Array &zyy)
{
  return (zxx * (1.f + zy * zy) - 2.f * zxy * zx * zy + zyy * (1.f + zx * zx)) *
         0.5f / pow(1.f + zx * zx + zy * zy, 1.5f);
}

Array compute_k(Array &zx, Array &zy, Array &zxx, Array &zxy, Array &zyy)
{
  return (zxx * zyy - pow(zxy, 2.f)) /
         pow(1.f + pow(zx, 2.f) + pow(zy, 2.f), 2.f);
}

// functions

Array curvature_gaussian(const Array &z)
{
  Array k = Array(z.shape); // output
  Array zx = gradient_x(z);
  Array zy = gradient_y(z);
  Array zxx = gradient_x(zx);
  Array zxy = gradient_y(zx);
  Array zyy = gradient_y(zy);

  return compute_k(zx, zy, zxx, zxy, zyy);
}

Array curvature_mean(const Array &z)
{
  Array h = Array(z.shape); // output
  Array zx = gradient_x(z);
  Array zy = gradient_y(z);
  Array zxx = gradient_x(zx);
  Array zxy = gradient_y(zx);
  Array zyy = gradient_y(zy);

  return compute_h(zx, zy, zxx, zxy, zyy);
}

Array shape_index(const Array &z, int ir)
{
  Array si = z;
  if (ir > 0)
    smooth_cpulse(si, ir);

  // compute curvature criteria
  Array zx = gradient_x(si);
  Array zy = gradient_y(si);
  Array zxx = gradient_x(zx);
  Array zxy = gradient_y(zx);
  Array zyy = gradient_y(zy);

  Array k = compute_k(zx, zy, zxx, zxy, zyy); // gaussian
  Array h = compute_h(zx, zy, zxx, zxy, zyy); // mean

  Array d = pow(h * h - k, 0.5f);
  si = 2.f / M_PI * atan(h / (d + 1e-30));
  si *= 0.5f;
  si += 0.5f;

  return si;
}

Array unsphericity(const Array &z, int ir)
{
  Array si = z;
  if (ir > 0)
    smooth_cpulse(si, ir);

  // compute curvature criteria
  Array zx = gradient_x(si);
  Array zy = gradient_y(si);
  Array zxx = gradient_x(zx);
  Array zxy = gradient_y(zx);
  Array zyy = gradient_y(zy);

  Array k = compute_k(zx, zy, zxx, zxy, zyy); // gaussian
  Array h = compute_h(zx, zy, zxx, zxy, zyy); // mean

  return pow(h * h - k, 0.5f);
}

Array valley_width(const Array &z, int ir)
{
  Array vw = z;
  if (ir > 0)
    smooth_cpulse(vw, ir);

  vw = curvature_mean(-vw);
  vw = distance_transform(vw);
  return vw;
}

} // namespace hmap
