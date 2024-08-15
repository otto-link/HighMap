/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/convolve.hpp"
#include "highmap/filters.hpp"
#include "highmap/hydrology.hpp"
#include "highmap/kernels.hpp"
#include "highmap/math.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

namespace hmap
{

void hydraulic_stream(Array &z,
                      float  c_erosion,
                      float  talus_ref,
                      Array *p_bedrock,
                      Array *p_moisture_map,
                      Array *p_erosion_map,
                      int    ir,
                      float  clipping_ratio)
{
  // keep a backup of the input if the erosion / deposition maps need
  // to be computed
  Array z_bckp = Array();
  if (p_erosion_map)
    z_bckp = z;

  // use flow accumulation to determine erosion intensity
  Array facc = flow_accumulation_dinf(z, talus_ref);

  // clip large flow accumulation values using a value loosely based
  // on the standard deviation (of an equivalent symetric
  // distribution)
  float vmax = clipping_ratio * std::pow(facc.sum() / (float)facc.size(), 0.5f);
  clamp(facc, 0.f, vmax);
  remap(facc);

  if (ir > 1)
  {
    Array kernel = cone({ir, ir});
    kernel.normalize();
    facc = convolve2d_svd(facc, kernel);
  }

  if (p_moisture_map)
    z -= (*p_moisture_map) * c_erosion * facc;
  else
    z -= c_erosion * facc;

  if (p_bedrock)
    z = maximum(*p_bedrock, z);

  // splatmaps
  if (p_erosion_map)
  {
    *p_erosion_map = z_bckp - z;
    clamp_min(*p_erosion_map, 0.f);
  }
}

void hydraulic_stream(Array &z,
                      Array *p_mask,
                      float  c_erosion,
                      float  talus_ref,
                      Array *p_moisture_map,
                      Array *p_bedrock,
                      Array *p_erosion_map,
                      int    ir,
                      float  clipping_ratio)
{
  if (!p_mask)
    hydraulic_stream(z,
                     c_erosion,
                     talus_ref,
                     p_bedrock,
                     p_moisture_map,
                     p_erosion_map,
                     ir,
                     clipping_ratio);
  else
  {
    Array z_f = z;
    hydraulic_stream(z_f,
                     c_erosion,
                     talus_ref,
                     p_bedrock,
                     p_moisture_map,
                     p_erosion_map,
                     ir,
                     clipping_ratio);
    z = lerp(z, z_f, *(p_mask));
  }
}

void hydraulic_stream_log(Array &z,
                          float  c_erosion,
                          float  talus_ref,
                          float  gamma,
                          Array *p_bedrock,
                          Array *p_moisture_map,
                          Array *p_erosion_map,
                          int    ir)
{
  // keep a backup of the input if the erosion / deposition maps need
  // to be computed
  Array z_bckp = Array();
  if (p_erosion_map)
    z_bckp = z;

  // use flow accumulation to determine erosion intensity
  Array facc = flow_accumulation_dinf(z, talus_ref);
  facc = log10(facc);
  facc.infos();
  remap(facc);
  gamma_correction(facc, gamma);

  if (ir > 1)
  {
    Array kernel = cone({ir, ir});
    kernel.normalize();
    facc = convolve2d_svd(facc, kernel);
  }

  if (p_moisture_map)
    z -= (*p_moisture_map) * c_erosion * facc;
  else
    z -= c_erosion * facc;

  if (p_bedrock)
    z = maximum(*p_bedrock, z);

  // splatmaps
  if (p_erosion_map)
  {
    *p_erosion_map = z_bckp - z;
    clamp_min(*p_erosion_map, 0.f);
  }
}

void hydraulic_stream_log(Array &z,
                          Array *p_mask,
                          float  c_erosion,
                          float  talus_ref,
                          float  gamma,
                          Array *p_moisture_map,
                          Array *p_bedrock,
                          Array *p_erosion_map,
                          int    ir)
{
  if (!p_mask)
    hydraulic_stream_log(z,
                         c_erosion,
                         talus_ref,
                         gamma,
                         p_bedrock,
                         p_moisture_map,
                         p_erosion_map,
                         ir);
  else
  {
    Array z_f = z;
    hydraulic_stream_log(z_f,
                         c_erosion,
                         talus_ref,
                         gamma,
                         p_bedrock,
                         p_moisture_map,
                         p_erosion_map,
                         ir);
    z = lerp(z, z_f, *(p_mask));
  }
}

} // namespace hmap
