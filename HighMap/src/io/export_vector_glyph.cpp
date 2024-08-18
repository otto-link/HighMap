/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/export.hpp"
#include "highmap/math.hpp"
#include "highmap/operator.hpp"

namespace hmap
{

Array render_vector_raster(const Array &array_u,
                           const Array &array_v,
                           const float  density,
                           const float  scale,
                           const uint   seed)
{
  Vec2<int>                          shape = array_u.shape;
  std::mt19937                       gen(seed);
  std::uniform_int_distribution<int> dis_i(0, shape.x - 1);
  std::uniform_int_distribution<int> dis_j(0, shape.y - 1);
  Array                              raster = Array(shape);
  int                                npoints = (int)(density * array_u.size());

  Array huv = hypot(array_u, array_v);
  float cnorm = 1.f / huv.max();
  float scale_pixels = scale * std::max(shape.x, shape.y);

  for (int k = 0; k < npoints; k++)
  {
    int i = dis_i(gen);
    int j = dis_j(gen);
    int ip = i + (int)(scale_pixels * array_u(i, j) * cnorm);
    int jp = j + (int)(scale_pixels * array_v(i, j) * cnorm);

    if (ip > 0 && ip < shape.x - 1 && jp > 0 && jp < shape.y - 1)
    {
      // line color
      float level = std::pow(huv(i, j) * cnorm, 2.f);

      // segment beginning
      raster(i, j) = std::max(level, raster(i, j));

      // then draw a segment
      int npoints_line = (int)std::ceil(std::hypot(i - ip, j - jp));
      for (int q = 1; q < npoints_line - 1; q++)
      {
        float r = (float)((float)q / (npoints_line - 1.f));

        int ir = (int)((1.f - r) * i + r * ip);
        int jr = (int)((1.f - r) * j + r * jp);
        raster(ir, jr) = std::max(level, raster(ir, jr));
      }
    }
  }
  return raster;
}

void export_vector_glyph_png_8bit(const std::string fname,
                                  const Array      &array_u,
                                  const Array      &array_v,
                                  const float       density,
                                  const float       scale,
                                  const uint        seed)
{
  Vec2<int> shape = array_u.shape;

  std::vector<uint8_t> img(shape.x * shape.y * 3);
  Array raster = render_vector_raster(array_u, array_v, density, scale, seed);

  int k = 0;
  for (int j = shape.y - 1; j > -1; j -= 1)
    for (int i = 0; i < shape.x; i++)
    {
      img[k++] = (uint8_t)(255.f * raster(i, j)); // R
      img[k++] = (uint8_t)(255.f * raster(i, j)); // G
      img[k++] = (uint8_t)(255.f * raster(i, j)); // B
    }
  write_png_rgb_8bit(fname, img, shape);
}

void export_vector_glyph_png_16bit(const std::string fname,
                                   const Array      &array_u,
                                   const Array      &array_v,
                                   const float       density,
                                   const float       scale,
                                   const uint        seed)
{
  Vec2<int> shape = array_u.shape;

  std::vector<uint16_t> img(shape.x * shape.y * 3);
  Array raster = render_vector_raster(array_u, array_v, density, scale, seed);

  int k = 0;
  for (int j = shape.y - 1; j > -1; j -= 1)
    for (int i = 0; i < shape.x; i++)
    {
      img[k++] = (uint16_t)(65535.f * raster(i, j)); // R
      img[k++] = (uint16_t)(65535.f * raster(i, j)); // G
      img[k++] = (uint16_t)(65535.f * raster(i, j)); // B
    }
  write_png_rgb_16bit(fname, img, shape);
}

} // namespace hmap
