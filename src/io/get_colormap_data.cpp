/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <vector>

#include "highmap/colormaps.hpp"

namespace hmap
{

std::vector<std::vector<float>> get_colormap_data(int cmap)
{
  switch (cmap)
  {
  case cmap::bone:
    return CMAP_BONE;
  case cmap::gray:
    return CMAP_GRAY;
  case cmap::hot:
    return CMAP_HOT;
  case cmap::inferno:
    return CMAP_INFERNO;
  case cmap::jet:
    return CMAP_JET;
  case cmap::magma:
    return CMAP_MAGMA;
  case cmap::nipy_spectral:
    return CMAP_NIPY_SPECTRAL;
  case cmap::seismic:
    return CMAP_SEISMIC;
  case cmap::terrain:
    return CMAP_TERRAIN;
  case cmap::viridis:
    return CMAP_VIRIDIS;
  }
}

} // namespace hmap
