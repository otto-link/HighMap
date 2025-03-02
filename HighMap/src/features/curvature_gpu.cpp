/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/features.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/math.hpp"
#include "highmap/morphology.hpp"
#include "highmap/range.hpp"

namespace hmap::gpu
{

Array accumulation_curvature(const Array &z, int ir)
{
  // taken from Florinsky, I. (2016). Digital terrain analysis in soil
  // science and geology. Academic Press.

  Array ac = z;
  if (ir > 0) gpu::smooth_cpulse(ac, ir);

  // compute curvature criteria
  Array zx = gradient_x(ac);
  Array zy = gradient_y(ac);
  Array zxx = gradient_x(zx);
  Array zxy = gradient_y(zx);
  Array zyy = gradient_y(zy);

  Array k = compute_curvature_k(zx, zy, zxx, zxy, zyy);
  Array h = compute_curvature_h(zx, zy, zxx, zxy, zyy);

  ac = h * h - k * k;

  set_borders(ac, 0.f, ir);

  return ac;
}

Array shape_index(const Array &z, int ir)
{
  Array si = z;
  if (ir > 0) gpu::smooth_cpulse(si, ir);

  // compute curvature criteria
  Array zx = gradient_x(si);
  Array zy = gradient_y(si);
  Array zxx = gradient_x(zx);
  Array zxy = gradient_y(zx);
  Array zyy = gradient_y(zy);

  Array k = compute_curvature_k(zx, zy, zxx, zxy, zyy);
  Array h = compute_curvature_h(zx, zy, zxx, zxy, zyy);

  Array d = h * h - k;
  clamp_min(d, 0.f);
  d = pow(d, 0.5f);

  si = -2.f / M_PI * atan(h / (d + 1e-30));
  si *= 0.5f;
  si += 0.5f;

  set_borders(si, 0.f, ir);

  return si;
}

Array unsphericity(const Array &z, int ir)
{
  Array si = z;
  if (ir > 0) gpu::smooth_cpulse(si, ir);

  // compute curvature criteria
  Array zx = gradient_x(si);
  Array zy = gradient_y(si);
  Array zxx = gradient_x(zx);
  Array zxy = gradient_y(zx);
  Array zyy = gradient_y(zy);

  Array k = compute_curvature_k(zx, zy, zxx, zxy, zyy);
  Array h = compute_curvature_h(zx, zy, zxx, zxy, zyy);

  Array d = h * h - k;

  clamp_min(d, 0.f);
  d = pow(d, 0.5f);

  set_borders(d, 0.f, ir);

  return d;
}

} // namespace hmap::gpu
