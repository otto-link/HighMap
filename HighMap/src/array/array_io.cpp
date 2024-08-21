/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>
#include <iomanip>

#include "macrologger.h"
#include "npy.hpp"

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/export.hpp"
#include "highmap/range.hpp"

namespace hmap
{

void Array::from_file(std::string fname)
{
  LOG_DEBUG("reading binary file");
  std::ifstream f;
  f.open(fname, std::ios::binary);

  for (auto &v : this->vector)
    f.read(reinterpret_cast<char *>(&v), sizeof(float));
  f.close();
}

void Array::from_numpy(std::string fname)
{
  npy::npy_data d = npy::read_npy<float>(fname);

  // update array shape
  Vec2<int> new_shape = {(int)d.shape[0], (int)d.shape[1]};
  this->set_shape(new_shape);

  // copy the data
  if (d.fortran_order)
  {
    for (int i = 0; i < this->shape.x; i++)
      for (int j = 0; j < this->shape.y; j++)
      {
        int k = j * this->shape.x + i;
        (*this)(i, j) = d.data[k];
      }
  }
  else
  {
    for (int i = 0; i < this->shape.x; i++)
      for (int j = 0; j < this->shape.y; j++)
      {
        int k = i * this->shape.y + j;
        (*this)(i, j) = d.data[k];
      }
  }
}

void Array::infos(std::string msg) const
{
  std::cout << "Array: " << msg << " ";
  std::cout << "address: " << this << ", ";
  std::cout << "shape: {" << this->shape.x << ", " << this->shape.y << "}"
            << ", ";
  std::cout << "min: " << this->min() << ", ";
  std::cout << "max: " << this->max();
  std::cout << std::endl;
}

void Array::print()
{
  std::cout << std::fixed << std::setprecision(6) << std::setfill('0');
  for (int j = shape.y - 1; j > -1; j--)
  {
    for (int i = 0; i < shape.x; i++)
    {
      std::cout << std::setw(5) << (*this)(i, j) << " ";
    }
    std::cout << std::endl;
  }
}

void Array::to_exr(std::string fname)
{
  Array array_copy = *this;
  remap(array_copy);

  cv::Mat mat = hmap::array_to_cv_mat(array_copy);
  cv::rotate(mat, mat, cv::ROTATE_90_COUNTERCLOCKWISE);

  std::vector<int> codec_params = {cv::IMWRITE_EXR_TYPE,
                                   cv::IMWRITE_EXR_TYPE_FLOAT,
                                   cv::IMWRITE_EXR_COMPRESSION,
                                   cv::IMWRITE_EXR_COMPRESSION_NO};

  cv::imwrite(fname, mat, codec_params);
}

void Array::to_file(std::string fname)
{
  LOG_DEBUG("writing binary file");
  std::ofstream f;
  f.open(fname, std::ios::binary);

  for (auto &v : this->vector)
    f.write(reinterpret_cast<const char *>(&v), sizeof(float));

  f.close();
}

void Array::to_numpy(std::string fname)
{
  npy::npy_data_ptr<float> d;
  d.data_ptr = this->vector.data();
  d.shape = {(uint)this->shape.x, (uint)this->shape.y};
  d.fortran_order = false;

  npy::write_npy(fname, d);
}

void Array::to_png(std::string fname, int cmap, bool hillshading)
{
  const float vmin = this->min();
  const float vmax = this->max();

  Array3 color3 =
      colorize(*this, vmin, vmax, cmap, hillshading, false, nullptr);
  color3.to_png_8bit(fname);
}

void Array::to_png_grayscale_8bit(std::string fname)
{
  Array array_copy = *this;
  remap(array_copy);

  cv::Mat mat = hmap::array_to_cv_mat(array_copy);
  cv::rotate(mat, mat, cv::ROTATE_90_COUNTERCLOCKWISE);
  mat.convertTo(mat, CV_8U, 255);
  cv::imwrite(fname, mat);
}

void Array::to_png_grayscale_16bit(std::string fname)
{
  Array array_copy = *this;
  remap(array_copy);

  cv::Mat mat = hmap::array_to_cv_mat(array_copy);
  cv::rotate(mat, mat, cv::ROTATE_90_COUNTERCLOCKWISE);
  mat.convertTo(mat, CV_16U, 65535);
  cv::imwrite(fname, mat);
}

void Array::to_raw_16bit(std::string fname)
{
  write_raw_16bit(fname, *this);
}

void Array::to_tiff(std::string fname)
{
  Array array_copy = *this;
  remap(array_copy);

  cv::Mat mat = hmap::array_to_cv_mat(array_copy);
  cv::rotate(mat, mat, cv::ROTATE_90_COUNTERCLOCKWISE);

  // set compression to cv::IMWRITE_TIFF_COMPRESSION_LZW (apparently
  // not available in openCV public header?)
  std::vector<int> codec_params = {cv::IMWRITE_TIFF_COMPRESSION, 5};

  cv::imwrite(fname, mat, codec_params);
}

} // namespace hmap
