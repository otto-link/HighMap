/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#define _USE_MATH_DEFINES
#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"

namespace hmap
{

//----------------------------------------------------------------------
// Main operator
//----------------------------------------------------------------------

void stratify(Array &z, std::vector<float> hs, std::vector<float> gamma)
{
  for (uint k = 0; k < hs.size() - 1; k++)
    for (auto &v : z.vector)
      if ((v >= hs[k]) and (v < hs[k + 1]))
      {
        // scale to [0, 1], apply gamma correction and scale back
        float dh = hs[k + 1] - hs[k];
        v = (v - hs[k]) / dh;
        v = std::pow(v, gamma[k]);
        v = hs[k] + v * dh;
      }
}

void stratify_oblique(Array             &z,
                      std::vector<float> hs,
                      std::vector<float> gamma,
                      float              talus,
                      float              angle)
{
  // elevation shift due to the oblicity
  Array shift = Array(z.shape);

  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  for (int i = 0; i < z.shape.x; i++)
    for (int j = 0; j < z.shape.y; j++)
      shift(i, j) = talus * (ca * (float)i + sa * (float)j);

  Array zs = z + shift;

  // add data to the strata elevations (and gamma) to ensure they span the whole
  // range of elevation after the shift
  float zs_min = zs.min();
  float zs_max = zs.max();

  std::vector<float> hs_o = hs;
  std::vector<float> gamma_o = gamma;

  // reset elevation values
  for (auto &v : hs_o)
    v = v - hs[0] + zs_min;

  float hs_o_min = *std::min_element(hs_o.begin(), hs_o.end());
  float hs_o_max = *std::max_element(hs_o.begin(), hs_o.end());

  if (hs_o.size() > 1)
  {
    int   nstrata = hs_o.size();
    float dh = hs_o[1] - hs_o[0];
    for (int n = 0; n < std::ceil((zs_max - hs_o_max) / (hs_o_max - hs_o_min));
         n++)
      for (size_t k = 0; k < nstrata; k++)
      {
        hs_o.push_back(hs_o[k] + (hs_o_max - hs_o_min + dh) * (n + 1));
        gamma_o.push_back(gamma_o[k]);
      }
  }

  stratify(zs, hs_o, gamma_o);
  z = zs - shift;
}

//----------------------------------------------------------------------
// Overloading
//----------------------------------------------------------------------

void stratify(Array &z, std::vector<float> hs, float gamma)
{
  // float to vector
  std::vector<float> gs(hs.size() - 1);
  for (auto &v : gs)
    v = gamma;

  stratify(z, hs, gs);
}

} // namespace hmap
