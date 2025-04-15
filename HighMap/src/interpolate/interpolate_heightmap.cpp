/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/coord_frame.hpp"

namespace hmap
{

void interpolate_heightmap(const hmap::Heightmap &h_source,
                           hmap::Heightmap       &h_target,
                           const CoordFrame      &t_source,
                           const CoordFrame      &t_target)
{
  for (size_t k = 0; k < h_target.tiles.size(); k++)
  {
    for (int j = 0; j < h_target.tiles[k].shape.y; j++)
      for (int i = 0; i < h_target.tiles[k].shape.x; i++)
      {
        Vec4<float> bbox = h_target.tiles[k].bbox;

        // relative position within the heightmap (NB - end points of
        // the bounding box are not included in the grid)
        float xrel = (float)i / (float)h_target.tiles[k].shape.x *
                         (bbox.b - bbox.a) +
                     bbox.a;
        float yrel = (float)j / (float)h_target.tiles[k].shape.y *
                         (bbox.d - bbox.c) +
                     bbox.c;

        // global position
        Vec2<float> g = t_target.map_to_global_coords(xrel, yrel);

        h_target.tiles[k](
            i,
            j) = t_source.get_heightmap_value_bilinear(h_source, g.x, g.y);
      }
  }
}

} // namespace hmap
