/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"

namespace hmap
{

void export_birdview_png(std::string  fname,
                         const Array &array,
                         const float  gamma)
{
  std::vector<uint16_t> img(array.shape.x * array.shape.y * 3);

  Array hs = hillshade(array,
                       180.f,
                       45.f,
                       10.f * array.ptp() / (float)array.shape.y);
  Array shadow = shadow_heightmap(array, 180.f, 45.f, 0.2f);
  clamp_min_smooth(shadow, 0.3f, 0.2f);
  clamp_max_smooth(shadow, 1.f, 0.2f);
  hs = pow(hs, gamma) * pow(shadow, gamma);
  clamp(hs);

  int k = 0;
  for (int j = array.shape.y - 1; j > -1; j -= 1)
    for (int i = 0; i < array.shape.x; i++)
    {
      img[k++] = (uint16_t)(65535.f * hs(i, j)); // R
      img[k++] = (uint16_t)(65535.f * hs(i, j)); // G
      img[k++] = (uint16_t)(65535.f * hs(i, j)); // B
    }
  write_png_rgb_16bit(fname, img, array.shape);
}

} // namespace hmap
