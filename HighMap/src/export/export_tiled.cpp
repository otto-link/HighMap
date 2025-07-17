/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/internal/string_utils.hpp"

namespace hmap
{

void export_tiled(const std::string &fname_radical,
                  const std::string &fname_extension,
                  const Array       &array,
                  const Vec2<int>   &tiling,
                  int                leading_zeros,
                  int                depth,
                  bool               overlapping_edges,
                  bool               reverse_tile_y_indexing)
{
  int nx = static_cast<int>(std::ceil((float)array.shape.x / tiling.x));
  int ny = static_cast<int>(std::ceil((float)array.shape.y / tiling.y));

  for (int it = 0; it < tiling.x; it++)
    for (int jt = 0; jt < tiling.y; jt++)
    {
      LOG_DEBUG("%d %d", it, jt);
      // define tile indices extent
      int i1 = it * nx;
      int i2 = (it + 1) * nx;

      int j1, j2;

      if (reverse_tile_y_indexing)
      {
        int jt_inv = tiling.y - 1 - jt;

        j1 = jt_inv * ny;
        j2 = (jt_inv + 1) * ny;
      }
      else
      {
        j1 = jt * ny;
        j2 = (jt + 1) * ny;
      }

      if (overlapping_edges)
      {
        i2++;
        j2++;
      }

      i2 = std::min(i2, array.shape.x - 1);
      j2 = std::min(j2, array.shape.y - 1);

      Array tile = array.extract_slice(i1, i2, j1, j2);

      // export to image file
      std::string str_it = zfill(std::to_string(it), leading_zeros);
      std::string str_jt = zfill(std::to_string(jt), leading_zeros);

      std::string fname_tile = fname_radical + "_" + str_it + "_" + str_jt +
                               "." + fname_extension;

      tile.to_png_grayscale(fname_tile, depth);
    }
}

} // namespace hmap
