/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/boundary.hpp"
#include "highmap/features.hpp"
#include "highmap/filters.hpp"
#include "highmap/range.hpp"

namespace hmap::gpu
{

Array accumulation_curvature(const Array &z, int ir)
{
  Array ac = z;
  if (ir > 0) gpu::smooth_cpulse(ac, ir);

  // compute curvature criteria
  Array p, q, r, s, t;
  compute_curvature_gradients(ac, p, q, r, s, t);

  Array k = compute_curvature_k(p, q, r, s, t);
  Array h = compute_curvature_h(r, t);

  ac = h * h - k * k;

  set_borders(ac, 0.f, ir);

  return ac;
}

Array shape_index(const Array &z, int ir)
{
  Array si = z;
  if (ir > 0) gpu::smooth_cpulse(si, ir);

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
  if (ir > 0) gpu::smooth_cpulse(si, ir);

  // compute curvature criteria
  Array p, q, r, s, t;
  compute_curvature_gradients(si, p, q, r, s, t);

  Array k = compute_curvature_k(p, q, r, s, t);
  Array h = compute_curvature_h(r, t);

  Array d = h * h - k;

  clamp_min(d, 0.f);
  d = pow(d, 0.5f);

  set_borders(d, 0.f, ir);

  return si;
}

} // namespace hmap::gpu
