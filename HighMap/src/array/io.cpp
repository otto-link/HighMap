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

void Array::from_file(const std::string &fname)
{
  LOG_DEBUG("reading binary file");
  std::ifstream f;
  f.open(fname, std::ios::binary);

  for (auto &v : this->vector)
    f.read(reinterpret_cast<char *>(&v), sizeof(float));
  f.close();
}

void Array::from_numpy(const std::string &fname)
{
  npy::npy_data d = npy::read_npy<float>(fname);

  // update array shape
  Vec2<int> new_shape = {(int)d.shape[0], (int)d.shape[1]};
  this->set_shape(new_shape);

  // copy the data
  if (d.fortran_order)
  {
    for (int j = 0; j < this->shape.y; j++)
      for (int i = 0; i < this->shape.x; i++)
      {
        int k = j * this->shape.x + i;
        (*this)(i, j) = d.data[k];
      }
  }
  else
  {
    for (int j = 0; j < this->shape.y; j++)
      for (int i = 0; i < this->shape.x; i++)
      {
        int k = i * this->shape.y + j;
        (*this)(i, j) = d.data[k];
      }
  }
}

void Array::infos(std::string msg) const
{
  float vmin = this->min();
  float vmax = this->max();

  std::cout << "Array: " << msg << "\n";
  std::cout << " - address: " << this << "\n";
  std::cout << " - shape: {" << this->shape.x << ", " << this->shape.y << "}"
            << "\n";
  std::cout << " - min: " << vmin << "\n";
  std::cout << " - max: " << vmax << "\n";
  std::cout << " - ptp: " << vmax - vmin << "\n";
}

void Array::print() const
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

void Array::to_exr(const std::string &fname) const
{
  Array array_copy = *this;
  remap(array_copy);

  cv::Mat mat = array_copy.to_cv_mat();

  std::vector<int> codec_params = {cv::IMWRITE_EXR_TYPE,
                                   cv::IMWRITE_EXR_TYPE_FLOAT,
                                   cv::IMWRITE_EXR_COMPRESSION,
                                   cv::IMWRITE_EXR_COMPRESSION_NO};

  cv::imwrite(fname, mat, codec_params);
}

void Array::to_file(const std::string &fname) const
{
  LOG_DEBUG("writing binary file");
  std::ofstream f;
  f.open(fname, std::ios::binary);

  for (auto &v : this->vector)
    f.write(reinterpret_cast<const char *>(&v), sizeof(float));

  f.close();
}

void Array::to_numpy(const std::string &fname) const
{
  npy::npy_data_ptr<float> d;
  d.data_ptr = this->vector.data();
  d.shape = {(uint)this->shape.x, (uint)this->shape.y};
  d.fortran_order = true;

  npy::write_npy(fname, d);
}

void Array::to_png(const std::string &fname,
                   int                cmap,
                   bool               hillshading,
                   int                depth) const
{
  Array       array_copy = *this;
  const float vmin = this->min();
  const float vmax = this->max();

  Tensor color3 =
      colorize(array_copy, vmin, vmax, cmap, hillshading, false, nullptr);
  color3.to_png(fname, depth);
}

void Array::to_png_grayscale(const std::string &fname, int depth) const
{
  Array array_copy = *this;
  remap(array_copy);

  cv::Mat mat = array_copy.to_cv_mat();
  int     scale_factor = (depth == CV_8U) ? 255 : 65535;
  mat.convertTo(mat, depth, scale_factor);
  cv::flip(mat, mat, 0); // flipud
  cv::imwrite(fname, mat);
}

void Array::to_raw_16bit(const std::string &fname) const
{
  write_raw_16bit(fname, *this);
}

void Array::to_tiff(const std::string &fname) const
{
  Array array_copy = *this;
  remap(array_copy);

  cv::Mat mat = array_copy.to_cv_mat();

  // set compression to cv::IMWRITE_TIFF_COMPRESSION_LZW (apparently
  // not available in openCV public header?)
  std::vector<int> codec_params = {cv::IMWRITE_TIFF_COMPRESSION, 5};

  cv::imwrite(fname, mat, codec_params);
}

} // namespace hmap
