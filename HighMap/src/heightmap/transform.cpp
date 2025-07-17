/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>
#include <future>
#include <thread>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry/point.hpp"
#include "highmap/heightmap.hpp"

namespace hmap
{

void transform(std::vector<Heightmap *>                     p_hmaps,
               std::function<void(const std::vector<Array *>,
                                  const hmap::Vec2<int>,
                                  const hmap::Vec4<float>)> op,
               TransformMode                                transform_mode)
{
  if (!p_hmaps.size())
  {
    LOG_ERROR("the list of hmap::Heightmap pointers provided is empty, nothing "
              "to do here");
    return;
  }

  switch (transform_mode)
  {
  case TransformMode::DISTRIBUTED:
  {
    size_t                         nthreads = p_hmaps[0]->get_ntiles();
    std::vector<std::future<void>> futures(nthreads);

    for (size_t i = 0; i < nthreads; ++i)
    {
      // fill-in arrays pointers
      std::vector<Array *> p_arrays = {};
      for (auto p_h : p_hmaps)
        p_arrays.push_back((p_h == nullptr) ? nullptr : &p_h->tiles[i]);

      futures[i] = std::async(op,
                              p_arrays,
                              p_hmaps[0]->tiles[i].shape,
                              p_hmaps[0]->tiles[i].bbox);
    }

    for (size_t i = 0; i < nthreads; ++i)
      futures[i].get();
  }
  break;
  //
  case TransformMode::SEQUENTIAL:
  {
    for (size_t i = 0; i < p_hmaps[0]->get_ntiles(); ++i)
    {
      std::vector<Array *> p_arrays = {};
      for (auto p_h : p_hmaps)
        p_arrays.push_back((p_h == nullptr) ? nullptr : &p_h->tiles[i]);

      op(p_arrays, p_hmaps[0]->tiles[i].shape, p_hmaps[0]->tiles[i].bbox);
    }
  }
  break;
  //
  case TransformMode::SINGLE_ARRAY:
  {
    std::vector<Array *> p_arrays = {};
    std::vector<Array>   arrays = {};

    // generate and store arrays build from heightmaps
    for (auto p_h : p_hmaps)
    {
      hmap::Array array = p_h ? p_h->to_array() : hmap::Array();
      arrays.push_back(std::move(array));
    }

    // pointers
    for (size_t k = 0; k < p_hmaps.size(); k++)
      if (p_hmaps[k])
        p_arrays.push_back(&arrays[k]);
      else
        p_arrays.push_back(nullptr);

    Vec4<float> bbox = unit_square_bbox();
    op(p_arrays, p_hmaps[0]->shape, bbox);

    // convert back to heightmaps from arrays
    for (size_t k = 0; k < p_hmaps.size(); k++)
      if (p_hmaps[k]) p_hmaps[k]->from_array_interp_nearest(arrays[k]);
  }
  break;
  //
  default: LOG_ERROR("unknown hmap::Heightmap transform mode"); return;
  }
}

void transform(std::vector<Heightmap *>                        p_hmaps,
               std::function<void(const std::vector<Array *>)> op,
               TransformMode                                   transform_mode)
{
  // use a pass-through wrapper
  auto op_wrap = [op](const std::vector<Array *> p_arrays,
                      const hmap::Vec2<int>,
                      const hmap::Vec4<float>) { op(p_arrays); };

  transform(p_hmaps, op_wrap, transform_mode);
}

} // namespace hmap
