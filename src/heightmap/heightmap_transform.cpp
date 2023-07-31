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

void fill(HeightMap &h, std::function<Array(std::vector<int>)> nullary_op)
{
  LOG_DEBUG("nullary (shape)");
  size_t                          nthreads = h.get_ntiles();
  std::vector<std::future<Array>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(nullary_op, h.tiles[i].shape);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    h.tiles[i] = futures[i].get();
}

void fill(HeightMap                               &h,
          std::function<Array(std::vector<int>,
                              std::vector<float>,
                              std::vector<float>)> nullary_op)
{
  LOG_DEBUG("nullary (shape, size)");
  size_t                          nthreads = h.get_ntiles();
  std::vector<std::future<Array>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(nullary_op,
                            h.tiles[i].shape,
                            h.tiles[i].shift,
                            h.tiles[i].scale);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    h.tiles[i] = futures[i].get();

  // for (auto &t : h.tiles)
  //   t = nullary_op(t.shape, t.shift);
}

void fill(HeightMap                          &h,
          HeightMap                          *p_noise,
          std::function<Array(std::vector<int>,
                              std::vector<float>,
                              std::vector<float>,
                              hmap::Array *)> nullary_op)
{
  LOG_DEBUG("nullary (shape, size, p_noise)");
  size_t                          nthreads = h.get_ntiles();
  std::vector<std::future<Array>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(nullary_op,
                            h.tiles[i].shape,
                            h.tiles[i].shift,
                            h.tiles[i].scale,
                            &p_noise->tiles[i]);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    h.tiles[i] = futures[i].get();
}

void fill(HeightMap                          &h,
          HeightMap                          *p_noise_x,
          HeightMap                          *p_noise_y,
          std::function<Array(std::vector<int>,
                              std::vector<float>,
                              std::vector<float>,
                              hmap::Array *,
                              hmap::Array *)> nullary_op)
{
  LOG_DEBUG("nullary (shape, size, p_noise, p_noise)");
  size_t                          nthreads = h.get_ntiles();
  std::vector<std::future<Array>> futures(nthreads);

  LOG_DEBUG("pointer: %p", (void *)p_noise_y);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
  {
    Array *p_nx = (p_noise_x == nullptr) ? nullptr : &p_noise_x->tiles[i];
    Array *p_ny = (p_noise_y == nullptr) ? nullptr : &p_noise_y->tiles[i];

    futures[i] = std::async(nullary_op,
                            h.tiles[i].shape,
                            h.tiles[i].shift,
                            h.tiles[i].scale,
                            p_nx,
                            p_ny);
  }

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    h.tiles[i] = futures[i].get();
}

void transform(HeightMap &h, std::function<void(Array &)> unary_op)
{
  LOG_DEBUG("unary");
  size_t                         nthreads = h.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(unary_op, std::ref(h.tiles[i]));

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

void transform(HeightMap                            &h1,
               HeightMap                            &h2,
               std::function<void(Array &, Array &)> binary_op)
{
  LOG_DEBUG("binary");
  size_t                         nthreads = h1.get_ntiles();
  std::vector<std::future<void>> futures(nthreads);

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i] = std::async(binary_op,
                            std::ref(h1.tiles[i]),
                            std::ref(h2.tiles[i]));

  for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    futures[i].get();
}

void transform(HeightMap                                     &h1,
               HeightMap                                     &h2,
               HeightMap                                     &h3,
               std::function<void(Array &, Array &, Array &)> ternary_op)
{
  LOG_DEBUG("ternary");
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

} // namespace hmap
