/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "macrologger.h"

#include "highmap/array3.hpp"

namespace hmap
{

Array3::Array3(Vec3<int> shape) : shape(shape)
{
  this->vector.clear();
  this->vector.resize(shape.x * shape.y * shape.z);
}

Array3::Array3(Vec2<int> shape_xy, int shape_z)
{
  this->shape = {shape_xy.x, shape_xy.y, shape_z};
  this->vector.clear();
  this->vector.resize(shape.x * shape.y * shape.z);
}

void Array3::set_slice(int k, const Array &slice)
{
  // TODO check shapes

  for (int i = 0; i < this->shape.x; i++)
    for (int j = 0; j < this->shape.y; j++)
      (*this)(i, j, k) = slice(i, j);
}

cv::Mat Array3::to_cv_mat()
{
  int cv_mat_type = CV_32FC1;

  if (this->shape.z == 2)
    cv_mat_type = CV_32FC2;
  else if (this->shape.z == 3)
    cv_mat_type = CV_32FC3;
  else if (this->shape.z == 4)
    cv_mat_type = CV_32FC4;

  cv::Mat mat = cv::Mat(this->shape.x,
                        this->shape.y,
                        cv_mat_type,
                        this->vector.data());

  if (this->shape.z == 3)
    cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
  else if (this->shape.z == 4)
    cv::cvtColor(mat, mat, cv::COLOR_BGRA2RGBA);

  return mat;
}

void Array3::to_png_8bit(std::string fname)
{
  cv::Mat mat = this->to_cv_mat();
  mat.convertTo(mat, CV_8U, 255);
  cv::rotate(mat, mat, cv::ROTATE_90_COUNTERCLOCKWISE);
  cv::imwrite(fname, mat);
}

void Array3::to_png_16bit(std::string fname)
{
  cv::Mat mat = this->to_cv_mat();
  mat.convertTo(mat, CV_16U, 65535);
  cv::rotate(mat, mat, cv::ROTATE_90_COUNTERCLOCKWISE);
  cv::imwrite(fname, mat);
}

} // namespace hmap
