#include <cmath>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap
{

Array convolve2d_svd(const Array &array, const Array &kernel, int rank)
{
  Array array_out = Array(array.shape);

  // --- perform SVD decomposition of the kernel

  gsl_matrix *mat_u = gsl_matrix_alloc(kernel.shape[0], kernel.shape[1]);
  gsl_matrix *mat_v = gsl_matrix_alloc(kernel.shape[1], kernel.shape[1]);
  gsl_vector *vec_s = gsl_vector_alloc(kernel.shape[1]);
  gsl_vector *vec_w = gsl_vector_alloc(kernel.shape[1]); // work vector

  for (int i = 0; i < kernel.shape[0]; i++)
    for (int j = 0; j < kernel.shape[1]; j++)
      gsl_matrix_set(mat_u, i, j, kernel(i, j));

  // (NB - mat_v is the transpose of classical "V" of SVD formulation)
  gsl_linalg_SV_decomp(mat_u, mat_v, vec_s, vec_w);

  // --- use SVD vectors for convolution with 1D kernels use SVD
  // --- vectors for each singular SVD values as a pair of 1D //
  // --- kernels

  for (int p = 0; p < std::min(rank, kernel.shape[1]); p++)
  {
    Array              c2d = Array(array.shape);
    std::vector<float> ki(kernel.shape[0]);
    std::vector<float> kj(kernel.shape[1]);

    for (int i = 0; i < kernel.shape[0]; i++)
      ki[i] = gsl_matrix_get(mat_u, i, p);

    for (int j = 0; j < kernel.shape[1]; j++)
      kj[j] = gsl_matrix_get(mat_v, j, p);

    c2d = convolve1d_i(array, ki);
    c2d = convolve1d_j(c2d, kj);

    array_out = array_out + gsl_vector_get(vec_s, p) * c2d;
  }

  // --- delete GSL objects
  gsl_matrix_free(mat_u);
  gsl_matrix_free(mat_v);
  gsl_vector_free(vec_s);
  gsl_vector_free(vec_w);

  return array_out;
}

} // namespace hmap
