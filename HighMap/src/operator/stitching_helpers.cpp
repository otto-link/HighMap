/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/filters.hpp"
#include "highmap/transform.hpp"

namespace hmap
{

bool cmp_path(std::pair<float, std::vector<int>> &a,
              std::pair<float, std::vector<int>> &b)
{
  return a.first < b.first;
}

// among all the possible cut path from top to bottom and find the one
// with the minimum cost using Dijkstra's algorithm
void find_vertical_cut_path(const Array &error, std::vector<int> &path_i)
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

  for (int j = 0; j < shape.y; j++)
    for (int i = istart; i < shape.x; i++)
      mask(i, j) = (i > cut_path_i[j]) ? 1.f : 0.f;

  for (int j = 0; j < shape.y; j++)
    mask(shape.x - 1, j) = 1.f;

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

    if (vmin != vmax)
      for (int i = 0; i < shape.x; i++)
      {
        mask(i, j) -= vmin;
        mask(i, j) /= (vmax - vmin);
      }
  }

  return mask;
}

void helper_flip_rot_transpose(Array &array,
                               bool   do_flip_ud,
                               bool   do_flip_lr,
                               bool   do_rot90,
                               bool   do_transpose)
{
  if (do_flip_ud) flip_ud(array);
  if (do_flip_lr) flip_lr(array);

  // square patches only...
  if (array.shape.x == array.shape.y)
  {
    if (do_rot90) rot90(array);
    if (do_transpose) transpose(array);
  }
}

Array get_random_patch(const Array          &array,
                       hmap::Vec2<int>       patch_shape,
                       std::mt19937         &gen,
                       bool                  patch_flip,
                       bool                  patch_rotate,
                       bool                  patch_transpose,
                       std::vector<Array *> *p_secondary_arrays,
                       std::vector<Array>   *p_secondary_patches)
{
  std::uniform_int_distribution<int> dis_i(0,
                                           array.shape.x - 2 - patch_shape.x);
  std::uniform_int_distribution<int> dis_j(0,
                                           array.shape.y - 2 - patch_shape.y);

  // random pair of indices
  int i = dis_i(gen);
  int j = dis_j(gen);

  Array patch = array.extract_slice(
      Vec4<int>(i, i + patch_shape.x, j, j + patch_shape.y));

  // flipping, etc...
  int imid = (int)(0.5f * (array.shape.x - 1 - patch_shape.x));

  bool do_flip_ud = patch_flip && (dis_i(gen) > imid);
  bool do_flip_lr = patch_flip && (dis_i(gen) > imid);
  bool do_rot90 = patch_rotate && (dis_i(gen) > imid);
  bool do_transpose = patch_transpose && (dis_i(gen) > imid);

  helper_flip_rot_transpose(patch,
                            do_flip_ud,
                            do_flip_lr,
                            do_rot90,
                            do_transpose);

  // apply the patch extraction with the same parameters to the
  // secondary arrays
  if (p_secondary_arrays && p_secondary_patches)
  {
    p_secondary_patches->clear();

    for (auto pa : *p_secondary_arrays)
    {
      Array sec_patch = pa->extract_slice(
          Vec4<int>(i, i + patch_shape.x, j, j + patch_shape.y));
      helper_flip_rot_transpose(sec_patch,
                                do_flip_ud,
                                do_flip_lr,
                                do_rot90,
                                do_transpose);
      p_secondary_patches->push_back(sec_patch);
    }
  }

  return patch;
}

} // namespace hmap
