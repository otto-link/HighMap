/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/convolve.hpp"
#include "highmap/operator.hpp"
#include "highmap/primitives.hpp"
#include "highmap/transform.hpp"

namespace hmap
{

Array convolve2d_svd(const Array &array, const Array &kernel, int rank)
{
  Array array_out = Array(array.shape);

  // --- perform SVD decomposition of the kernel

  gsl_matrix *mat_u = gsl_matrix_alloc(kernel.shape.x, kernel.shape.y);
  gsl_matrix *mat_v = gsl_matrix_alloc(kernel.shape.y, kernel.shape.y);
  gsl_vector *vec_s = gsl_vector_alloc(kernel.shape.y);
  gsl_vector *vec_w = gsl_vector_alloc(kernel.shape.y); // work vector

  for (int j = 0; j < kernel.shape.y; j++)
    for (int i = 0; i < kernel.shape.x; i++)
      gsl_matrix_set(mat_u, i, j, kernel(i, j));

  // (NB - mat_v is the transpose of classical "V" of SVD formulation)
  gsl_linalg_SV_decomp(mat_u, mat_v, vec_s, vec_w);

  // --- use SVD vectors for convolution with 1D kernels use SVD
  // --- vectors for each singular SVD values as a pair of 1D //
  // --- kernels

  for (int p = 0; p < std::min(rank, kernel.shape.y); p++)
  {
    Array              c2d = Array(array.shape);
    std::vector<float> ki(kernel.shape.x);
    std::vector<float> kj(kernel.shape.y);

    for (int i = 0; i < kernel.shape.x; i++)
      ki[i] = gsl_matrix_get(mat_u, i, p);

    for (int j = 0; j < kernel.shape.y; j++)
      kj[j] = gsl_matrix_get(mat_v, j, p);

    c2d = convolve1d_i(array, ki);
    c2d = convolve1d_j(c2d, kj);

    array_out += gsl_vector_get(vec_s, p) * c2d;
  }

  // --- delete GSL objects
  gsl_matrix_free(mat_u);
  gsl_matrix_free(mat_v);
  gsl_vector_free(vec_s);
  gsl_vector_free(vec_w);

  return array_out;
}

Array convolve2d_svd_rotated_kernel(const Array &array,
                                    const Array &kernel,
                                    int          rank,
                                    int          n_rotations,
                                    uint         seed)
{
  Array array_out = Array(array.shape);

  std::vector<float> angles = linspace(0.f, 360.f, n_rotations, false);
  float              density = 1.f / (float)n_rotations;

  for (int k = 0; k < n_rotations; k++)
  {
    Array base = white_sparse_binary(array.shape, density, seed);
    base *= array;

    Array kr = kernel;
    rotate(kr, angles[k]);

    array_out += convolve2d_svd(base, kernel, rank);
  }

  return array_out;
}

} // namespace hmap
