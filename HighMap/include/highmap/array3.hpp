/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file array_vec3.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file for the ArrayVec class.
 *
 * @version 0.1
 * @date 2023-05-07
 *
 * @copyright Copyright (c) 2023 Otto Link
 *
 */
#pragma once
#include <opencv2/core/mat.hpp>

#include "highmap/algebra.hpp"
#include "highmap/array.hpp"

namespace hmap
{

class Array3
{
public:
  Vec3<int>          shape;
  std::vector<float> vector;

  Array3(Vec3<int> shape);

  Array3(Vec2<int> shape_xy, int shape_z);

  float &operator()(int i, int j, int k)
  {
    return this->vector[(i * this->shape.y + j) * this->shape.z + k];
  }

  const float &operator()(int i, int j, int k) const ///< @overload
  {
    return this->vector[(i * this->shape.y + j) * this->shape.z + k];
  }

  void set_slice(int k, const Array &slice);

  cv::Mat to_cv_mat();

  void to_png_8bit(std::string fname);

  void to_png_16bit(std::string fname);

  std::vector<uint8_t> to_img_8bit();
};

} // namespace hmap
