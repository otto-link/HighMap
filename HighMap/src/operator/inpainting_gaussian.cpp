/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"

namespace hmap
{

Array inpainting_diffusion(const Array &array,
                           const Array &mask,
                           int          iterations)
{
  // M. M. Oliveira, B. Bowen, R. McKenna, Y.-S. Chang: Fast Digital
  // Image Inpainting, Proc. of Int. Conf. on Visualization, Imaging
  // and Image Processing (VIIP), pp. 261-266, 2001.

  const float a = 0.073235f;
  const float b = 0.176765f;

  Array out = Array(array.shape);

  // store indices of the cells to be filled
  std::vector<int> idx = {};
  idx.reserve(array.size());

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      if (mask(i, j) == 0.f)
        out(i, j) = array(i, j);
      else
        idx.push_back(array.linear_index(i, j));
    }

  // diffusion process
  for (int it = 0; it < iterations; it++)
  {
    for (size_t k = 0; k < idx.size(); k++)
    {
      Vec2<int> ij = array.linear_index_reverse(idx[k]);

      out(ij.x, ij.y) = a * (out(ij.x - 1, ij.y - 1) + out(ij.x + 1, ij.y - 1) +
                             out(ij.x - 1, ij.y + 1) +
                             out(ij.x + 1, ij.y + 1)) +
                        b * (out(ij.x, ij.y - 1) + out(ij.x - 1, ij.y) +
                             out(ij.x, ij.y + 1) + out(ij.x + 1, ij.y));
    }
    fill_borders(out);
  }

  extrapolate_borders(out);

  return out;
}

} // namespace hmap
