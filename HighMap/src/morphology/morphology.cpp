/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/filters.hpp"
#include "highmap/morphology.hpp"

namespace hmap
{

Array closing(const Array &array, int ir)
{
  return erosion(dilation(array, ir), ir);
}

Array dilation(const Array &array, int ir)
{
  return maximum_local(array, ir);
}

Array erosion(const Array &array, int ir)
{
  return minimum_local(array, ir);
}

void flood_fill(Array &array,
                int    i,
                int    j,
                float  fill_value,
                float  background_value)
{
  std::vector<int> queue_i = {i};
  std::vector<int> queue_j = {j};

  queue_i.reserve(array.shape.x * array.shape.y);
  queue_j.reserve(array.shape.x * array.shape.y);

  while (queue_i.size() > 0)
  {
    i = queue_i.back();
    j = queue_j.back();
    queue_i.pop_back();
    queue_j.pop_back();

    if (array(i, j) == background_value)
    {
      array(i, j) = fill_value;

      if (i > 0)
      {
        queue_i.push_back(i - 1);
        queue_j.push_back(j);
      }
      if (i < array.shape.x - 1)
      {
        queue_i.push_back(i + 1);
        queue_j.push_back(j);
      }
      if (j > 0)
      {
        queue_i.push_back(i);
        queue_j.push_back(j - 1);
      }
      if (j < array.shape.y - 1)
      {
        queue_i.push_back(i);
        queue_j.push_back(j + 1);
      }
    }
  }
}

Array morphological_black_hat(const Array &array, int ir)
{
  return closing(array, ir) - array;
}

Array morphological_gradient(const Array &array, int ir)
{
  return dilation(array, ir) - erosion(array, ir);
}

Array morphological_top_hat(const Array &array, int ir)
{
  return array - opening(array, ir);
}

Array opening(const Array &array, int ir)
{
  return dilation(erosion(array, ir), ir);
}

// helper

void helper_thinning(Array &in, int iter)
{
  Array marker(in.shape);

  for (int i = 1; i < in.shape.x - 1; i++)
    for (int j = 1; j < in.shape.y - 1; j++)
    {
      int a = (in(i - 1, j) == 0.f && in(i - 1, j + 1) == 1.f) +
              (in(i - 1, j + 1) == 0.f && in(i, j + 1) == 1.f) +
              (in(i, j + 1) == 0.f && in(i + 1, j + 1) == 1.f) +
              (in(i + 1, j + 1) == 0.f && in(i + 1, j) == 1.f) +
              (in(i + 1, j) == 0.f && in(i + 1, j - 1) == 1.f) +
              (in(i + 1, j - 1) == 0.f && in(i, j - 1) == 1.f) +
              (in(i, j - 1) == 0.f && in(i - 1, j - 1) == 1.f) +
              (in(i - 1, j - 1) == 0.f && in(i - 1, j) == 1.f);
      int b = in(i - 1, j) + in(i - 1, j + 1) + in(i, j + 1) +
              in(i + 1, j + 1) + in(i + 1, j) + in(i + 1, j - 1) +
              in(i, j - 1) + in(i - 1, j - 1);
      int m1 = iter == 0 ? (in(i - 1, j) * in(i, j + 1) * in(i + 1, j))
                         : (in(i - 1, j) * in(i, j + 1) * in(i, j - 1));
      int m2 = iter == 0 ? (in(i, j + 1) * in(i + 1, j) * in(i, j - 1))
                         : (in(i - 1, j) * in(i + 1, j) * in(i, j - 1));

      if (a == 1 && (b >= 2 && b <= 6) && m1 == 0 && m2 == 0)
        marker(i, j) = 1.f;
    }

  for (int i = 0; i < in.shape.x; i++)
    for (int j = 0; j < in.shape.y; j++)
      in(i, j) *= 1.f - marker(i, j);
}

Array skeleton(const Array &array, bool zeroed_borders)
{
  // https://github.com/krishraghuram/Zhang-Suen-Skeletonization

  Array sk = array;
  Array prev;
  Array diff;

  do
  {
    prev = sk;

    helper_thinning(sk, 0);
    helper_thinning(sk, 1);

    diff = sk - prev;

  } while (diff.count_non_zero() > 0);

  // set border to zero
  if (zeroed_borders) set_borders(sk, 0.f, 1);

  return sk;
}

} // namespace hmap
