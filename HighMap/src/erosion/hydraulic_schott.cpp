/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>
#include <random>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/erosion.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/math.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

#include "highmap/internal/vector_utils.hpp"

namespace hmap
{

float get_slope(const Array &z, int i, int j, int p, int q, float dist)
{
  return (z(i, j) - z(p, q)) / dist;
}

float helper_find_steepest_downslope_neighbor(const Array              &z,
                                              int                       i,
                                              int                       j,
                                              int                      &is,
                                              int                      &js,
                                              const std::vector<int>   &di,
                                              const std::vector<int>   &dj,
                                              const std::vector<float> &c)
{
  float slope_max = 0.f;

  for (uint k = 0; k < di.size(); k++)
  {
    int p = i + di[k];
    int q = j + dj[k];

    if (z(i, j) >= z(p, q))
    {
      float slope = get_slope(z, i, j, p, q, c[k]);

      if (slope > slope_max)
      {
        slope_max = slope;
        is = p;
        js = q;
      }
    }
  }

  return slope_max;
}

float helper_get_flow_weight(const Array              &z,
                             int                       i,
                             int                       j,
                             int                       u,
                             int                       v,
                             const std::vector<int>   &di,
                             const std::vector<int>   &dj,
                             const std::vector<float> &c)
{
  float weight = 0.f;
  float total_weight = 0.f;

  for (uint k = 0; k < di.size(); k++)
  {
    int p = i + di[k];
    int q = j + dj[k];

    if (z(i, j) > z(p, q))
    {
      float slope = get_slope(z, i, j, p, q, c[k]);
      float s = std::pow(slope, 1.3f);
      total_weight += s;

      if (p == u && q == v) weight = s;
    }
  }

  if (total_weight == 0.f)
    return 0.f;
  else
    return weight / total_weight;
}

float helper_get_flow(const Array              &z,
                      const Array              &flow,
                      int                       i,
                      int                       j,
                      const std::vector<int>   &di,
                      const std::vector<int>   &dj,
                      const std::vector<float> &c)
{
  float flow_out = 0.f;

  for (uint k = 0; k < di.size(); k++)
  {
    int p = i + di[k];
    int q = j + dj[k];

    if (z(i, j) <= z(p, q))
    {
      float weight = helper_get_flow_weight(z, p, q, i, j, di, dj, c);
      flow_out += flow(p, q) * weight;
    }
  }

  return flow_out;
}

void hydraulic_schott(Array &z,
                      int    iterations,
                      float  deposition_iterations_ratio,
                      float  c_erosion,
                      float  c_deposition,
                      Array *p_softness,
                      Array *p_flow)
{
  // https://www.shadertoy.com/view/XX2XWD

  std::vector<int>   di = DI;
  std::vector<int>   dj = DJ;
  std::vector<float> c = CD;

  Array softness = p_softness ? *p_softness : Array(z.shape, 1.f);
  Array flow = p_flow ? *p_flow : Array(z.shape, 1.f);

  Array deposition(z.shape, 0.f);
  Array sediment(z.shape, 0.f);

  float c_erosion_scaled = c_erosion * (float)z.shape.x / 256.f;

  int deposition_iterations = (int)(deposition_iterations_ratio * iterations);

  // erosion
  for (int it = 0; it < iterations; it++)
  {
    Array z_new = z;
    Array flow_new = flow;
    Array deposition_new = deposition;

    for (int j = 2; j < z.shape.y - 2; j++)
      for (int i = 2; i < z.shape.x - 2; i++)
      {
        int is = i;
        int js = j;

        float slope_max =
            helper_find_steepest_downslope_neighbor(z, i, j, is, js, di, dj, c);

        float z_steepest = z(is, js);

        float spe = c_erosion_scaled *
                    std::min(10.f,
                             std::pow(flow(i, j), 0.8f) * slope_max *
                                 slope_max) *
                    softness(i, j);

        z_new(i, j) = std::max(z_steepest, z(i, j) - spe);

        deposition_new(i, j) = std::max(0.f,
                                        deposition(i, j) -
                                            (z(i, j) - z_new(i, j)));
      }

    // flow map update
    for (int j = 2; j < z.shape.y - 2; j++)
      for (int i = 2; i < z.shape.x - 2; i++)
        flow_new(i, j) = 1.f + helper_get_flow(z, flow, i, j, di, dj, c);

    z = z_new;
    flow = flow_new;
    deposition = deposition_new;

    fill_borders(z, 2);
    fill_borders(flow, 2);
    fill_borders(deposition, 2);
  }

  // deposition
  for (int it = 0; it < deposition_iterations; it++)
  {
    Array z_new = z;
    Array flow_new = flow;
    Array deposition_new = deposition;
    Array sediment_new = sediment;

    for (int j = 2; j < z.shape.y - 2; j++)
      for (int i = 2; i < z.shape.x - 2; i++)
      {
        int is = i;
        int js = j;

        float slope_max =
            helper_find_steepest_downslope_neighbor(z, i, j, is, js, di, dj, c);

        float spe = std::pow(flow(i, j), 0.8f) * slope_max * slope_max;

        float new_sed = helper_get_flow(z, sediment, i, j, di, dj, c);
        float depo_index = std::max(0.f, new_sed - 0.7f * spe);
        float depo = std::min(new_sed, 0.01f * depo_index);

        z_new(i, j) += depo;
        sediment_new(i, j) = new_sed + c_deposition * spe - depo;
        deposition_new(i, j) += depo;
      }

    // flow map update
    for (int j = 2; j < z.shape.y - 2; j++)
      for (int i = 2; i < z.shape.x - 2; i++)
        flow_new(i, j) = 1.f + helper_get_flow(z, flow, i, j, di, dj, c);

    z = z_new;
    flow = flow_new;
    deposition = deposition_new;
    sediment = sediment_new;

    fill_borders(z, 2);
    fill_borders(flow, 2);
    fill_borders(deposition, 2);
    fill_borders(sediment, 2);
  }

  // clean-up and exit
  extrapolate_borders(z, 2);
  extrapolate_borders(flow, 3);
  flow = maximum(flow, 0.f);

  if (p_flow) *p_flow = flow;
}

void hydraulic_schott(Array &z,
                      Array *p_mask,
                      int    iterations,
                      float  deposition_iterations_ratio,
                      float  c_erosion,
                      float  c_deposition,
                      Array *p_softness,
                      Array *p_flow)
{
  if (!p_mask)
    hydraulic_schott(z,
                     iterations,
                     deposition_iterations_ratio,
                     c_erosion,
                     c_deposition,
                     p_softness,
                     p_flow);
  else
  {
    Array zf = z;
    hydraulic_schott(zf,
                     iterations,
                     deposition_iterations_ratio,
                     c_erosion,
                     c_deposition,
                     p_softness,
                     p_flow);
    z = lerp(z, zf, *(p_mask));
  }
}

} // namespace hmap
