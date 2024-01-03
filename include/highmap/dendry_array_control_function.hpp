/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file dendry_array_control_function.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <cassert>
#include <utility>

#include "dendry/include/controlfunction.h"
#include "dendry/include/math2d.h"
#include "dendry/include/utils.h"

#include "highmap/array.hpp"
#include "highmap/dendry_array_control_function.hpp"

class ArrayControlFunction : public ControlFunction<ArrayControlFunction>
{
  friend class ControlFunction<ArrayControlFunction>;

public:
  explicit ArrayControlFunction(hmap::Array array) : m_array(std::move(array))
  {
  }

protected:
  double EvaluateImpl(double x, double y) const
  {
    x = std::clamp(x, 0.0, 1.0);
    y = std::clamp(y, 0.0, 1.0);

    return sample(x, y);
  }

  bool InsideDomainImpl(double x, double y) const
  {
    return x >= 0.0 && x <= 1.0 && y >= 0.0 && y <= 1.0;
  }

  double DistToDomainImpl(double x, double y) const
  {
    if (InsideDomainImpl(x, y))
    {
      return 0.0;
    }

    const Point2D p(x, y);

    const Point2D topLeft(0.0, 0.0);
    const Point2D topRight(1.0, 0.0);
    const Point2D bottomLeft(0.0, 1.0);
    const Point2D bottomRight(1.0, 1.0);

    Point2D c; // Useless point for distToLineSegment

    auto dist = std::numeric_limits<double>::max();

    dist = std::min(dist, distToLineSegment(p, topLeft, topRight, c));
    dist = std::min(dist, distToLineSegment(p, topRight, bottomRight, c));
    dist = std::min(dist, distToLineSegment(p, bottomRight, bottomLeft, c));
    dist = std::min(dist, distToLineSegment(p, bottomLeft, topLeft, c));

    return dist;
  }

  double MinimumImpl() const
  {
    return 0.0;
  }

  double MaximumImpl() const
  {
    return 1.0;
  }

private:
  double get(int i, int j) const
  {
    return (double)m_array(i, j);
  }

  double sample(double ri, double rj) const
  {
    float x = ri * (m_array.shape.x - 1);
    float y = rj * (m_array.shape.y - 1);

    int i = (int)x;
    int j = (int)y;

    float u;
    float v;

    if (i == m_array.shape.x - 1)
    {
      i = m_array.shape.x - 2;
      u = 1.f;
    }
    else
      u = x - (float)i;

    if (j == m_array.shape.y - 1)
    {
      j = m_array.shape.y - 2;
      v = 1.f;
    }
    else
      v = y - (float)j;

    return m_array.get_value_bilinear_at(i, j, u, v);
  }

  const hmap::Array m_array;
};
