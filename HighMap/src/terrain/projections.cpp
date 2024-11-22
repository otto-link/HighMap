/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <stdexcept>

#include "macrologger.h"

#include "highmap/terrain/terrain.hpp"

namespace hmap
{

void interpolate_terrain_heightmap(Terrain           &t_source,
                                   const std::string &heightmap_id_source,
                                   Terrain           &t_target,
                                   const std::string &heightmap_id_target)
{
  hmap::Heightmap *p_s = t_source.get_heightmap_ref(heightmap_id_source);
  hmap::Heightmap *p_t = t_target.get_heightmap_ref(heightmap_id_target);

  if (!(p_s && p_t))
  {
    LOG_DEBUG("interpolate_terrain_heightmap, at least one heightmap not "
              "found: %s %s",
              heightmap_id_source.c_str(),
              heightmap_id_target.c_str());
    return;
  }

  for (size_t k = 0; k < p_t->tiles.size(); k++)
  {
    // for the current tile
    Vec4<float> bbox = p_t->tiles[k].bbox;
    float       lx = bbox.b - bbox.a;
    float       ly = bbox.d - bbox.c;

    for (int i = 0; i < p_t->tiles[k].shape.x; i++)
      for (int j = 0; j < p_t->tiles[k].shape.y; j++)
      {
        // relative position within the heightmap
        float xrel = (float)(i / (p_t->tiles[k].shape.x - 1.f) * lx + bbox.a);
        float yrel = (float)(j / (p_t->tiles[k].shape.y - 1.f) * ly + bbox.c);

        // global position
        Vec2<float> g = t_target.map_to_global_coords(xrel, yrel);

        // get value from source heightmap
        // p_t->tiles[k](i,
        //               j) = t_source.get_heightmap_value_nearest(*p_s, g.x,
        //               g.y);

        p_t->tiles[k](i, j) = t_source.get_heightmap_value_bilinear(*p_s,
                                                                    g.x,
                                                                    g.y);
      }
  }
}

} // namespace hmap
