/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/authoring.hpp"
#include "highmap/geometry.hpp"
#include "highmap/op.hpp"

#include "op/vector_utils.hpp"

namespace hmap
{

Array stamping(Vec2<int>           shape,
               std::vector<float>  xr,
               std::vector<float>  yr,
               std::vector<float>  zr,
               Array               kernel,
               float               kernel_radius,
               bool                kernel_scale_radius,
               bool                kernel_scale_amplitude,
               StampingBlendMethod blend_method,
               uint                seed,
               bool                kernel_flip,
               bool                kernel_rotate,
               Vec4<float>         bbox_array)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0.f, 1.f);

  Array array = Array(shape);

  // normalized node coordinates
  std::vector<float> xrs = xr;
  std::vector<float> yrs = yr;
  rescale_grid_to_unit_square(xrs, yrs, bbox_array);

  // --- define blending function

  std::function<void(float &, float &)> blend_function;

  float k_smoothing = 0.1f;

  switch (blend_method)
  {
    // 'va': value array, 'vk': value kernel

  case StampingBlendMethod::ADD:
    blend_function = [](float &va, float &vk) { va += vk; };
    break;

  case StampingBlendMethod::MAXIMUM:
    blend_function = [](float &va, float &vk) { va = std::max(va, vk); };
    break;

  case StampingBlendMethod::MAXIMUM_SMOOTH:
    blend_function = [&k_smoothing](float &va, float &vk)
    { va = maximum_smooth(va, vk, k_smoothing); };
    break;

  case StampingBlendMethod::MINIMUM:
    blend_function = [](float &va, float &vk) { va = std::min(va, vk); };
    break;

  case StampingBlendMethod::MINIMUM_SMOOTH:
    blend_function = [&k_smoothing](float &va, float &vk)
    { va = minimum_smooth(va, vk, k_smoothing); };
    break;

  case StampingBlendMethod::MULTIPLY:
    blend_function = [](float &va, float &vk) { va *= vk; };
    break;

  case StampingBlendMethod::SUBSTRACT:
    blend_function = [](float &va, float &vk) { va -= vk; };
    break;
  }

  // --- do the stamping

  // scale kernel
  Vec2<int> scaled_kernel_shape = {std::max(2, (int)(kernel_radius * shape.x)),
                                   std::max(2, (int)(kernel_radius * shape.y))};

  Array kernel_scaled = kernel.resample_to_shape(scaled_kernel_shape);

  // sort points by value to limit the number of a kernel resizing
  std::vector<size_t> ki = argsort(zr);

  for (size_t k : ki)
  {

    // resize kernel (if requested)
    if (kernel_scale_radius)
    {
      float coeff = zr[k] * kernel_radius;

      Vec2<int> new_scaled_kernel_shape = {std::max(2, (int)(coeff * shape.x)),
                                           std::max(2, (int)(coeff * shape.y))};

      if (new_scaled_kernel_shape != scaled_kernel_shape)
      {
        scaled_kernel_shape = new_scaled_kernel_shape;
        kernel_scaled = kernel.resample_to_shape(scaled_kernel_shape);
      }
    }

    Array kernel_local = kernel_scaled;

    // transform
    if (kernel_flip)
    {
      if (dis(gen) > 0.5f)
        flip_ud(kernel_local);
      if (dis(gen) > 0.5f)
        flip_lr(kernel_local);
      if (dis(gen) > 0.5f)
        rot90(kernel_local);
      if (dis(gen) > 0.5f)
        kernel_local = transpose(kernel_local);
    }

    // any rotation angle, time consuming and add some scaling
    // distortions to the input kernel
    if (kernel_rotate)
      rotate(kernel_local, 360.f * dis(gen), true);

    // amplitude
    if (kernel_scale_amplitude)
      kernel_local *= zr[k];

    // blend (center kernel on point)
    int i0 = (int)(xrs[k] * (shape.x - 1));
    int j0 = (int)(yrs[k] * (shape.y - 1));

    i0 -= (int)(0.5f * scaled_kernel_shape.x);
    j0 -= (int)(0.5f * scaled_kernel_shape.y);

    for (int i = 0; i < scaled_kernel_shape.x; i++)
      for (int j = 0; j < scaled_kernel_shape.y; j++)
      {
        if (i0 + i >= 0 && i0 + i < shape.x && j0 + j >= 0 && j0 + j < shape.y)
          blend_function(array(i0 + i, j0 + j), kernel_local(i, j));
      }
  }

  return array;
}

} // namespace hmap
