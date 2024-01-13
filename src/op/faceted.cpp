/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/interpolate.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array faceted(const Array &array,
              int          neighborhood,
              Array       *p_noise_x,
              Array       *p_noise_y)
{
  // find sinks and preaks to use their positions and elevations as
  // reference points for the Delauney interpolation
  std::vector<float> x;
  std::vector<float> y;
  std::vector<float> value;

  std::vector<int> di, dj;

  switch (neighborhood)
  {
  case neighborhood::moore:
    di = {-1, -1, 0, 1, 1, 1, 0, -1};
    dj = {0, 1, 1, 1, 0, -1, -1, -1};
    break;

  case neighborhood::von_neumann:
    di = {-1, 0, 1, 0};
    dj = {0, 1, 0, -1};
    break;

  case neighborhood::cross:
    di = {-1, 1, 1, -1};
    dj = {1, 1, -1, -1};
  }

  const uint nb = di.size();

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      std::vector<int> signature(nb);

      for (size_t k = 0; k < nb; k++)
      {
        int ip = i + di[k];
        int jp = j + dj[k];

        if ((ip > -1) && (ip < array.shape.x) && (jp > -1) &&
            (jp < array.shape.y))
        {
          if (array(ip, jp) > array(i, j))
            signature[k] = 1;
          if (array(ip, jp) < array(i, j))
            signature[k] = -1;
        }
      }

      bool store_xyz = false;
      bool all_ones = std::all_of(signature.begin(),
                                  signature.end(),
                                  [](int i) { return i == 1; });
      if (all_ones)
        store_xyz = true;
      else
      {
        bool all_minus_ones = std::all_of(signature.begin(),
                                          signature.end(),
                                          [](int i) { return i == -1; });
        if (all_minus_ones)
          store_xyz = true;
      }

      if (store_xyz)
      {
        x.push_back((float)i);
        y.push_back((float)j);
        value.push_back((float)array(i, j));
        LOG_DEBUG("%d %d %f", i, j, array(i, j));
      }
    }

  // rescale positions
  for (size_t k = 0; k < x.size(); k++)
  {
    x[k] /= (float)array.shape.x;
    y[k] /= (float)array.shape.y;
  }

  //
  expand_grid(x, y, value, {0.f, 1.f, 0.f, 1.f});

  // interpolate
  Array array_out = interpolate2d(array.shape,
                                  x,
                                  y,
                                  value,
                                  interpolator2d::delaunay,
                                  p_noise_x,
                                  p_noise_y,
                                  {0.f, 0.f},
                                  {1.f, 1.f});

  return array_out;
}

} // namespace hmap
