/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <queue>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap
{

// --- helpers

Array get_random_patch(Array          &array,
                       hmap::Vec2<int> patch_shape,
                       std::mt19937   &gen)
{
  std::uniform_real_distribution<float> dis_i(0,
                                              array.shape.x - 1 -
                                                  patch_shape.x);
  std::uniform_real_distribution<float> dis_j(0,
                                              array.shape.y - 1 -
                                                  patch_shape.y);

  // random pair of indices
  int i = dis_i(gen);
  int j = dis_j(gen);

  Array patch = array.extract_slice(
      Vec4<int>(i, i + patch_shape.x, j, j + patch_shape.y));

  return patch;
}

bool cmp_path(std::pair<float, std::vector<int>> &a,
              std::pair<float, std::vector<int>> &b)
{
  return a.first < b.first;
}

// among all the possible cut path from top to bottom and find the one
// with the minimum cost using Dijkstra's algorithm
void find_vertical_cut_path(Array &error, std::vector<int> &path_i)
{
  Vec2<int> shape = error.shape;

  path_i.clear();
  path_i.reserve(shape.y);

  Mat<int> cell_done(shape);

  // queue of possible paths from bottom to top (described but a
  // list of 'i' indices std::vector<int>) and their cumulative
  // error (float)
  std::vector<std::pair<float, std::vector<int>>> queue;

  // populate queue from the top
  for (int i = 0; i < shape.x; i++)
    queue.push_back(std::pair(error(i, 0), std::vector<int>({i})));

  std::make_heap(queue.begin(), queue.end(), cmp_path);

  while (queue.size() > 0)
  {
    std::pair<float, std::vector<int>> current = queue.back();
    queue.pop_back();

    int i = current.second.back();
    int j = (int)current.second.size() - 1;

    if (j == shape.y - 1)
    {
      // top has been reached, we're done
      for (auto &v : current.second)
        path_i.push_back(v);
      break;
    }

    for (int di = -1; di < 2; di++)
    {
      int inext = i + di;
      if (inext >= 0 && inext < shape.x)
        if (cell_done(inext, j + 1) == 0)
        {
          std::vector<int> new_path = current.second;
          new_path.push_back(inext);

          queue.push_back(std::pair(current.first + error(inext, j), new_path));
          cell_done(inext, j + 1) = 1;
          std::push_heap(queue.begin(), queue.end());
        }
    }
  }
}

Array generate_mask(hmap::Vec2<int> shape, std::vector<int> cut_path_i, int ir)
{
  Array mask = Array(shape);

  // make sure there will be a minimum transition length in the mask
  // in order to avoid visible discontinuities
  int istart = std::min(ir, (int)(0.5f * shape.x));

  for (int i = istart; i < shape.x; i++)
    for (int j = 0; j < shape.y; j++)
      mask(i, j) = (i > cut_path_i[j]) ? 1.f : 0.f;

  // smoothing
  smooth_cpulse(mask, ir);

  // eventually make sure the mask amplitude, used for lerping, is
  // in [0, 1] on every horizontal line of the array
  for (int j = 0; j < shape.y; j++)
  {
    float vmin = std::numeric_limits<float>::max();
    float vmax = -std::numeric_limits<float>::max();
    for (int i = 0; i < shape.x; i++)
    {
      vmin = std::min(vmin, mask(i, j));
      vmax = std::max(vmax, mask(i, j));
    }

    for (int i = 0; i < shape.x; i++)
    {
      mask(i, j) -= vmin;
      mask(i, j) /= (vmax - vmin);
    }
  }

  return mask;
}

// --- quilting

Array quilting(Array          &array,
               hmap::Vec2<int> patch_base_shape,
               hmap::Vec2<int> tiling,
               float           overlap,
               uint            seed,
               float           filter_width_ratio)
{
  std::mt19937 gen(seed);

  hmap::Vec2<int> patch_shape = {(int)(patch_base_shape.x * (1.f + overlap)),
                                 (int)(patch_base_shape.y * (1.f + overlap))};

  hmap::Vec2<int> noverlap = {patch_shape.x - patch_base_shape.x,
                              patch_shape.y - patch_base_shape.y};

  hmap::Vec2<int> shape_output = {tiling.x * patch_base_shape.x + noverlap.x,
                                  tiling.y * patch_base_shape.y + noverlap.y};

  Array array_out = Array(shape_output);

  // smoothing radius for the patch transitions
  int ir = (int)(noverlap.x * filter_width_ratio);

  for (int jt = 0; jt < tiling.y; jt++)
  {
    int   j1 = jt * patch_base_shape.y; // tile start
    Array array_strip = Array(Vec2<int>(array_out.shape.x, patch_shape.y));

    // build up domain-wide horizontal strips
    for (int it = 0; it < tiling.x; it++)
    {
      int i1 = it * patch_base_shape.x;

      Array patch = get_random_patch(array, patch_shape, gen);

      if (it > 0)
      {
        Array error = Array(Vec2<int>(noverlap.x, patch_shape.y));
        for (int i = 0; i < noverlap.x; i++)
          for (int j = 0; j < patch_shape.y; j++)
            error(i, j) = std::abs(array_strip(i1 + i, j) - patch(i, j));

        // find cut path
        std::vector<int> cut_path_i;
        find_vertical_cut_path(error, cut_path_i);

        // define lerp factor
        Array mask = generate_mask(error.shape, cut_path_i, ir);

        for (int i = 0; i < noverlap.x; i++)
          for (int j = 0; j < patch_shape.y; j++)
            patch(i, j) = lerp(array_strip(i1 + i, j), patch(i, j), mask(i, j));
      }

      for (int i = 0; i < patch_shape.x; i++)
        for (int j = 0; j < patch_shape.y; j++)
          array_strip(i1 + i, j) = patch(i, j);
    }

    // patch the horizontal stripes
    if (jt > 0)
    {
      Array error = Array(Vec2<int>(shape_output.x, noverlap.y));

      for (int i = 0; i < shape_output.x; i++)
        for (int j = 0; j < noverlap.y; j++)
          error(i, j) = std::abs(array_strip(i, j) - array_out(i, j1 + j));

      Array mask = Array(error.shape);
      {
        Array            error_t = transpose(error);
        std::vector<int> cut_path_i;
        find_vertical_cut_path(error_t, cut_path_i);
        Array mask_t = generate_mask(error_t.shape, cut_path_i, ir);
        mask = transpose(mask_t);
      }

      for (int i = 0; i < shape_output.x; i++)
      {
        for (int j = 0; j < noverlap.y; j++)
          array_out(i, j1 + j) = lerp(array_out(i, j1 + j),
                                      array_strip(i, j),
                                      mask(i, j));

        for (int j = noverlap.y; j < patch_shape.y; j++)
          array_out(i, j1 + j) = array_strip(i, j);
      }
    }
    else
    {
      for (int i = 0; i < array_out.shape.x; i++)
        for (int j = 0; j < patch_shape.y; j++)
          array_out(i, j1 + j) = array_strip(i, j);
    }
  }

  return array_out;
}

Array quilting_expand(Array          &array,
                      float           expansion_ratio,
                      hmap::Vec2<int> patch_base_shape,
                      float           overlap,
                      uint            seed,
                      bool            keep_input_shape,
                      float           filter_width_ratio)
{
  expansion_ratio = std::max(1.f, expansion_ratio);

  if (keep_input_shape)
  {
    // output shape is the same as the output
    Vec2<int> work_shape = Vec2<int>((int)(array.shape.x / expansion_ratio),
                                     (int)(array.shape.y / expansion_ratio));

    Array array_work = array.resample_to_shape(work_shape);

    Vec2<int> patch_base_shape_work = Vec2<int>(
        (int)(patch_base_shape.x / expansion_ratio),
        (int)(patch_base_shape.y / expansion_ratio));

    Vec2<int> tiling = Vec2<int>(
        (int)(std::ceil(array.shape.x / patch_base_shape_work.x)),
        (int)(std::ceil(array.shape.y / patch_base_shape_work.y)));

    Array array_out = quilting(array_work,
                               patch_base_shape_work,
                               tiling,
                               overlap,
                               seed,
                               filter_width_ratio);

    return array_out.extract_slice(
        Vec4<int>(0, array.shape.x, 0, array.shape.y));
  }
  else
  {
    // output shape is also expanded according to expansion factor
    Vec2<int> expanded_shape = Vec2<int>(
        (int)(array.shape.x * expansion_ratio),
        (int)(array.shape.y * expansion_ratio));

    Vec2<int> tiling = Vec2<int>(
        (int)(std::ceil(expanded_shape.x / patch_base_shape.x)),
        (int)(std::ceil(expanded_shape.y / patch_base_shape.y)));

    Array array_out = quilting(array,
                               patch_base_shape,
                               tiling,
                               overlap,
                               seed,
                               filter_width_ratio);

    return array_out.extract_slice(
        Vec4<int>(0, expanded_shape.x, 0, expanded_shape.y));
  }
}

Array quilting_shuffle(Array          &array,
                       hmap::Vec2<int> patch_base_shape,
                       float           overlap,
                       uint            seed,
                       float           filter_width_ratio)
{
  Vec2<int> tiling = Vec2<int>(
      (int)(std::ceil(array.shape.x / patch_base_shape.x)),
      (int)(std::ceil(array.shape.y / patch_base_shape.y)));

  Array array_out = quilting(array,
                             patch_base_shape,
                             tiling,
                             overlap,
                             seed,
                             filter_width_ratio);

  // return an array with the same shape as the input
  return array_out.extract_slice(Vec4<int>(0, array.shape.x, 0, array.shape.y));
}

} // namespace hmap
