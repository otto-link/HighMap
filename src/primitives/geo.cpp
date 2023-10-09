/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "FastNoiseLite.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array caldera(Vec2<int>   shape,
              float       radius,
              float       sigma_inner,
              float       sigma_outer,
              float       z_bottom,
              Array      *p_noise,
              float       noise_r_amp,
              float       noise_z_ratio,
              Vec2<float> shift,
              Vec2<float> scale)
{
  Array z = Array(shape);
  int   ic = (int)((0.5f - shift.x) / scale.x * z.shape.x);
  int   jc = (int)((0.5f - shift.y) / scale.y * z.shape.y);

  float si2 = sigma_inner * sigma_inner;
  float so2 = sigma_outer * sigma_outer;

  if (p_noise)
  {
    for (int i = 0; i < z.shape.x; i++)
      for (int j = 0; j < z.shape.y; j++)
      {
        float r = std::hypot((float)(i - ic), (float)(j - jc)) - radius;

        r += noise_r_amp * (2 * (*p_noise)(i, j) - 1);

        if (r < 0.f)
          z(i, j) = z_bottom + std::exp(-0.5f * r * r / si2) * (1 - z_bottom);
        else
          z(i, j) = 1 / (1 + r * r / so2);

        z(i, j) *= 1.f + noise_z_ratio * (2.f * (*p_noise)(i, j) - 1.f);
      }
  }
  else
  {
    for (int i = 0; i < z.shape.x; i++)
      for (int j = 0; j < z.shape.y; j++)
      {
        float r = std::hypot((float)(i - ic), (float)(j - jc)) - radius;

        if (r < 0.f)
          z(i, j) = z_bottom + std::exp(-0.5f * r * r / si2) * (1 - z_bottom);
        else
          z(i, j) = 1 / (1 + r * r / so2);
      }
  }

  return z;
}

Array caldera(Vec2<int>   shape,
              float       radius,
              float       sigma_inner,
              float       sigma_outer,
              float       z_bottom,
              Vec2<float> shift,
              Vec2<float> scale)
{
  Array noise = constant(shape, 0.f);
  Array z = caldera(shape,
                    radius,
                    sigma_inner,
                    sigma_outer,
                    z_bottom,
                    nullptr,
                    0.f,
                    0.f,
                    shift,
                    scale);
  return z;
}

Array crater(Vec2<int>   shape,
             float       radius,
             float       depth,
             float       lip_decay,
             float       lip_height_ratio,
             Array      *p_noise,
             Vec2<float> shift,
             Vec2<float> scale)
{
  Array z = Array(shape);
  int   ic = (int)((0.5f - shift.x) / scale.x * z.shape.x);
  int   jc = (int)((0.5f - shift.y) / scale.y * z.shape.y);

  if (!p_noise)
  {
    for (int i = 0; i < z.shape.x; i++)
      for (int j = 0; j < z.shape.y; j++)
      {
        float r = std::hypot((float)(i - ic), (float)(j - jc));

        z(i, j) = std::min(r * r / (radius * radius),
                           1.f + lip_height_ratio *
                                     std::exp(-(r - radius) / lip_decay));
        z(i, j) -= 1.f;
        z(i, j) *= depth;
      }
  }
  else
  {
    for (int i = 0; i < z.shape.x; i++)
      for (int j = 0; j < z.shape.y; j++)
      {
        float r = std::hypot((float)(i - ic), (float)(j - jc));
        r += (*p_noise)(i, j) * std::min(shape.x / scale.x, shape.y / scale.y);

        z(i, j) = std::min(r * r / (radius * radius),
                           1.f + lip_height_ratio *
                                     std::exp(-(r - radius) / lip_decay));
        z(i, j) -= 1.f;
        z(i, j) *= depth;
      }
  }

  return z;
}

Array peak(Vec2<int>   shape,
           float       radius,
           Array      *p_noise,
           float       noise_r_amp,
           float       noise_z_ratio,
           Vec2<float> shift,
           Vec2<float> scale)
{
  Array z = Array(shape);
  int   ic = (int)((0.5f - shift.x) / scale.x * z.shape.x);
  int   jc = (int)((0.5f - shift.y) / scale.y * z.shape.y);

  if (!p_noise)
  {
    for (int i = 0; i < z.shape.x; i++)
      for (int j = 0; j < z.shape.y; j++)
      {
        float r = std::hypot((float)(i - ic), (float)(j - jc)) / radius;

        if (r < 1.f)
          z(i, j) = 1.f - r * r * (3.f - 2.f * r);
      }
  }
  else
  {
    for (int i = 0; i < z.shape.x; i++)
      for (int j = 0; j < z.shape.y; j++)
      {
        float r = std::hypot((float)(i - ic), (float)(j - jc)) / radius;
        r += r * noise_r_amp / radius * (2 * (*p_noise)(i, j) - 1);

        if (r < 1.f)
          z(i, j) = 1.f - r * r * (3.f - 2.f * r);

        z(i, j) *= 1.f + noise_z_ratio * (2.f * (*p_noise)(i, j) - 1.f);
      }
  }

  return z;
}

} // namespace hmap
