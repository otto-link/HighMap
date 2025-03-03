/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/boundary.hpp"
#include "highmap/features.hpp"
#include "highmap/filters.hpp"
#include "highmap/range.hpp"

namespace hmap
{

Array accumulation_curvature(const Array &z, int ir)
{
  // taken from Florinsky, I. (2016). Digital terrain analysis in soil
  // science and geology. Academic Press.

  Array ac = z;
  if (ir > 0) smooth_cpulse(ac, ir);

  // compute curvature criteria
  Array p, q, r, s, t;
  compute_curvature_gradients(ac, p, q, r, s, t);

  Array k = compute_curvature_k(p, q, r, s, t);
  Array h = compute_curvature_h(r, t);

  ac = h * h - k * k;

  set_borders(ac, 0.f, ir);

  return ac;
}

void compute_curvature_gradients(const Array &z,
                                 Array       &p,
                                 Array       &q,
                                 Array       &r,
                                 Array       &s,
                                 Array       &t)
{
  p = Array(z.shape);
  q = Array(z.shape);
  r = Array(z.shape);
  s = Array(z.shape);
  t = Array(z.shape);

  for (int j = 1; j < z.shape.y - 1; ++j)
    for (int i = 1; i < z.shape.x - 1; ++i)
    {
      p(i, j) = 0.5f * (z(i + 1, j) - z(i - 1, j));        // dz/dx
      q(i, j) = 0.5f * (z(i, j + 1) - z(i, j - 1));        // dz/dy
      r(i, j) = z(i + 1, j) - 2.f * z(i, j) + z(i - 1, j); // d2z/dx2
      s(i, j) = 0.25f * (z(i - 1, j - 1) - z(i - 1, j + 1) - z(i + 1, j - 1) +
                         z(i + 1, j + 1));                 // d2z/dxdy
      t(i, j) = z(i, j + 1) - 2.f * z(i, j) + z(i, j - 1); // d2z/dy2
    }
}

Array compute_curvature_h(const Array &r, const Array &t)
{
  return -0.5f * (r + t);
}

Array compute_curvature_k(const Array &p,
                          const Array &q,
                          const Array &r,
                          const Array &s,
                          const Array &t)
{
  return (r * t - s * s) / pow(1.f + p * p + q * q, 2.f);
}

Array curvature_gaussian(const Array &z)
{
  Array p, q, r, s, t;
  compute_curvature_gradients(z, p, q, r, s, t);
  return compute_curvature_k(p, q, r, s, t);
}

Array curvature_mean(const Array &z)
{
  Array p, q, r, s, t;
  compute_curvature_gradients(z, p, q, r, s, t);
  return compute_curvature_h(r, t);
}

Array shape_index(const Array &z, int ir)
{
  Array si = z;
  if (ir > 0) smooth_cpulse(si, ir);

  // compute curvature criteria
  Array p, q, r, s, t;
  compute_curvature_gradients(si, p, q, r, s, t);

  Array k = compute_curvature_k(p, q, r, s, t);
  Array h = compute_curvature_h(r, t);

  Array d = h * h - k;
  clamp_min(d, 0.f);
  d = pow(d, 0.5f);

  si = 2.f / M_PI * atan(h / (d + 1e-30));
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
  Array p, q, r, s, t;
  compute_curvature_gradients(si, p, q, r, s, t);

  Array k = compute_curvature_k(p, q, r, s, t);
  Array h = compute_curvature_h(r, t);

  Array d = h * h - k;

  clamp_min(d, 0.f);
  d = pow(d, 0.5f);

  set_borders(d, 0.f, ir);

  return d;
}

} // namespace hmap
