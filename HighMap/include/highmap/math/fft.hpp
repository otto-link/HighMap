/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file fft.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Fast Fourier Transform (FFT) wrappers and FFT-based convolution for
 * arrays.
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include <complex>
#include <vector>

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Return 2D convolution of an array with a kernel using FFT.
 *
 * This function performs 2D convolution in the frequency domain using PocketFFT
 * real-to-complex (r2c) and complex-to-real (c2r) transforms with
 * single-precision (float) operations. It is optimized for large kernels and
 * values performance over numerical precision beyond single precision.
 *
 * The boundary condition and output shape match the standard convolve2d
 * convention (same shape as input array, with mirrored boundary conditions via
 * buffering).
 *
 * @param  array  Input array to be convolved.
 * @param  kernel 2D kernel to be used for the convolution.
 * @return        Array Resulting array after applying the FFT-based
 *                convolution.
 */
Array convolve2d_fft(const Array &array, const Array &kernel);

/**
 * @brief Return 2D convolution of an array with a kernel using FFT with
 * truncated output.
 *
 * The output array has dimensions:
 * {array.shape.x - kernel.shape.x, array.shape.y - kernel.shape.y}.
 *
 * @param  array  Input array to be convolved.
 * @param  kernel 2D kernel to be used for the convolution.
 * @return        Array Resulting array after applying truncated convolution.
 */
Array convolve2d_fft_truncated(const Array &array, const Array &kernel);

/**
 * @brief 2D Complex-to-Real (c2r) inverse FFT wrapper.
 *
 * Transforms a half-complex spectrum back to a 2D real array of dimensions
 * shape_out.
 *
 * @param  spectrum  Input frequency domain complex data of size shape_out.y *
 *                   (shape_out.x / 2 + 1).
 * @param  shape_out Spatial shape {x, y} of the real target array.
 * @param  normalize Whether to normalize by 1 / (shape_out.x * shape_out.y).
 * @return           Array Resulting real 2D array.
 */
Array fft2d_c2r(const std::vector<std::complex<float>> &spectrum,
                glm::ivec2                              shape_out,
                bool                                    normalize = true);

/**
 * @brief 2D Real-to-Complex (r2c) forward FFT wrapper.
 *
 * Transforms a 2D real array into the frequency domain. Output shape in
 * frequency domain is {shape_in.x / 2 + 1, shape_in.y}.
 *
 * @param  array Input real 2D array.
 * @return       std::vector<std::complex<float>> Frequency spectrum.
 */
std::vector<std::complex<float>> fft2d_r2c(const Array &array);

/**
 * @brief Returns the next good FFT size for real transforms (multiples of 2, 3,
 * 5).
 *
 * @param  n Target minimum size.
 * @return   int         Optimal FFT size >= n.
 */
int fft_good_size(int n);

} // namespace hmap
