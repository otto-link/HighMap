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

float &Tensor::operator()(int i, int j, int k)
{
  return this->vector[(i * this->shape.y + j) * this->shape.z + k];
}

const float &Tensor::operator()(int i, int j, int k) const ///< @overload
{
  return this->vector[(i * this->shape.y + j) * this->shape.z + k];
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

void Tensor::set_slice(int k, const Array &slice)
{
  // TODO check shapes

  for (int i = 0; i < this->shape.x; i++)
    for (int j = 0; j < this->shape.y; j++)
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

  cv::Mat mat(shape.x, shape.y, cv_mat_type, vector.data());

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
  cv::rotate(mat, mat, cv::ROTATE_90_COUNTERCLOCKWISE);
  cv::imwrite(fname, mat);
}

std::vector<uint8_t> Tensor::to_img_8bit()
{
  std::vector<uint8_t> vec;
  vec.reserve(this->vector.size());

  for (int j = this->shape.y - 1; j > -1; j--)
    for (int i = 0; i < this->shape.x; i++)
      for (int k = 0; k < this->shape.z; k++)
        vec.push_back(static_cast<uint8_t>(255.f * (*this)(i, j, k)));

  return vec;
}

} // namespace hmap
