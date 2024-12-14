/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/range.hpp"

namespace hmap
{

cv::Mat Array::to_cv_mat()
{
  return cv::Mat(this->shape.y, this->shape.x, CV_32FC1, this->vector.data());
}

// Helper function to convert OpenCV matrix to Array
template <typename T>
void convert_mat_to_array(const cv::Mat &mat, Array &array)
{
  for (int j = 0; j < array.shape.y; ++j)
    for (int i = 0; i < array.shape.x; ++i)
      array(i, j) = static_cast<float>(mat.at<T>(j, i));
}

Array cv_mat_to_array(const cv::Mat &mat, bool remap_values)
{
  Vec2<int> shape = {mat.cols, mat.rows};
  Array     array(shape);

  switch (mat.type())
  {
  case CV_8U: convert_mat_to_array<uint8_t>(mat, array); break;
  case CV_8S: convert_mat_to_array<int8_t>(mat, array); break;
  case CV_16U: convert_mat_to_array<uint16_t>(mat, array); break;
  case CV_16S: convert_mat_to_array<int16_t>(mat, array); break;
  case CV_32S: convert_mat_to_array<int>(mat, array); break;
  case CV_32F: convert_mat_to_array<float>(mat, array); break;
  case CV_64F: convert_mat_to_array<double>(mat, array); break;
  default: throw std::invalid_argument("Unsupported matrix type.");
  }

  if (remap_values) remap(array);

  return array;
}

} // namespace hmap
