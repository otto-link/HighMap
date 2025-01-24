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

Array border(const Array &array, int ir)
{
  return array - erosion(array, ir);
}

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

  for (int j = 1; j < in.shape.y - 1; j++)
    for (int i = 1; i < in.shape.x - 1; i++)
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

  for (int j = 0; j < in.shape.y; j++)
    for (int i = 0; i < in.shape.x; i++)
      in(i, j) *= 1.f - marker(i, j);
}

Array relative_distance_from_skeleton(const Array &array,
                                      int          ir_search,
                                      bool         zero_at_borders,
                                      int          ir_erosion)
{
  Array border = array - erosion(array, ir_erosion);
  Array sk = skeleton(array, zero_at_borders);

  Array rdist(array.shape);

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      // only work for cells within the non-zero regions
      if (array(i, j) != 0.f)
      {
        // find the closest skeleton and border cells
        float dmax_sk = std::numeric_limits<float>::max();
        float dmax_bd = std::numeric_limits<float>::max();

        int p1 = std::max(i - ir_search, 0);
        int p2 = std::min(i + ir_search + 1, array.shape.x);
        int q1 = std::max(j - ir_search, 0);
        int q2 = std::min(j + ir_search + 1, array.shape.y);

        for (int q = q1; q < q2; q++)
          for (int p = p1; p < p2; p++)
          {
            // distance to skeleton
            if (sk(p, q) == 1.f)
            {
              float d2 = (float)((i - p) * (i - p) + (j - q) * (j - q));
              if (d2 < dmax_sk) dmax_sk = d2;
            }

            // distance to border
            if (border(p, q) == 1.f)
            {
              float d2 = (float)((i - p) * (i - p) + (j - q) * (j - q));
              if (d2 < dmax_bd) dmax_bd = d2;
            }
          }

        // relative distance (from 1.f on the skeleton to 0.f and
        // the border)
        float sum = dmax_bd + dmax_sk;
        if (sum) rdist(i, j) = dmax_bd / sum;
      }

  return rdist;
}

Array skeleton(const Array &array, bool zero_at_borders)
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
  if (zero_at_borders) zeroed_borders(sk);

  return sk;
}

} // namespace hmap
