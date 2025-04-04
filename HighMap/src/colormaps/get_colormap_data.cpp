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
  case Cmap::BONE: return CMAP_BONE;
  case Cmap::GRAY: return CMAP_GRAY;
  case Cmap::HOT: return CMAP_HOT;
  case Cmap::INFERNO: return CMAP_INFERNO;
  case Cmap::JET: return CMAP_JET;
  case Cmap::MAGMA: return CMAP_MAGMA;
  case Cmap::NIPY_SPECTRAL: return CMAP_NIPY_SPECTRAL;
  case Cmap::SEISMIC: return CMAP_SEISMIC;
  case Cmap::TERRAIN: return CMAP_TERRAIN;
  case Cmap::TURBO: return CMAP_TURBO;
  case Cmap::VIRIDIS: return CMAP_VIRIDIS;
  case Cmap::WHITE_UNIFORM: return CMAP_WHITE_UNIFORM;
  }
}

} // namespace hmap
