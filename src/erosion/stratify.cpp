/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "highmap/array.hpp"

namespace hmap
{

//----------------------------------------------------------------------
// Main operator
//----------------------------------------------------------------------

void stratify(Array &z, std::vector<float> hs, std::vector<float> gamma)
{
  for (uint k = 0; k < hs.size() - 1; k++)
  {
    for (auto &v : z.vector)
    {
      if ((v >= hs[k]) and (v < hs[k + 1]))
      {
        // scale to [0, 1], apply gamma correction and scale back
        float dh = hs[k + 1] - hs[k];
        v = (v - hs[k]) / dh;
        v = std::pow(v, gamma[k]);
        v = hs[k] + v * dh;
      }
    }
  }
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