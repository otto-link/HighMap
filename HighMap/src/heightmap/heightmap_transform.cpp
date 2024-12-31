/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <functional>
#include <future>
#include <thread>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/heightmap.hpp"

namespace hmap
{

void fill(Heightmap &h, std::function<Array(Vec2<int>)> nullary_op)
{
  size_t                          nthreads = h.get_ntiles();
  std::vector<std::future<Array>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(nullary_op, h.tiles[i].shape);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    h.tiles[i] = futures[i].get();
}

void fill(Heightmap &h, std::function<Array(Vec2<int>, Vec4<float>)> nullary_op)
{
  size_t                          nthreads = h.get_ntiles();
  std::vector<std::future<Array>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(nullary_op, h.tiles[i].shape, h.tiles[i].bbox);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    h.tiles[i] = futures[i].get();
}

void fill(
    Heightmap &h,
    Heightmap *p_noise_x,
    Heightmap *p_noise_y,
    std::function<Array(Vec2<int>, Vec4<float>, hmap::Array *, hmap::Array *)>
        nullary_op)
{
  size_t                          nthreads = h.get_ntiles();
  std::vector<std::future<Array>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
  {
    Array *p_nx = (p_noise_x == nullptr) ? nullptr : &p_noise_x->tiles[i];
    Array *p_ny = (p_noise_y == nullptr) ? nullptr : &p_noise_y->tiles[i];

    futures[i] = std::async(nullary_op,
                            h.tiles[i].shape,
                            h.tiles[i].bbox,
                            p_nx,
                            p_ny);
  }

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    h.tiles[i] = futures[i].get();
}

void fill(Heightmap                          &h,
          Heightmap                          &hin,
          Heightmap                          *p_noise_x,
          Heightmap                          *p_noise_y,
          std::function<Array(hmap::Array &,
                              Vec2<int>,
                              Vec4<float>,
                              hmap::Array *,
                              hmap::Array *)> unary_op)
{
  size_t                          nthreads = h.get_ntiles();
  std::vector<std::future<Array>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
  {
    Array *p_nx = (p_noise_x == nullptr) ? nullptr : &p_noise_x->tiles[i];
    Array *p_ny = (p_noise_y == nullptr) ? nullptr : &p_noise_y->tiles[i];

    futures[i] = std::async(unary_op,
                            std::ref(hin.tiles[i]),
                            h.tiles[i].shape,
                            h.tiles[i].bbox,
                            p_nx,
                            p_ny);
  }

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    h.tiles[i] = futures[i].get();
}

void fill(Heightmap                          &h,
          Heightmap                          *p_noise_x,
          Heightmap                          *p_noise_y,
          Heightmap                          *p_stretching,
          std::function<Array(Vec2<int>,
                              Vec4<float>,
                              hmap::Array *,
                              hmap::Array *,
                              hmap::Array *)> nullary_op)
{
  size_t                          nthreads = h.get_ntiles();
  std::vector<std::future<Array>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
  {
    Array *p_nx = (p_noise_x == nullptr) ? nullptr : &p_noise_x->tiles[i];
    Array *p_ny = (p_noise_y == nullptr) ? nullptr : &p_noise_y->tiles[i];
    Array *p_s = (p_stretching == nullptr) ? nullptr : &p_stretching->tiles[i];

    futures[i] = std::async(nullary_op,
                            h.tiles[i].shape,
                            h.tiles[i].bbox,
                            p_nx,
                            p_ny,
                            p_s);
  }

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    h.tiles[i] = futures[i].get();
}

void fill(
    Heightmap                                                  &h,
    Heightmap                                                  *p_noise,
    std::function<Array(Vec2<int>, Vec4<float>, hmap::Array *)> nullary_op)
{
  size_t                          nthreads = h.get_ntiles();
  std::vector<std::future<Array>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
  {
    Array *p_n = (p_noise == nullptr) ? nullptr : &p_noise->tiles[i];

    futures[i] = std::async(nullary_op, h.tiles[i].shape, h.tiles[i].bbox, p_n);
  }

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    h.tiles[i] = futures[i].get();
}

void transform(Heightmap                    &h_out,
               Heightmap                    &h1,
               std::function<Array(Array &)> unary_op)
{
  size_t                          nthreads = h1.get_ntiles();
  std::vector<std::future<Array>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(unary_op, std::ref(h1.tiles[i]));

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    h_out.tiles[i] = futures[i].get();
}

void transform(Heightmap                             &h_out,
               Heightmap                             &h1,
               Heightmap                             &h2,
               std::function<Array(Array &, Array &)> binary_op)
{
  size_t                          nthreads = h1.get_ntiles();
  std::vector<std::future<Array>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(binary_op,
                            std::ref(h1.tiles[i]),
                            std::ref(h2.tiles[i]));

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    h_out.tiles[i] = futures[i].get();
}

void transform(Heightmap &h, std::function<void(Array &)> unary_op)
{
  size_t                         nthreads = h.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(unary_op, std::ref(h.tiles[i]));

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

// void transform(HeightMap                                             &h,
//                std::function<void(Array &, Vec4<float>)> unary_op)
// {
//   size_t                         nthreads = h.get_ntiles();
//   std::vector<std::future<void>> futures(nthreads);

//   for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
//     futures[i] = std::async(unary_op,
//                             std::ref(h.tiles[i]),
//                             h.tiles[i].bbox);

//   for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
//     futures[i].get();
// }

void transform(Heightmap &h, std::function<void(Array &, Vec4<float>)> unary_op)
{
  size_t                         nthreads = h.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(unary_op, std::ref(h.tiles[i]), h.tiles[i].bbox);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

void transform(Heightmap                                         &h,
               Heightmap                                         *p_noise_x,
               std::function<void(Array &, Vec4<float>, Array *)> unary_op)
{
  size_t                         nthreads = h.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
  {
    Array *p_nx = (p_noise_x == nullptr) ? nullptr : &p_noise_x->tiles[i];

    futures[i] = std::async(unary_op,
                            std::ref(h.tiles[i]),
                            h.tiles[i].bbox,
                            p_nx);
  }

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

void transform(
    Heightmap                                                  &h,
    Heightmap                                                  *p_noise_x,
    Heightmap                                                  *p_noise_y,
    std::function<void(Array &, Vec4<float>, Array *, Array *)> unary_op)
{
  size_t                         nthreads = h.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
  {
    Array *p_nx = (p_noise_x == nullptr) ? nullptr : &p_noise_x->tiles[i];
    Array *p_ny = (p_noise_y == nullptr) ? nullptr : &p_noise_y->tiles[i];

    futures[i] = std::async(unary_op,
                            std::ref(h.tiles[i]),
                            h.tiles[i].bbox,
                            p_nx,
                            p_ny);
  }

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

void transform(Heightmap                            &h,
               Heightmap                            *p_mask,
               std::function<void(Array &, Array *)> unary_op)
{
  size_t                         nthreads = h.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
  {
    Array *p_mask_array = (p_mask == nullptr) ? nullptr : &p_mask->tiles[i];
    futures[i] = std::async(unary_op, std::ref(h.tiles[i]), p_mask_array);
  }

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

void transform(Heightmap                                              &h,
               hmap::Heightmap                                        *p_1,
               hmap::Heightmap                                        *p_2,
               hmap::Heightmap                                        *p_3,
               std::function<void(Array &, Array *, Array *, Array *)> unary_op)
{
  size_t                         nthreads = h.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
  {
    Array *p_1_array = (p_1 == nullptr) ? nullptr : &p_1->tiles[i];
    Array *p_2_array = (p_2 == nullptr) ? nullptr : &p_2->tiles[i];
    Array *p_3_array = (p_3 == nullptr) ? nullptr : &p_3->tiles[i];

    futures[i] = std::async(unary_op,
                            std::ref(h.tiles[i]),
                            p_1_array,
                            p_2_array,
                            p_3_array);
  }

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

void transform(
    Heightmap       &h,
    hmap::Heightmap *p_1,
    hmap::Heightmap *p_2,
    hmap::Heightmap *p_3,
    hmap::Heightmap *p_4,
    hmap::Heightmap *p_5,
    std::function<void(Array &, Array *, Array *, Array *, Array *, Array *)>
        unary_op)
{
  size_t                         nthreads = h.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
  {
    Array *p_1_array = (p_1 == nullptr) ? nullptr : &p_1->tiles[i];
    Array *p_2_array = (p_2 == nullptr) ? nullptr : &p_2->tiles[i];
    Array *p_3_array = (p_3 == nullptr) ? nullptr : &p_3->tiles[i];
    Array *p_4_array = (p_4 == nullptr) ? nullptr : &p_4->tiles[i];
    Array *p_5_array = (p_5 == nullptr) ? nullptr : &p_5->tiles[i];

    futures[i] = std::async(unary_op,
                            std::ref(h.tiles[i]),
                            p_1_array,
                            p_2_array,
                            p_3_array,
                            p_4_array,
                            p_5_array);
  }

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

void transform(Heightmap                                     &h,
               hmap::Heightmap                               *p_1,
               hmap::Heightmap                               *p_2,
               std::function<void(Array &, Array *, Array *)> unary_op)
{
  size_t                         nthreads = h.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
  {
    Array *p_1_array = (p_1 == nullptr) ? nullptr : &p_1->tiles[i];
    Array *p_2_array = (p_2 == nullptr) ? nullptr : &p_2->tiles[i];

    futures[i] = std::async(unary_op,
                            std::ref(h.tiles[i]),
                            p_1_array,
                            p_2_array);
  }

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

void transform(Heightmap                            &h1,
               Heightmap                            &h2,
               std::function<void(Array &, Array &)> binary_op)
{
  size_t                         nthreads = h1.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(binary_op,
                            std::ref(h1.tiles[i]),
                            std::ref(h2.tiles[i]));

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

void transform(Heightmap                                         &h1,
               Heightmap                                         &h2,
               std::function<void(Array &, Array &, Vec4<float>)> binary_op)
{
  size_t                         nthreads = h1.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(binary_op,
                            std::ref(h1.tiles[i]),
                            std::ref(h2.tiles[i]),
                            h1.tiles[i].bbox);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

void transform(Heightmap                                     &h1,
               Heightmap                                     &h2,
               Heightmap                                     &h3,
               std::function<void(Array &, Array &, Array &)> ternary_op)
{
  size_t                         nthreads = h1.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(ternary_op,
                            std::ref(h1.tiles[i]),
                            std::ref(h2.tiles[i]),
                            std::ref(h3.tiles[i]));

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

void transform(
    Heightmap                                                  &h1,
    Heightmap                                                  &h2,
    Heightmap                                                  &h3,
    std::function<void(Array &, Array &, Array &, Vec4<float>)> ternary_op)
{
  size_t                         nthreads = h1.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(ternary_op,
                            std::ref(h1.tiles[i]),
                            std::ref(h2.tiles[i]),
                            std::ref(h3.tiles[i]),
                            h1.tiles[i].bbox);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

void transform(
    Heightmap                                              &h1,
    Heightmap                                              &h2,
    Heightmap                                              &h3,
    Heightmap                                              &h4,
    std::function<void(Array &, Array &, Array &, Array &)> quaternary_op)
{
  size_t                         nthreads = h1.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(quaternary_op,
                            std::ref(h1.tiles[i]),
                            std::ref(h2.tiles[i]),
                            std::ref(h3.tiles[i]),
                            std::ref(h4.tiles[i]));

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

void transform(
    Heightmap &h1,
    Heightmap &h2,
    Heightmap &h3,
    Heightmap &h4,
    Heightmap &h5,
    Heightmap &h6,
    std::function<void(Array &, Array &, Array &, Array &, Array &, Array &)>
        op)
{
  size_t                         nthreads = h1.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(op,
                            std::ref(h1.tiles[i]),
                            std::ref(h2.tiles[i]),
                            std::ref(h3.tiles[i]),
                            std::ref(h4.tiles[i]),
                            std::ref(h5.tiles[i]),
                            std::ref(h6.tiles[i]));

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

// --- generic transform

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

    Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f};
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

} // namespace hmap
