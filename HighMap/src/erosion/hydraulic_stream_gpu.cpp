/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/blending.hpp"
#include "highmap/boundary.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/hydrology.hpp"
#include "highmap/kernels.hpp"
#include "highmap/math.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/range.hpp"

namespace hmap::gpu
{

void hydraulic_stream_log(Array &z,
                          float  c_erosion,
                          float  talus_ref,
                          int    deposition_ir,
                          float  deposition_scale_ratio,
                          float  gradient_power,
                          float  gradient_scaling_ratio,
                          int    gradient_prefilter_ir,
                          float  saturation_ratio,
                          Array *p_bedrock,
                          Array *p_moisture_map,
                          Array *p_erosion_map,
                          Array *p_deposition_map,
                          Array *p_flow_map)
{
  // keep a backup of the input if the erosion / deposition maps need
  // to be computed
  Array z_bckp = Array();
  if (p_erosion_map || p_deposition_map) z_bckp = z;

  // use flow accumulation to determine erosion intensity
  Array facc = flow_accumulation_dinf(z, talus_ref);
  facc = log10(facc);
  remap(facc);

  if (saturation_ratio < 1.f)
    saturate(facc, 0.f, saturation_ratio, 0.1f * saturation_ratio);

  // scale erosion with local gradient
  Array gn = gpu::gradient_norm(z);

  // TODO next operations in one local pass
  {
    gpu::smooth_cpulse(gn, gradient_prefilter_ir);
    remap(gn);
    gn = pow(gn, gradient_power);
    gn = smoothstep5_lower(gn);
    facc *= (1.f - gradient_scaling_ratio) + gradient_scaling_ratio * gn;
  }

  if (p_moisture_map)
    z -= (*p_moisture_map) * c_erosion * facc;
  else
    z -= c_erosion * facc;

  // mimic deposition
  Array zd = z;
  gpu::smooth_fill_holes(zd, deposition_ir);
  zd = gpu::blend_gradients(zd, z, deposition_ir);
  z = lerp(z, zd, deposition_scale_ratio);

  // enforce bedrock
  if (p_bedrock) z = maximum(*p_bedrock, z);

  // splatmaps
  if (p_erosion_map)
  {
    *p_erosion_map = z_bckp - z;
    clamp_min(*p_erosion_map, 0.f);
  }

  if (p_deposition_map)
  {
    *p_deposition_map = z - z_bckp;
    clamp_min(*p_deposition_map, 0.f);
  }

  if (p_flow_map) *p_flow_map = facc;
}

void hydraulic_stream_log(Array &z,
                          float  c_erosion,
                          float  talus_ref,
                          Array *p_mask,
                          int    deposition_ir,
                          float  deposition_scale_ratio,
                          float  gradient_power,
                          float  gradient_scaling_ratio,
                          int    gradient_prefilter_ir,
                          float  saturation_ratio,
                          Array *p_moisture_map,
                          Array *p_bedrock,
                          Array *p_erosion_map,
                          Array *p_deposition_map,
                          Array *p_flow_map)
{
  if (!p_mask)
    hydraulic_stream_log(z,
                         c_erosion,
                         talus_ref,
                         deposition_ir,
                         deposition_scale_ratio,
                         gradient_power,
                         gradient_scaling_ratio,
                         gradient_prefilter_ir,
                         saturation_ratio,
                         p_bedrock,
                         p_moisture_map,
                         p_erosion_map,
                         p_deposition_map,
                         p_flow_map);
  else
  {
    Array z_f = z;
    hydraulic_stream_log(z_f,
                         c_erosion,
                         talus_ref,
                         deposition_ir,
                         deposition_scale_ratio,
                         gradient_power,
                         gradient_scaling_ratio,
                         gradient_prefilter_ir,
                         saturation_ratio,
                         p_bedrock,
                         p_moisture_map,
                         p_erosion_map,
                         p_deposition_map,
                         p_flow_map);
    z = lerp(z, z_f, *(p_mask));
  }
}

} // namespace hmap::gpu
