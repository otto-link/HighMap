/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <algorithm>

#include "highmap/array.hpp"
#include "highmap/op.hpp"

#include "macrologger.h"

namespace hmap
{

void hydraulic_algebric(Array &z,
                        float  talus_ref,
                        int    ir,
                        float  c_erosion,
                        float  c_deposition,
                        int    iterations)
{
  Array talus = Array(z.shape);
  Array zf = Array(z.shape);

  for (int it = 0; it < iterations; it++)
  {
    if (ir > 0)
    {
      // pre-filter the highmap to avoid "kinky" artifacts
      zf = z;
      smooth_cpulse(zf, ir);
      gradient_talus(zf, talus);
    }
    else
      gradient_talus(z, talus);

    for (int i = 0; i < z.shape[0]; i++)
      for (int j = 0; j < z.shape[1]; j++)
      {
        if (talus(i, j) > talus_ref)
          z(i, j) -= c_erosion * (talus(i, j) / talus_ref - 1.f);
        else
          z(i, j) += c_deposition * (1.f - talus(i, j) / talus_ref);
      }
  }
  // z = talus;
}

} // namespace hmap
