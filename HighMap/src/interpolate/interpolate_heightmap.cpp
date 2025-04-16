/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/coord_frame.hpp"
#include "highmap/math.hpp"

namespace hmap
{

void flatten_heightmap(const hmap::Heightmap &h_source1,
                       const hmap::Heightmap &h_source2,
                       hmap::Heightmap       &h_target,
                       const CoordFrame      &t_source1,
                       const CoordFrame      &t_source2,
                       const CoordFrame      &t_target)
{
  for (size_t k = 0; k < h_target.tiles.size(); k++)
  {
    Vec4<float> bbox = h_target.tiles[k].bbox;

    for (int j = 0; j < h_target.tiles[k].shape.y; j++)
      for (int i = 0; i < h_target.tiles[k].shape.x; i++)
      {
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

        // values
        float v_source1 = t_source1.get_heightmap_value_bilinear(h_source1,
                                                                 g.x,
                                                                 g.y);

        if (!t_source2.is_point_within(g.x, g.y))
        {
          h_target.tiles[k](i, j) = v_source1;
        }
        else
        {
          float v_source2 = t_source2.get_heightmap_value_bilinear(h_source2,
                                                                   g.x,
                                                                   g.y);

          // transition between the tow heightmaps based on the
          // distance to the bounding box
          float r = t_source2.normalized_distance_to_edges(g.x, g.y);
          r = smoothstep3(r);

          h_target.tiles[k](i, j) = lerp(v_source1, v_source2, r);
        }
      }
  }
}

void interpolate_heightmap(const hmap::Heightmap &h_source,
                           hmap::Heightmap       &h_target,
                           const CoordFrame      &t_source,
                           const CoordFrame      &t_target)
{
  for (size_t k = 0; k < h_target.tiles.size(); k++)
  {
    Vec4<float> bbox = h_target.tiles[k].bbox;

    for (int j = 0; j < h_target.tiles[k].shape.y; j++)
      for (int i = 0; i < h_target.tiles[k].shape.x; i++)
      {
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
