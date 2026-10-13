/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <algorithm>
#include <cmath>
#include <complex>
#include <vector>

#include "highmap/boundary.hpp"
#include "highmap/internal/validation.hpp"
#include "highmap/logger.hpp"
#include "highmap/math/fft.hpp"

#include "pocketfft_hdronly.h"

#include <omp.h>

namespace hmap
{

namespace
{

size_t get_num_threads()
{
  int n = omp_get_max_threads();
  return (n > 0) ? static_cast<size_t>(n) : 1;
}

} // namespace

// --- FFT size

int fft_good_size(int n)
{
  if (n <= 1) return 1;
  return static_cast<int>(
      pocketfft::detail::util::good_size_real(static_cast<size_t>(n)));
}

// --- FFT 2D Real-to-Complex

std::vector<std::complex<float>> fft2d_r2c(const Array &array)
{
  if (!validate_non_empty(array)) return {};

  const size_t nx = static_cast<size_t>(array.shape.x);
  const size_t ny = static_cast<size_t>(array.shape.y);
  const size_t half_nx = nx / 2 + 1;

  pocketfft::shape_t  shape_in = {ny, nx};
  pocketfft::stride_t stride_in = {static_cast<ptrdiff_t>(nx * sizeof(float)),
                                   static_cast<ptrdiff_t>(sizeof(float))};

  pocketfft::stride_t stride_out = {
      static_cast<ptrdiff_t>(half_nx * sizeof(std::complex<float>)),
      static_cast<ptrdiff_t>(sizeof(std::complex<float>))};

  pocketfft::shape_t axes = {0, 1};

  std::vector<std::complex<float>> spectrum(ny * half_nx);

  pocketfft::r2c<float>(shape_in,
                        stride_in,
                        stride_out,
                        axes,
                        pocketfft::FORWARD,
                        array.vector.data(),
                        spectrum.data(),
                        1.0f,
                        get_num_threads());

  return spectrum;
}

// --- FFT 2D Complex-to-Real

Array fft2d_c2r(const std::vector<std::complex<float>> &spectrum,
                glm::ivec2                              shape_out,
                bool                                    normalize)
{
  if (shape_out.x <= 0 || shape_out.y <= 0) return Array();

  const size_t nx = static_cast<size_t>(shape_out.x);
  const size_t ny = static_cast<size_t>(shape_out.y);
  const size_t half_nx = nx / 2 + 1;

  if (spectrum.size() < ny * half_nx)
  {
    hmap::log::error(
        "fft2d_c2r: spectrum size ({}) is smaller than expected ({})",
        spectrum.size(),
        ny * half_nx);
    return Array();
  }

  Array result(shape_out);

  pocketfft::shape_t  shape = {ny, nx};
  pocketfft::stride_t stride_in = {
      static_cast<ptrdiff_t>(half_nx * sizeof(std::complex<float>)),
      static_cast<ptrdiff_t>(sizeof(std::complex<float>))};

  pocketfft::stride_t stride_out = {static_cast<ptrdiff_t>(nx * sizeof(float)),
                                    static_cast<ptrdiff_t>(sizeof(float))};

  pocketfft::shape_t axes = {0, 1};

  const float factor = normalize ? (1.0f / static_cast<float>(nx * ny)) : 1.0f;

  pocketfft::c2r<float>(shape,
                        stride_in,
                        stride_out,
                        axes,
                        pocketfft::BACKWARD,
                        spectrum.data(),
                        result.vector.data(),
                        factor,
                        get_num_threads());

  return result;
}

// --- FFT 2D Convolution

Array convolve2d_fft(const Array &array, const Array &kernel)
{
  if (!validate_non_empty(array)) return Array();
  if (!validate_non_empty(kernel)) return Array(array.shape);

  const int i1 = static_cast<int>(
      std::ceil(0.5f * static_cast<float>(kernel.shape.x)));
  const int i2 = kernel.shape.x - i1;
  const int j1 = static_cast<int>(
      std::ceil(0.5f * static_cast<float>(kernel.shape.y)));
  const int j2 = kernel.shape.y - j1;

  Array array_buffered = generate_buffered_array(array, {i1, i2, j1, j2});
  return convolve2d_fft_truncated(array_buffered, kernel);
}

Array convolve2d_fft_truncated(const Array &array, const Array &kernel)
{
  if (!validate_non_empty(array)) return Array();
  if (!validate_non_empty(kernel)) return Array();

  if (array.shape.x <= kernel.shape.x || array.shape.y <= kernel.shape.y)
  {
    hmap::log::warn(
        "Array shape ({}, {}) must be strictly larger than kernel shape ({}, "
        "{}) for truncated convolution",
        array.shape.x,
        array.shape.y,
        kernel.shape.x,
        kernel.shape.y);
    return Array();
  }

  const int out_x = array.shape.x - kernel.shape.x;
  const int out_y = array.shape.y - kernel.shape.y;

  // Linear convolution pad size
  const int min_fft_x = array.shape.x + kernel.shape.x - 1;
  const int min_fft_y = array.shape.y + kernel.shape.y - 1;

  const int fft_x = fft_good_size(min_fft_x);
  const int fft_y = fft_good_size(min_fft_y);

  const size_t s_fft_x = static_cast<size_t>(fft_x);
  const size_t s_fft_y = static_cast<size_t>(fft_y);
  const size_t half_fft_x = s_fft_x / 2 + 1;

  // Prepare padded array data
  std::vector<float> padded_array(s_fft_x * s_fft_y, 0.0f);
  for (int j = 0; j < array.shape.y; ++j)
  {
    const float *src_row = &array.vector[j * array.shape.x];
    float       *dst_row = &padded_array[j * s_fft_x];
    std::copy_n(src_row, array.shape.x, dst_row);
  }

  // Prepare padded and flipped kernel data (flip for convolution: h(p, q) ->
  // h(-p, -q))
  std::vector<float> padded_kernel(s_fft_x * s_fft_y, 0.0f);
  for (int j = 0; j < kernel.shape.y; ++j)
  {
    const int flip_j = kernel.shape.y - 1 - j;
    float    *dst_row = &padded_kernel[j * s_fft_x];
    for (int i = 0; i < kernel.shape.x; ++i)
    {
      const int flip_i = kernel.shape.x - 1 - i;
      dst_row[i] = kernel(flip_i, flip_j);
    }
  }

  pocketfft::shape_t  shape_real = {s_fft_y, s_fft_x};
  pocketfft::stride_t stride_real = {
      static_cast<ptrdiff_t>(s_fft_x * sizeof(float)),
      static_cast<ptrdiff_t>(sizeof(float))};

  pocketfft::stride_t stride_complex = {
      static_cast<ptrdiff_t>(half_fft_x * sizeof(std::complex<float>)),
      static_cast<ptrdiff_t>(sizeof(std::complex<float>))};

  pocketfft::shape_t axes = {0, 1};

  const size_t                     n_complex = s_fft_y * half_fft_x;
  std::vector<std::complex<float>> dft_array(n_complex);
  std::vector<std::complex<float>> dft_kernel(n_complex);

  const size_t num_threads = get_num_threads();

  pocketfft::r2c<float>(shape_real,
                        stride_real,
                        stride_complex,
                        axes,
                        pocketfft::FORWARD,
                        padded_array.data(),
                        dft_array.data(),
                        1.0f,
                        num_threads);

  pocketfft::r2c<float>(shape_real,
                        stride_real,
                        stride_complex,
                        axes,
                        pocketfft::FORWARD,
                        padded_kernel.data(),
                        dft_kernel.data(),
                        1.0f,
                        num_threads);

  // Elementwise multiplication with normalization factor
  const float scale = 1.0f / static_cast<float>(s_fft_x * s_fft_y);

#pragma omp parallel for
  for (size_t idx = 0; idx < n_complex; ++idx)
  {
    dft_array[idx] = (dft_array[idx] * dft_kernel[idx]) * scale;
  }

  // Inverse transform back into padded_array buffer
  pocketfft::c2r<float>(shape_real,
                        stride_complex,
                        stride_real,
                        axes,
                        pocketfft::BACKWARD,
                        dft_array.data(),
                        padded_array.data(),
                        1.0f,
                        num_threads);

  // Extract the valid region matching convolve2d_truncated
  Array     array_out(glm::ivec2(out_x, out_y));
  const int roi_x = kernel.shape.x - 1;
  const int roi_y = kernel.shape.y - 1;

#pragma omp parallel for
  for (int j = 0; j < out_y; ++j)
  {
    const float *src_row = &padded_array[(roi_y + j) * s_fft_x + roi_x];
    float       *dst_row = &array_out.vector[j * out_x];
    std::copy_n(src_row, out_x, dst_row);
  }

  return array_out;
}

} // namespace hmap
