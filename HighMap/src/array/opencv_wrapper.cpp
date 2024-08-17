/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#ifdef ENABLE_OPENCV

#include "macrologger.h"

#include "highmap/array.hpp"

namespace hmap
{

cv::Mat array_to_cv_mat(Array &array)
{
  return cv::Mat(array.shape.x, array.shape.y, CV_32FC1, array.vector.data());
}

} // namespace hmap

#endif
