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
  Array c = z;
  if (ir > 0) gpu::smooth_cpulse(c, ir);

  c = hmap::accumulation_curvature(c, 0);

  set_borders(c, 0.f, ir);
  return c;
}

Array curvature_horizontal_cross_sectional(const Array &z, int ir)
{
  Array c = z;
  if (ir > 0) gpu::smooth_cpulse(c, ir);

  c = hmap::curvature_horizontal_cross_sectional(c, 0);

  set_borders(c, 0.f, ir);
  return c;
}

Array curvature_horizontal_plan(const Array &z, int ir)
{
  Array c = z;
  if (ir > 0) gpu::smooth_cpulse(c, ir);

  c = hmap::curvature_horizontal_plan(c, 0);

  set_borders(c, 0.f, ir);
  return c;
}

Array curvature_horizontal_tangential(const Array &z, int ir)
{
  Array c = z;
  if (ir > 0) gpu::smooth_cpulse(c, ir);

  c = hmap::curvature_horizontal_tangential(c, 0);

  set_borders(c, 0.f, ir);
  return c;
}

Array curvature_ring(const Array &z, int ir)
{
  Array c = z;
  if (ir > 0) gpu::smooth_cpulse(c, ir);

  c = hmap::curvature_ring(c, 0);

  set_borders(c, 0.f, ir);
  return c;
}

Array curvature_rotor(const Array &z, int ir)
{
  Array c = z;
  if (ir > 0) gpu::smooth_cpulse(c, ir);

  c = hmap::curvature_rotor(c, 0);

  set_borders(c, 0.f, ir);
  return c;
}

Array curvature_vertical_longitudinal(const Array &z, int ir)
{
  Array c = z;
  if (ir > 0) gpu::smooth_cpulse(c, ir);

  c = hmap::curvature_vertical_longitudinal(c, 0);

  set_borders(c, 0.f, ir);
  return c;
}

Array curvature_vertical_profile(const Array &z, int ir)
{
  Array c = z;
  if (ir > 0) gpu::smooth_cpulse(c, ir);

  c = hmap::curvature_vertical_profile(c, 0);

  set_borders(c, 0.f, ir);
  return c;
}

Array shape_index(const Array &z, int ir)
{
  Array c = z;
  if (ir > 0) gpu::smooth_cpulse(c, ir);

  c = hmap::shape_index(c, 0);

  set_borders(c, 0.f, ir);
  return c;
}

Array unsphericity(const Array &z, int ir)
{
  Array c = z;
  if (ir > 0) gpu::smooth_cpulse(c, ir);

  c = hmap::unsphericity(c, 0);

  set_borders(c, 0.f, ir);
  return c;
}

} // namespace hmap::gpu
