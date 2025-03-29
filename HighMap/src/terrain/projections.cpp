/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <stdexcept>

#include "macrologger.h"

#include "highmap/terrain/terrain.hpp"

namespace hmap
{

void interpolate_terrain_heightmap(Terrain         &t_source,
                                   hmap::Heightmap &h_source,
                                   Terrain         &t_target,
                                   hmap::Heightmap &h_target)
{
  for (size_t k = 0; k < h_target.tiles.size(); k++)
  {
    // for the current tile
    Vec4<float> bbox = h_target.tiles[k].bbox;
    float       lx = bbox.b - bbox.a;
    float       ly = bbox.d - bbox.c;

    // end point of the bounding box is not included in the grid
    lx -= lx / h_target.tiles[k].shape.x;
    ly -= ly / h_target.tiles[k].shape.y;

    for (int j = 0; j < h_target.tiles[k].shape.y; j++)
      for (int i = 0; i < h_target.tiles[k].shape.x; i++)
      {
        // relative position within the heightmap
        float xrel = (float)(i / (h_target.tiles[k].shape.x - 1.f) * lx +
                             bbox.a);
        float yrel = (float)(j / (h_target.tiles[k].shape.y - 1.f) * ly +
                             bbox.c);

        // global position
        Vec2<float> g = t_target.map_to_global_coords(xrel, yrel);

        // get value from source heightmap

        // h_target.tiles[k](i,
        //               j) = t_source.get_heightmap_value_nearest(*h_source,
        //               g.x, g.y);

        h_target.tiles[k](
            i,
            j) = t_source.get_heightmap_value_bilinear(h_source, g.x, g.y);
      }
  }
}

} // namespace hmap
