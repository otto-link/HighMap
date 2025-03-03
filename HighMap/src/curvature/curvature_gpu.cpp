/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/boundary.hpp"
#include "highmap/curvature.hpp"
#include "highmap/filters.hpp"
#include "highmap/range.hpp"

namespace hmap::gpu
{

Array accumulation_curvature(const Array &z, int ir)
{
  Array ac = z;
  if (ir > 0) gpu::smooth_cpulse(ac, ir);

  ac = hmap::accumulation_curvature(ac, 0);

  set_borders(ac, 0.f, ir);
  return ac;
}

Array shape_index(const Array &z, int ir)
{
  Array si = z;
  if (ir > 0) gpu::smooth_cpulse(si, ir);

  si = hmap::shape_index(si, 0);

  set_borders(si, 0.f, ir);
  return si;
}

Array unsphericity(const Array &z, int ir)
{
  Array si = z;
  if (ir > 0) gpu::smooth_cpulse(si, ir);

  si = hmap::unsphericity(si, 0);

  set_borders(si, 0.f, ir);
  return si;
}

} // namespace hmap::gpu
