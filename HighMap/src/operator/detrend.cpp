/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"

namespace hmap
{

Array detrend_reg(const Array &array)
{
  Array out = Array(array.shape);

  // --- columns

  for (int i = 0; i < array.shape.x; i++)
  {
    int n = array.shape.y;

    // slope
    float sx = 0.5f * (n - 1.f) * n;
    float sxx = (n - 1.f) * n * (2.f * n - 1.f);

    float sy = 0.f;
    float sxy = 0.f;

    for (int j = 0; j < array.shape.y; j++)
    {
      sy += array(i, j);
      sxy += (float)j * array(i, j);
    }

    float b = (sy * sxx - sx * sxy) / (n * sxx - sx * sx);
    float m = (n * sxy - sx * sy) / (n * sxx - sx * sx);

    // detrend
    for (int j = 0; j < array.shape.y; j++)
      out(i, j) = array(i, j) - m * j - b;
  }

  // --- lines

  for (int j = 0; j < array.shape.y; j++)
  {
    int n = array.shape.x;

    // slope
    float sx = 0.5f * (n - 1.f) * n;
    float sxx = (n - 1.f) * n * (2.f * n - 1.f);

    float sy = 0.f;
    float sxy = 0.f;

    for (int i = 0; i < array.shape.x; i++)
    {
      sy += array(i, j);
      sxy += (float)j * array(i, j);
    }

    float b = (sy * sxx - sx * sxy) / (n * sxx - sx * sx);
    float m = (n * sxy - sx * sy) / (n * sxx - sx * sx);

    // detrend
    for (int i = 0; i < array.shape.x; i++)
      out(i, j) = array(i, j) - m * i - b;
  }

  return out;
}

} // namespace hmap
