/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/range.hpp"

namespace hmap
{

cv::Mat array_to_cv_mat(Array &array)
{
  return cv::Mat(array.shape.x, array.shape.y, CV_32FC1, array.vector.data());
}

Array cv_mat_to_array(cv::Mat &mat, bool remap_values)
{
  Vec2<int> shape = {mat.size().height, mat.size().width};
  Array     array(shape);

  switch (mat.type())
  {
  case (CV_8U):
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = (float)mat.at<uint8_t>(i, j);
    break;

  case (CV_8S):
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = (float)mat.at<int8_t>(i, j);
    break;

  case (CV_16U):
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = (float)mat.at<uint16_t>(i, j);
    break;

  case (CV_16S):
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = (float)mat.at<int16_t>(i, j);
    break;

  case (CV_32S):
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = (float)mat.at<int>(i, j);
    break;

  case (CV_32F):
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = mat.at<float>(i, j);
    break;

  case (CV_64F):
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = (float)mat.at<double>(i, j);
    break;
  }

  if (remap_values)
    remap(array);

  return array;
}

} // namespace hmap
