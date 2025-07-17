/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/array.hpp"
#include "highmap/operator.hpp"

namespace hmap
{

void export_banner_png(const std::string        &fname,
                       const std::vector<Array> &arrays,
                       int                       cmap,
                       bool                      hillshading)
{
  // build up big array by stacking input arrays
  if (arrays.size() > 1)
  {
    Array banner_array = hmap::hstack(arrays[0], arrays[1]);
    for (uint i = 2; i < arrays.size(); i++)
      banner_array = hmap::hstack(banner_array, arrays[i]);
    banner_array.to_png(fname, cmap, hillshading);
  }
  else
  {
    Array banner_array = arrays[0];
    banner_array.to_png(fname, cmap, hillshading);
  }
}

} // namespace hmap
