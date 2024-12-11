/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/math.hpp"
#include "highmap/operator.hpp"

float f(int i, float gi)
{
  return (float)(i * i) + gi * gi;
}

int sep(int i, int u, float gi, float gu)
{
  return (int)((u * u - i * i + gu * gu - gi * gi) / (2 * (u - i)));
}

namespace hmap
{

Array distance_transform(const Array &array, bool return_squared_distance)
{
  Array dt = Array(array.shape); // output distance
  Array g = Array(array.shape);
  int   ni = array.shape.x;
  int   nj = array.shape.y;
  float inf = (float)(ni + nj);

  // phase 1

  for (int i = 0; i < ni; i++)
  {
    // scan 1
    if (array(i, 0) > 0.f)
      g(i, 0) = 0.f;
    else
      g(i, 0) = inf;

    for (int j = 1; j < nj; j++)
    {
      if (array(i, j) > 0.f)
        g(i, j) = 0.f;
      else
        g(i, j) = 1.f + g(i, j - 1);
    }

    // scan 2
    for (int j = nj - 2; j > -1; j--)
      if (g(i, j + 1) < g(i, j)) g(i, j) = 1.f + g(i, j + 1);
  }

  // phase 2
  std::vector<int> s(std::max(ni, nj));
  std::vector<int> t(std::max(ni, nj));

  for (int j = 0; j < nj; j++)
  {
    int q = 0;
    s[0] = 0;
    t[0] = 0;

    // scan 3
    for (int u = 1; u < ni; u++)
    {
      while ((q >= 0) and (f(t[q] - s[q], g(s[q], j)) > f(t[q] - u, g(u, j))))
        q--;

      if (q < 0)
      {
        q = 0;
        s[0] = u;
      }
      else
      {
        int w = 1 + sep(s[q], u, g(s[q], j), g(u, j));

        if (w < ni)
        {
          q++;
          s[q] = u;
          t[q] = w;
        }
      }
    }

    // scan 4
    for (int u = ni - 1; u > -1; u--)
    {
      dt(u, j) = f(u - s[q], g(s[q], j));
      if (u == t[q]) q--;
    }
  }

  if (return_squared_distance)
    return dt;
  else
    return sqrt(dt);
}

} // namespace hmap
