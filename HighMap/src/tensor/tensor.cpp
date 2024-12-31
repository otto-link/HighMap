/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "macrologger.h"

#include "highmap/tensor.hpp"

namespace hmap
{

Tensor::Tensor(Vec3<int> shape) : shape(shape)
{
  this->vector.clear();
  this->vector.resize(shape.x * shape.y * shape.z);
}

Tensor::Tensor(Vec2<int> shape_xy, int shape_z)
{
  this->shape = {shape_xy.x, shape_xy.y, shape_z};
  this->vector.clear();
  this->vector.resize(shape.x * shape.y * shape.z);
}

Tensor::Tensor(const std::string &fname)
{
  cv::Mat mat = cv::imread(fname, cv::IMREAD_COLOR);
  cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
  mat.convertTo(mat, CV_32FC3, 1.f / 255.f);

  // RGBA image
  *this = Tensor(Vec2<int>(mat.cols, mat.rows), 4);

  // fill tensor
  for (int j = 0; j < shape.y; j++)
    for (int i = 0; i < shape.x; i++)
    {
      // OpenCV stores pixels as (row, column), hence (mat.rows - 1 - j, i)
      cv::Vec3f pixel = mat.at<cv::Vec3f>(mat.rows - 1 - j, i);

      // assign RGB values to the tensor
      (*this)(i, j, 0) = pixel[0]; // red
      (*this)(i, j, 1) = pixel[1]; // green
      (*this)(i, j, 2) = pixel[2]; // blue

      // set alpha channel to 1.0
      (*this)(i, j, 3) = 1.f;
    }
}

float &Tensor::operator()(int i, int j, int k)
{
  return this->vector[(j * this->shape.x + i) * this->shape.z + k];
}

const float &Tensor::operator()(int i, int j, int k) const ///< @overload
{
  return this->vector[(j * this->shape.x + i) * this->shape.z + k];
}

Array Tensor::get_slice(int k) const
{
  Array out = Array(Vec2<int>(this->shape.x, this->shape.y));

  for (int j = 0; j < this->shape.y; j++)
    for (int i = 0; i < this->shape.x; i++)
      out(i, j) = (*this)(i, j, k);

  return out;
}

float Tensor::max() const
{
  return *std::max_element(this->vector.begin(), this->vector.end());
}

float Tensor::min() const
{
  return *std::min_element(this->vector.begin(), this->vector.end());
};

void Tensor::remap(float vmin, float vmax)
{
  float min = this->min();
  float max = this->max();

  if (min != max)
    for (auto &v : this->vector)
      v = (v - min) / (max - min) * (vmax - vmin) + vmin;
  else
    for (auto &v : this->vector)
      v = vmin;
}

Tensor Tensor::resample_to_shape_xy(Vec2<int> new_shape_xy)
{
  Tensor out = Tensor(new_shape_xy, this->shape.z);

  for (int k = 0; k < this->shape.z; k++)
  {
    Array slice = this->get_slice(k);
    out.set_slice(k, slice.resample_to_shape(new_shape_xy));
  }

  return out;
}

void Tensor::set_slice(int k, const Array &slice)
{
  // TODO check shapes

  for (int j = 0; j < this->shape.y; j++)
    for (int i = 0; i < this->shape.x; i++)
      (*this)(i, j, k) = slice(i, j);
}

cv::Mat Tensor::to_cv_mat()
{
  int cv_mat_type;
  switch (shape.z)
  {
  case 2: cv_mat_type = CV_32FC2; break;
  case 3: cv_mat_type = CV_32FC3; break;
  case 4: cv_mat_type = CV_32FC4; break;
  default: cv_mat_type = CV_32FC1; break;
  }

  cv::Mat mat(shape.y, shape.x, cv_mat_type, vector.data());

  if (shape.z == 3)
    cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
  else if (shape.z == 4)
    cv::cvtColor(mat, mat, cv::COLOR_BGRA2RGBA);

  return mat;
}

void Tensor::to_png(const std::string &fname, int depth)
{
  cv::Mat mat = to_cv_mat();
  int     scale_factor = (depth == CV_8U) ? 255 : 65535;
  mat.convertTo(mat, depth, scale_factor);
  cv::flip(mat, mat, 0); // up-down
  cv::imwrite(fname, mat);
}

std::vector<uint8_t> Tensor::to_img_8bit()
{
  std::vector<uint8_t> vec;
  vec.reserve(this->vector.size());

  for (int j = this->shape.y - 1; j >= 0; j--)
    for (int i = 0; i < this->shape.x; i++)
      for (int k = 0; k < this->shape.z; k++)
        vec.push_back(static_cast<uint8_t>(255.f * (*this)(i, j, k)));

  return vec;
}

} // namespace hmap
