/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/math.hpp"
#include "highmap/operator.hpp"
#include "highmap/transform.hpp"

namespace hmap
{

Array quilting(const std::vector<const Array *> &p_arrays,
               hmap::Vec2<int>                   patch_base_shape,
               hmap::Vec2<int>                   tiling,
               float                             overlap,
               uint                              seed,
               std::vector<Array *>              secondary_arrays,
               bool                              patch_flip,
               bool                              patch_rotate,
               bool                              patch_transpose,
               float                             filter_width_ratio)
{
  std::mt19937 gen(seed);

  std::uniform_int_distribution<int> dis_a(0, (int)p_arrays.size() - 1);

  hmap::Vec2<int> patch_shape = {(int)(patch_base_shape.x * (1.f + overlap)),
                                 (int)(patch_base_shape.y * (1.f + overlap))};

  hmap::Vec2<int> noverlap = {patch_shape.x - patch_base_shape.x,
                              patch_shape.y - patch_base_shape.y};

  hmap::Vec2<int> shape_output = {tiling.x * patch_base_shape.x + noverlap.x,
                                  tiling.y * patch_base_shape.y + noverlap.y};

  Array array_out = Array(shape_output);

  // reserve outputs for secondary arrays transformed using primary
  // array(s) as guide(s)
  std::vector<Array> secondary_arrays_output(secondary_arrays.size(),
                                             shape_output);

  // smoothing radius for the patch transitions
  int ir = (int)(noverlap.x * filter_width_ratio);

  for (int jt = 0; jt < tiling.y; jt++)
  {
    int   j1 = jt * patch_base_shape.y; // tile start
    Array array_strip = Array(Vec2<int>(array_out.shape.x, patch_shape.y));

    std::vector<Array> secondary_arrays_strips(secondary_arrays_output.size(),
                                               array_strip);

    // build up domain-wide horizontal strips
    for (int it = 0; it < tiling.x; it++)
    {
      int i1 = it * patch_base_shape.x;

      int array_idx = dis_a(gen);

      // used only if secondary_arrays is not empty
      std::vector<Array> secondary_patches = {};

      Array patch = get_random_patch(*p_arrays[array_idx],
                                     patch_shape,
                                     gen,
                                     patch_flip,
                                     patch_rotate,
                                     patch_transpose,
                                     &secondary_arrays,
                                     &secondary_patches);

      if (it > 0)
      {
        Array error = Array(Vec2<int>(noverlap.x, patch_shape.y));
        for (int j = 0; j < patch_shape.y; j++)
          for (int i = 0; i < noverlap.x; i++)
            error(i, j) = std::abs(array_strip(i1 + i, j) - patch(i, j));

        // find cut path
        std::vector<int> cut_path_i;
        find_vertical_cut_path(error, cut_path_i);

        // define lerp factor
        Array mask = generate_mask(error.shape, cut_path_i, ir);

        // primary patches
        for (int j = 0; j < patch_shape.y; j++)
          for (int i = 0; i < noverlap.x; i++)
            patch(i, j) = lerp(array_strip(i1 + i, j), patch(i, j), mask(i, j));

        // same for secondary patches
        for (size_t k = 0; k < secondary_patches.size(); k++)
        {
          for (int j = 0; j < patch_shape.y; j++)
            for (int i = 0; i < noverlap.x; i++)
              secondary_patches[k](i, j) = lerp(
                  secondary_arrays_strips[k](i1 + i, j),
                  secondary_patches[k](i, j),
                  mask(i, j));
        }
      }

      // primary array
      for (int j = 0; j < patch_shape.y; j++)
        for (int i = 0; i < patch_shape.x; i++)
          array_strip(i1 + i, j) = patch(i, j);

      // apply the same patching to the secondary arrays
      for (size_t k = 0; k < secondary_arrays_output.size(); k++)
      {
        for (int j = 0; j < patch_shape.y; j++)
          for (int i = 0; i < patch_shape.x; i++)
            secondary_arrays_strips[k](i1 + i, j) = secondary_patches[k](i, j);
      }
    }

    // patch the horizontal stripes
    if (jt > 0)
    {
      Array error = Array(Vec2<int>(shape_output.x, noverlap.y));

      for (int j = 0; j < noverlap.y; j++)
        for (int i = 0; i < shape_output.x; i++)
          error(i, j) = std::abs(array_strip(i, j) - array_out(i, j1 + j));

      Array mask = Array(error.shape);
      {
        Array            error_t = transpose(error);
        std::vector<int> cut_path_i;
        find_vertical_cut_path(error_t, cut_path_i);
        Array mask_t = generate_mask(error_t.shape, cut_path_i, ir);
        mask = transpose(mask_t);
      }

      // primary array
      for (int i = 0; i < shape_output.x; i++)
      {
        for (int j = 0; j < noverlap.y; j++)
          array_out(i, j1 + j) = lerp(array_out(i, j1 + j),
                                      array_strip(i, j),
                                      mask(i, j));

        for (int j = noverlap.y; j < patch_shape.y; j++)
          array_out(i, j1 + j) = array_strip(i, j);
      }

      // apply the same patching to the secondary arrays
      for (size_t k = 0; k < secondary_arrays_output.size(); k++)
      {
        for (int i = 0; i < shape_output.x; i++)
        {
          for (int j = 0; j < noverlap.y; j++)
            secondary_arrays_output[k](i, j1 + j) = lerp(
                secondary_arrays_output[k](i, j1 + j),
                secondary_arrays_strips[k](i, j),
                mask(i, j));

          for (int j = noverlap.y; j < patch_shape.y; j++)
            secondary_arrays_output[k](i,
                                       j1 + j) = secondary_arrays_strips[k](i,
                                                                            j);
        }
      }
    }
    else
    {
      // primary array
      for (int j = 0; j < patch_shape.y; j++)
        for (int i = 0; i < array_out.shape.x; i++)
          array_out(i, j1 + j) = array_strip(i, j);

      // apply the same patching to the secondary arrays
      for (size_t k = 0; k < secondary_arrays_output.size(); k++)
      {
        for (int j = 0; j < patch_shape.y; j++)
          for (int i = 0; i < array_out.shape.x; i++)
            secondary_arrays_output[k](i,
                                       j1 + j) = secondary_arrays_strips[k](i,
                                                                            j);
      }
    }
  }

  // override content of input secondary arrays with output
  for (size_t k = 0; k < secondary_arrays_output.size(); k++)
    *secondary_arrays[k] = secondary_arrays_output[k];

  return array_out;
}

Array quilting_blend(const std::vector<const Array *> &p_arrays,
                     hmap::Vec2<int>                   patch_base_shape,
                     float                             overlap,
                     uint                              seed,
                     bool                              patch_flip,
                     bool                              patch_rotate,
                     bool                              patch_transpose,
                     float                             filter_width_ratio)
{
  Vec2<int> shape = p_arrays.back()->shape;

  Vec2<int> tiling = Vec2<int>((int)(std::ceil(shape.x / patch_base_shape.x)),
                               (int)(std::ceil(shape.y / patch_base_shape.y)));

  Array array_out = quilting(p_arrays,
                             patch_base_shape,
                             tiling,
                             overlap,
                             seed,
                             {}, // no secondary arrays
                             patch_flip,
                             patch_rotate,
                             patch_transpose,
                             filter_width_ratio);

  // return an array with the same shape as the input
  return array_out.extract_slice(Vec4<int>(0, shape.x, 0, shape.y));
}

Array quilting_expand(const Array         &array,
                      float                expansion_ratio,
                      hmap::Vec2<int>      patch_base_shape,
                      float                overlap,
                      uint                 seed,
                      std::vector<Array *> secondary_arrays,
                      bool                 keep_input_shape,
                      bool                 patch_flip,
                      bool                 patch_rotate,
                      bool                 patch_transpose,
                      float                filter_width_ratio)
{
  expansion_ratio = std::max(1.f, expansion_ratio);

  if (keep_input_shape)
  {
    // output shape is the same as the output
    Vec2<int> work_shape = Vec2<int>((int)(array.shape.x / expansion_ratio),
                                     (int)(array.shape.y / expansion_ratio));

    Array array_work = array.resample_to_shape(work_shape);

    // secondary arrays
    std::vector<Array>   secondary_arrays_storage = {};
    std::vector<Array *> secondary_arrays_work = {};

    for (auto v : secondary_arrays)
      secondary_arrays_storage.push_back(v->resample_to_shape(work_shape));

    for (auto &v : secondary_arrays_storage)
      secondary_arrays_work.push_back(&v);

    // apply
    Vec2<int> patch_base_shape_work = Vec2<int>(
        (int)(patch_base_shape.x / expansion_ratio),
        (int)(patch_base_shape.y / expansion_ratio));

    Vec2<int> tiling = Vec2<int>(
        (int)(std::ceil(array.shape.x / patch_base_shape_work.x)),
        (int)(std::ceil(array.shape.y / patch_base_shape_work.y)));

    Array array_out = quilting({&array_work},
                               patch_base_shape_work,
                               tiling,
                               overlap,
                               seed,
                               secondary_arrays_work,
                               patch_flip,
                               patch_rotate,
                               patch_transpose,
                               filter_width_ratio);

    // override p_secondary_arrays content with output
    for (size_t k = 0; k < secondary_arrays.size(); k++)
      *secondary_arrays[k] = secondary_arrays_work[k]->extract_slice(
          Vec4<int>(0, array.shape.x, 0, array.shape.y));

    // return an array with the same shape as the input
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

    std::vector<const Array *> input = {&array};

    Array array_out = quilting(input,
                               patch_base_shape,
                               tiling,
                               overlap,
                               seed,
                               secondary_arrays,
                               patch_flip,
                               patch_rotate,
                               patch_transpose,
                               filter_width_ratio);

    // override p_secondary_arrays content with output
    for (auto v : secondary_arrays)
      *v = v->extract_slice(
          Vec4<int>(0, expanded_shape.x, 0, expanded_shape.y));

    // return an array with the expanded shape
    return array_out.extract_slice(
        Vec4<int>(0, expanded_shape.x, 0, expanded_shape.y));
  }
}

Array quilting_shuffle(const Array         &array,
                       hmap::Vec2<int>      patch_base_shape,
                       float                overlap,
                       uint                 seed,
                       std::vector<Array *> secondary_arrays,
                       bool                 patch_flip,
                       bool                 patch_rotate,
                       bool                 patch_transpose,
                       float                filter_width_ratio)
{
  Vec2<int> tiling = Vec2<int>(
      (int)(std::ceil(array.shape.x / patch_base_shape.x)),
      (int)(std::ceil(array.shape.y / patch_base_shape.y)));

  std::vector<const Array *> input = {&array};

  Array array_out = quilting(input,
                             patch_base_shape,
                             tiling,
                             overlap,
                             seed,
                             secondary_arrays,
                             patch_flip,
                             patch_rotate,
                             patch_transpose,
                             filter_width_ratio);

  // override p_secondary_arrays content with output
  for (auto v : secondary_arrays)
    *v = v->extract_slice(Vec4<int>(0, array.shape.x, 0, array.shape.y));

  // return an array with the same shape as the input
  return array_out.extract_slice(Vec4<int>(0, array.shape.x, 0, array.shape.y));
}

} // namespace hmap
