/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fftw3.h>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/interpolate1d.hpp"
#include "highmap/math.hpp"
#include "highmap/operator.hpp"

namespace hmap
{

Array fft_filter(Array &array, float kc, bool smooth_cutoff)
{
  if (array.shape.x != array.shape.y)
  {
    LOG_ERROR("Input data must be square. Returning a zero-filled array.");
    return Array(array.shape);
  }

  int n = array.shape.x;

  // output array for complex numbers in single precision (n * (n/2+1)
  // because of symmetry)
  std::vector<fftwf_complex> out(n * (n / 2 + 1));

  // output array for the filtered data
  std::vector<float> filtered(n * n);

  // create the FFTW plans for forward and backward transformations
  fftwf_plan forward_plan = fftwf_plan_dft_r2c_2d(n,
                                                  n,
                                                  array.vector.data(),
                                                  out.data(),
                                                  FFTW_ESTIMATE);
  fftwf_plan backward_plan = fftwf_plan_dft_c2r_2d(n,
                                                   n,
                                                   out.data(),
                                                   filtered.data(),
                                                   FFTW_ESTIMATE);

  // execute the forward FFT
  fftwf_execute(forward_plan);

  // // apply the low-pass filter in the frequency domain
  if (!smooth_cutoff)
  {
    for (int i = 0; i < n; ++i)
      for (int j = 0; j <= n / 2; ++j)
      {
        int idx = i * (n / 2 + 1) + j;

        // compute the wavenumbers (wrap around for negative frequencies)
        int kx = (i <= n / 2) ? i : i - n;
        int ky = j;

        float wavenumber = (float)std::sqrt(kx * kx + ky * ky);

        // apply the cutoff: zero out high wavenumbers
        if (wavenumber > kc)
        {
          out[idx][0] = 0.0f; // real part
          out[idx][1] = 0.0f; // imaginary part
        }
      }
  }
  else
  {
    // smoother transition
    for (int i = 0; i < n; ++i)
      for (int j = 0; j <= n / 2; ++j)
      {
        int idx = i * (n / 2 + 1) + j;

        // compute the wavenumbers (wrap around for negative frequencies)
        int kx = (i <= n / 2) ? i : i - n;
        int ky = j;

        float wavenumber = std::sqrt(kx * kx + ky * ky);

        // apply the cutoff: zero out high wavenumbers
        if (wavenumber > kc)
        {
          float t = 1.f - (wavenumber - kc) / (0.5f * n - kc);
          t = smoothstep7(std::clamp(t, 0.f, 1.f));
          out[idx][0] *= t;
          out[idx][1] *= t;
        }
      }
  }

  // execute the inverse FFT
  fftwf_execute(backward_plan);

  // normalize the filtered output
  Array array_out(array.shape);
  float norm_coeff = 1.f / (n * n);

  for (int k = 0; k < n * n; k++)
  {
    Vec2<int> ij;
    ij.x = (int)std::floor(k / n);
    ij.y = k - ij.x * n;

    array_out(ij.x, ij.y) = norm_coeff * filtered[k];
  }

  // clean up FFTW resources
  fftwf_destroy_plan(forward_plan);
  fftwf_destroy_plan(backward_plan);
  fftwf_cleanup();

  return array_out;
}

Array fft_filter(Array &array, const std::vector<float> &weights)
{
  if (array.shape.x != array.shape.y)
  {
    LOG_ERROR("Input data must be square. Returning a zero-filled array.");
    return Array(array.shape);
  }

  int n = array.shape.x;

  // weights interpolator
  std::vector<float> kw = linspace(0.f, (float)(n - 1), (int)weights.size());
  Interpolator1D     interp = Interpolator1D(kw,
                                         weights,
                                         InterpolationMethod1D::LINEAR);

  // output array for complex numbers in single precision (n * (n/2+1)
  // because of symmetry)
  std::vector<fftwf_complex> out(n * (n / 2 + 1));

  // output array for the filtered data
  std::vector<float> filtered(n * n);

  // create the FFTW plans for forward and backward transformations
  fftwf_plan forward_plan = fftwf_plan_dft_r2c_2d(n,
                                                  n,
                                                  array.vector.data(),
                                                  out.data(),
                                                  FFTW_ESTIMATE);
  fftwf_plan backward_plan = fftwf_plan_dft_c2r_2d(n,
                                                   n,
                                                   out.data(),
                                                   filtered.data(),
                                                   FFTW_ESTIMATE);

  // execute the forward FFT
  fftwf_execute(forward_plan);

  // apply the low-pass filter in the frequency domain
  float kmax = 0.f;

  for (int i = 0; i < n; ++i)
    for (int j = 0; j <= n / 2; ++j)
    {
      int idx = i * (n / 2 + 1) + j;

      // compute the wavenumbers (wrap around for negative frequencies)
      int kx = (i <= n / 2) ? i : i - n;
      int ky = j;

      float wavenumber = (float)std::sqrt(kx * kx + ky * ky);

      // apply the weights
      if (wavenumber < (float)(0.5f * n))
      {
        float c = interp(wavenumber);
        out[idx][0] *= c;
        out[idx][1] *= c;
      }
      else
      {
        out[idx][0] *= weights.back();
        out[idx][1] *= weights.back();
      }

      kmax = std::max(kmax, wavenumber);
    }

  // execute the inverse FFT
  fftwf_execute(backward_plan);

  // normalize the filtered output
  Array array_out(array.shape);
  float norm_coeff = 1.f / (n * n);

  for (int k = 0; k < n * n; k++)
  {
    Vec2<int> ij;
    ij.x = (int)std::floor(k / n);
    ij.y = k - ij.x * n;

    array_out(ij.x, ij.y) = norm_coeff * filtered[k];
  }

  // clean up FFTW resources
  fftwf_destroy_plan(forward_plan);
  fftwf_destroy_plan(backward_plan);
  fftwf_cleanup();

  return array_out;
}

Array fft_modulus(Array &array, bool shift_to_center)
{
  if (array.shape.x != array.shape.y)
  {
    LOG_ERROR("Input data must be square. Returning a zero-filled array.");
    return Array(array.shape);
  }

  Array modulus(array.shape);
  int   n = array.shape.x;

  // TODO work on a fix resolution

  // output array for complex numbers in single precision (n * (n/2+1)
  // because of symmetry)
  std::vector<fftwf_complex> out(n * (n / 2 + 1));

  // create the FFTW plan for single-precision real-to-complex transformation
  fftwf_plan plan = fftwf_plan_dft_r2c_2d(n,
                                          n,
                                          array.vector.data(),
                                          out.data(),
                                          FFTW_ESTIMATE);

  // execute the FFT
  fftwf_execute(plan);

  // compute and print the modulus of the first few output values
  for (int i = 0; i < n; ++i)
    for (int j = 0; j <= n / 2; ++j)
    {
      int idx = i * (n / 2 + 1) + j;

      // modulus for the stored upper triangle
      float real_part = out[idx][0];
      float imag_part = out[idx][1];
      float mod = std::hypot(real_part, imag_part);

      // assign modulus to the corresponding symmetric locations
      modulus(i, j) = mod;

      // Lower triangle
      if (j > 0 && j < n / 2)
      {
        int sym_i = (n - i) % n;
        int sym_j = (n - j) % n;
        modulus(sym_i, sym_j) = mod;
      }
    }

  // center the spectrum if requested
  if (shift_to_center)
  {
    Array modulus_shifted(array.shape);

    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        modulus_shifted(i, j) = modulus((i + n / 2) % n, (j + n / 2) % n);

    modulus = modulus_shifted;
  }

  // clean-up
  fftwf_destroy_plan(plan);
  fftwf_cleanup();

  return modulus;
}

} // namespace hmap
