/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/math.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

void hydraulic_schott(Array       &z,
                      int          iterations,
                      const Array &talus,
                      float        c_erosion,
                      float        c_thermal,
                      float        c_deposition,
                      float        flow_acc_exponent,
                      float        flow_acc_exponent_depo,
                      float        flow_routing_exponent,
                      float        thermal_weight,
                      float        deposition_weight,
                      Array       *p_flow)
{
  Array flow = p_flow ? *p_flow : Array(z.shape, 1.f);
  Array sediment(z.shape, 0.f);

  // erosion weight is always 1
  float sum_weight = 1.f + thermal_weight + deposition_weight;

  int erosion_it = (int)(10.f / sum_weight);
  int thermal_it = erosion_it + (int)(10.f * thermal_weight / sum_weight);

  auto run = clwrapper::Run("hydraulic_schott");

  Vec2<int> shape = z.shape;

  run.bind_imagef("z", z.vector, shape.x, shape.y);
  run.bind_imagef("flow", flow.vector, shape.x, shape.y);
  run.bind_imagef("sediment", sediment.vector, shape.x, shape.y);
  run.bind_imagef("talus",
                  const_cast<std::vector<float> &>(talus.vector),
                  shape.x,
                  shape.y);

  run.bind_imagef("z_new", z.vector, shape.x, shape.y, true);
  run.bind_imagef("flow_new", flow.vector, shape.x, shape.y, true);
  run.bind_imagef("sediment_new", sediment.vector, shape.x, shape.y, true);

  run.bind_arguments(shape.x,
                     shape.y,
                     c_erosion,
                     c_thermal,
                     c_deposition,
                     flow_acc_exponent,
                     flow_acc_exponent_depo,
                     flow_routing_exponent,
                     erosion_it,
                     thermal_it,
                     0);

  for (int it = 0; it < iterations; it++)
  {
    run.set_argument(17, it);
    run.execute({shape.x, shape.y});

    // z <- z_new
    run.read_imagef("z_new");
    run.read_imagef("flow_new");
    run.read_imagef("sediment_new");

    // to device
    run.write_imagef("z");
    run.write_imagef("flow");
    run.write_imagef("sediment");
  }

  if (p_flow) *p_flow = flow;
}

void hydraulic_schott(Array       &z,
                      int          iterations,
                      const Array &talus,
                      Array       *p_mask,
                      float        c_erosion,
                      float        c_thermal,
                      float        c_deposition,
                      float        flow_acc_exponent,
                      float        flow_acc_exponent_depo,
                      float        flow_routing_exponent,
                      float        thermal_weight,
                      float        deposition_weight,
                      Array       *p_flow)
{
  if (!p_mask)
    hydraulic_schott(z,
                     iterations,
                     talus,
                     c_erosion,
                     c_thermal,
                     c_deposition,
                     flow_acc_exponent,
                     flow_acc_exponent_depo,
                     flow_routing_exponent,
                     thermal_weight,
                     deposition_weight,
                     p_flow);
  else
  {
    Array z_f = z;
    hydraulic_schott(z_f,
                     iterations,
                     talus,
                     c_erosion,
                     c_thermal,
                     c_deposition,
                     flow_acc_exponent,
                     flow_acc_exponent_depo,
                     flow_routing_exponent,
                     thermal_weight,
                     deposition_weight,
                     p_flow);
    z = lerp(z, z_f, *(p_mask));
  }
}

} // namespace hmap::gpu
