/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file fft_filter.cpp
 * @brief Implementation of FFT-based filtering and modulus computation for 2D
 * arrays.
 *
 * This file provides functions to apply Fourier transform-based filtering
 * and to compute the modulus of frequency components for 2D arrays. It uses
 * FFTW3 for efficient FFT operations and supports both sharp and smooth cutoff
 * filters.
 *
 * Copyright (c) 2023 Otto Link.
 */
#pragma once

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Applies a low-pass filter to a 2D array using the FFT.
 *
 * This function computes the 2D FFT of the input array, applies a low-pass
 * filter in the frequency domain, and then computes the inverse FFT to produce
 * the filtered data. It supports both sharp and smooth cutoff filters.
 *
 * @param array A reference to the input 2D array (must be square).
 * @param kc The cutoff wavenumber for the low-pass filter.
 * @param smooth_cutoff Whether to apply a smooth cutoff (true) or a sharp
 * cutoff (false).
 * @return A new 2D array containing the filtered data.
 *
 * @note If the input array is not square, a zero-filled array is returned.
 *
 * **Example**
 * @include ex_fft_filter.cpp
 *
 * **Result**
 * @image html ex_fft_filter.png
 */
Array fft_filter(Array &array, float kc, bool smooth_cutoff = false);

/**
 * @brief Computes the modulus of the FFT of a 2D array.
 *
 * This function computes the 2D FFT of the input array and calculates the
 * modulus (magnitude) of the complex frequency components. Optionally, it can
 * shift the frequency spectrum to center low frequencies.
 *
 * @param array A reference to the input 2D array (must be square).
 * @param shift_to_center Whether to shift the frequency spectrum to center low
 * frequencies.
 * @return A new 2D array containing the modulus of the FFT components.
 *
 * @note If the input array is not square, a zero-filled array is returned.
 *
 *  * **Example**
 * @include ex_fft_modulus.cpp
 *
 * **Result**
 * @image html ex_fft_modulus.png
 */
Array fft_modulus(Array &array, bool shift_to_center = true);

} // namespace hmap