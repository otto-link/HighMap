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

namespace hmap
{

Array accumulation_curvature(const Array &z, int ir)
{
  Array ac = z;
  if (ir > 0) smooth_cpulse(ac, ir);

  Array zx = gradient_x(ac);
  Array zy = gradient_y(ac);
  Array zxx = gradient_x(zx);
  Array zxy = gradient_y(zx);
  Array zyy = gradient_y(zy);

  Array zx2 = zx * zx;
  Array zy2 = zy * zy;
  Array p = zx2 + zy2;
  Array n = zy2 * zxx - 2.f * zxy * zx * zy + zx2 * zyy;

  Array horizontal_curvature = p * pow(p + 1.f, 0.5f);
  Array vertical_curvature = p * pow(p + 1.f, 1.5f);

  ac = n * n / (horizontal_curvature * vertical_curvature + 1e-30);

  set_borders(ac, 0.f, ir);

  return ac;
}

Array compute_curvature_h(Array &zx,
                          Array &zy,
                          Array &zxx,
                          Array &zxy,
                          Array &zyy)
{
  // mean curvature
  return (zxx * (1.f + zy * zy) - 2.f * zxy * zx * zy + zyy * (1.f + zx * zx)) *
         0.5f / pow(1.f + zx * zx + zy * zy, 1.5f);
}

Array compute_curvature_k(Array &zx,
                          Array &zy,
                          Array &zxx,
                          Array &zxy,
                          Array &zyy)
{
  // Gaussian curvature
  return (zxx * zyy - pow(zxy, 2.f)) /
         pow(1.f + pow(zx, 2.f) + pow(zy, 2.f), 2.f);
}

Array curvature_gaussian(const Array &z)
{
  Array zx = gradient_x(z);
  Array zy = gradient_y(z);
  Array zxx = gradient_x(zx);
  Array zxy = gradient_y(zx);
  Array zyy = gradient_y(zy);

  return compute_curvature_k(zx, zy, zxx, zxy, zyy);
}

Array curvature_mean(const Array &z)
{
  Array zx = gradient_x(z);
  Array zy = gradient_y(z);
  Array zxx = gradient_x(zx);
  Array zxy = gradient_y(zx);
  Array zyy = gradient_y(zy);

  return compute_curvature_h(zx, zy, zxx, zxy, zyy);
}

Array shape_index(const Array &z, int ir)
{
  Array si = z;
  if (ir > 0) smooth_cpulse(si, ir);

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
  if (ir > 0) smooth_cpulse(si, ir);

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

Array valley_width(const Array &z, int ir)
{
  Array vw = z;
  if (ir > 0) smooth_cpulse(vw, ir);

  vw = curvature_mean(-vw);
  vw = distance_transform_approx(vw);

  return vw;
}

} // namespace hmap
