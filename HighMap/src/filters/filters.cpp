/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>
#include <random>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/convolve.hpp"
#include "highmap/curvature.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/kernels.hpp"
#include "highmap/operator.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"
#include "highmap/transform.hpp"

#include "highmap/internal/vector_utils.hpp"

#define NSIGMA 2

#define DI                                                                     \
  {                                                                            \
    -1, 0, 0, 1, -1, -1, 1, 1                                                  \
  }
#define DJ                                                                     \
  {                                                                            \
    0, 1, -1, 0, -1, 1, -1, 1                                                  \
  }
#define CD                                                                     \
  {                                                                            \
    1.f, 1.f, 1.f, 1.f, M_SQRT2, M_SQRT2, M_SQRT2, M_SQRT2                     \
  }

namespace hmap
{

void equalize(Array &array)
{
  Array flat_ref = hmap::white(array.shape, 0.f, 1.f, 0);
  match_histogram(array, flat_ref);
}

void equalize(Array &array, const Array *p_mask)
{
  if (!p_mask)
    equalize(array);
  else
  {
    Array array_f = array;
    equalize(array_f);
    array = lerp(array, array_f, *(p_mask));
  }
}

void expand(Array &array, int ir)
{
  Array array_new = array;
  int   ni = array.shape.x;
  int   nj = array.shape.y;
  Array k = cubic_pulse({2 * ir + 1, 2 * ir + 1});

  for (int j = 0; j < nj; j++)
  {
    int q1 = std::max(0, j - ir) - j;
    int q2 = std::min(nj, j + ir + 1) - j;
    for (int i = 0; i < ni; i++)
    {
      int p1 = std::max(0, i - ir) - i;
      int p2 = std::min(ni, i + ir + 1) - i;

      for (int q = q1; q < q2; q++)
        for (int p = p1; p < p2; p++)
        {
          float v = array(i + p, j + q) * k(p + ir, q + ir);
          array_new(i, j) = std::max(array_new(i, j), v);
        }
    }
  }

  array = array_new;
}

void expand(Array &array, int ir, const Array *p_mask)
{
  if (!p_mask)
    expand(array, ir);
  else
  {
    Array array_f = array;
    expand(array_f, ir);
    array = lerp(array, array_f, *(p_mask));
  }
}

void expand(Array &array, const Array &kernel)
{
  Array array_new = array;
  int   ni = array.shape.x;
  int   nj = array.shape.y;

  int ri1 = (int)(0.5f * kernel.shape.x);
  int ri2 = kernel.shape.x - ri1 - 1;
  int rj1 = (int)(0.5f * kernel.shape.y);
  int rj2 = kernel.shape.y - rj1 - 1;

  for (int j = 0; j < nj; j++)
  {
    int q1 = std::max(0, j - rj1) - j;
    int q2 = std::min(nj, j + rj2 + 1) - j;
    for (int i = 0; i < ni; i++)
    {
      int p1 = std::max(0, i - ri1) - i;
      int p2 = std::min(ni, i + ri2 + 1) - i;

      for (int q = q1; q < q2; q++)
        for (int p = p1; p < p2; p++)
        {
          float v = array(i + p, j + q) * kernel(p + ri1, q + rj1);
          array_new(i, j) = std::max(array_new(i, j), v);
        }
    }
  }
  array = array_new;
}

void expand(Array &array, const Array &kernel, const Array *p_mask)
{
  if (!p_mask)
    expand(array, kernel);
  else
  {
    Array array_f = array;
    expand(array_f, kernel);
    array = lerp(array, array_f, *(p_mask));
  }
}

void expand_directional(Array       &array,
                        int          ir,
                        float        angle,
                        float        aspect_ratio,
                        float        anisotropy,
                        const Array *p_mask)
{
  Array kernel = cubic_pulse_directional(Vec2<int>(2 * ir + 1, 2 * ir + 1),
                                         angle,
                                         aspect_ratio,
                                         anisotropy);
  expand(array, kernel, p_mask);
}

void expand_talus(Array       &z,
                  const Array &mask,
                  float        talus,
                  uint         seed,
                  float        noise_ratio)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(1.f - noise_ratio,
                                            1.f + noise_ratio);

  std::vector<int>   di = DI;
  std::vector<int>   dj = DJ;
  std::vector<float> c = CD;
  const uint         nb = di.size();

  Array mask_copy = mask;

  // initialize heap queue: algo starts from the cells defined by the mask
  std::vector<std::pair<float, std::pair<int, int>>> queue;

  for (int i = 2; i < z.shape.x - 2; i++)
    for (int j = 2; j < z.shape.y - 2; j++)
      if (mask_copy(i, j)) queue.push_back(std::pair(z(i, j), std::pair(i, j)));

  std::make_heap(queue.begin(), queue.end());

  // fill
  while (queue.size() > 0)
  {
    std::pair<int, std::pair<int, int>> current = queue.back();
    queue.pop_back();

    int i = current.second.first;
    int j = current.second.second;

    for (uint k = 0; k < nb; k++)
    {
      int p = i + di[k];
      int q = j + dj[k];

      if (p >= 0 && p < z.shape.x && q >= 0 && q < z.shape.y)
      {
        float h = z(i, j) + c[k] * talus;

        if (z(p, q) > h)
        {
          float rd = dis(gen);
          z(p, q) = z(i, j) + c[k] * talus * rd;
        }

        if (mask_copy(p, q) == 0.f)
        {
          queue.push_back(std::pair(z(p, q), std::pair(p, q)));
          std::push_heap(queue.begin(), queue.end());
          mask_copy(p, q) = 1.f;
        }
      }
    }
  }

  // clean-up boundaries
  extrapolate_borders(z, 2);
}

void fill_talus(Array &z, float talus, uint seed, float noise_ratio)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(1.f - noise_ratio,
                                            1.f + noise_ratio);

  std::vector<int>   di = DI;
  std::vector<int>   dj = DJ;
  std::vector<float> c = CD;
  const uint         nb = di.size();

  // trick to exclude border cells, to avoid checking out of bounds
  // indices
  set_borders(z, 10.f * z.max(), 2);

  // build queue (elevation, index (i, j))
  std::vector<std::pair<float, std::pair<int, int>>> queue;

  for (int i = 2; i < z.shape.x - 2; i++)
    for (int j = 2; j < z.shape.y - 2; j++)
      queue.push_back(std::pair(z(i, j), std::pair(i, j)));

  std::make_heap(queue.begin(), queue.end());

  // fill
  while (queue.size() > 0)
  {
    std::pair<int, std::pair<int, int>> current = queue.back();
    queue.pop_back();

    int i = current.second.first;
    int j = current.second.second;

    for (uint k = 0; k < nb; k++) // loop over neighbors
    {
      int   p = i + di[k];
      int   q = j + dj[k];
      float rd = dis(gen);
      float h = z(i, j) - c[k] * talus * rd;

      if (h > z(p, q))
      {
        z(p, q) = h;
        queue.push_back(std::pair(z(p, q), std::pair(p, q)));
        std::push_heap(queue.begin(), queue.end());
      }
    }
  }

  // clean-up boundaries
  extrapolate_borders(z, 2);
}

void fill_talus_fast(Array    &z,
                     Vec2<int> shape_coarse,
                     float     talus,
                     uint      seed,
                     float     noise_ratio)
{
  // apply the algorithm on the coarser mesh (and ajust the talus
  // value)
  int   step = std::max(z.shape.x / shape_coarse.x, z.shape.y / shape_coarse.y);
  float talus_coarse = talus * step;

  // add maximum filter to avoid loosing data (for instance those
  // defined at only one cell)
  Array z_coarse = Array(shape_coarse);
  {
    Array z_filtered = maximum_local(z, (int)std::ceil(0.5f * step));
    z_coarse = z_filtered.resample_to_shape(shape_coarse);
  }

  fill_talus(z_coarse, talus_coarse, seed, noise_ratio);

  // revert back to the original resolution but keep initial
  // smallscale details
  z_coarse = z_coarse.resample_to_shape(z.shape);

  clamp_min(z, z_coarse);
}

void fold(Array &array, int iterations, float k)
{
  fold(array, array.min(), array.max(), iterations, k);
}

void fold(Array &array, float vmin, float vmax, int iterations, float k)
{
  array -= vmin;
  float vref = (vmax - vmin) / (iterations + 1.f);

  for (int it = 0; it < iterations; it++)
  {
    array -= vref;

    if (k == 0.f)
      array = abs(array);
    else
      array = abs_smooth(array, k);
  }
}

void gain(Array &array, float factor)
{
  auto lambda = [&factor](float x)
  {
    return x < 0.5 ? 0.5f * std::pow(2.f * x, factor)
                   : 1.f - 0.5f * std::pow(2.f * (1.f - x), factor);
  };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void gain(Array &array, float factor, const Array *p_mask)
{
  if (!p_mask)
    gain(array, factor);
  else
  {
    Array array_f = array;
    gain(array_f, factor);
    array = lerp(array, array_f, *(p_mask));
  }
}

void gamma_correction(Array &array, float gamma)
{
  auto lambda = [&gamma](float x) { return std::pow(x, gamma); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void gamma_correction(Array &array, float gamma, const Array *p_mask)
{
  if (!p_mask)
    gamma_correction(array, gamma);
  else
  {
    Array array_f = array;
    gamma_correction(array, gamma);
    array = lerp(array, array_f, *(p_mask));
  }
}

void gamma_correction_local(Array &array, float gamma, int ir, float k)
{
  Array amin = minimum_local(array, ir);
  Array amax = maximum_local(array, ir);

  smooth_cpulse(amin, ir);
  smooth_cpulse(amax, ir);

  if (k != 0) // with smoothing
  {
    for (int j = 0; j < array.shape.y; j++)
      for (int i = 0; i < array.shape.x; i++)
      {
        float v = std::abs(array(i, j) - amin(i, j)) /
                  (amax(i, j) - amin(i, j) + 1e-30);
        v = std::sqrt(v * v + k);
        array(i, j) = std::pow(v, gamma) * (amax(i, j) - amin(i, j)) +
                      amin(i, j);
      }
  }
  else // without smoothing
  {
    for (int j = 0; j < array.shape.y; j++)
      for (int i = 0; i < array.shape.x; i++)
      {
        float v = std::abs(array(i, j) - amin(i, j)) /
                  (amax(i, j) - amin(i, j) + 1e-30);
        array(i, j) = std::pow(v, gamma) * (amax(i, j) - amin(i, j)) +
                      amin(i, j);
      }
  }
}

void gamma_correction_local(Array       &array,
                            float        gamma,
                            int          ir,
                            const Array *p_mask,
                            float        k)
{
  if (!p_mask)
    gamma_correction_local(array, gamma, ir, k);
  else
  {
    Array array_f = array;
    gamma_correction_local(array_f, gamma, ir, k);
    array = lerp(array, array_f, *(p_mask));
  }
}

void kuwahara(Array &array, int ir, float mix_ratio)
{

  Array array_buffered = generate_buffered_array(array,
                                                 Vec4<int>(ir, ir, ir, ir));
  Array array_out(array_buffered.shape);

  for (int j = ir; j < array_buffered.shape.y - ir; j++)
    for (int i = ir; i < array_buffered.shape.x - ir; i++)
    {
      // build quadrants
      Array q1 = array_buffered.extract_slice(
          Vec4<int>(i - ir, i + 1, j - ir, j + 1));
      Array q2 = array_buffered.extract_slice(
          Vec4<int>(i - ir, i + 1, j + 1, j + ir));
      Array q3 = array_buffered.extract_slice(
          Vec4<int>(i + 1, i + ir, j - ir, j + 1));
      Array q4 = array_buffered.extract_slice(
          Vec4<int>(i + 1, i + ir, j + 1, j + ir));

      std::vector<float> means = {q1.mean(), q2.mean(), q3.mean(), q4.mean()};
      std::vector<float> stds = {q1.std(), q2.std(), q3.std(), q4.std()};

      size_t imin = std::min_element(stds.begin(), stds.end()) - stds.begin();
      array_out(i, j) = means[imin];
    }

  if (mix_ratio == 1.f)
  {
    array = array_out.extract_slice(Vec4<int>(ir,
                                              array_buffered.shape.x - ir,
                                              ir,
                                              array_buffered.shape.y - ir));
  }
  else
  {
    array = lerp(
        array,
        array_out.extract_slice(Vec4<int>(ir,
                                          array_buffered.shape.x - ir,
                                          ir,
                                          array_buffered.shape.y - ir)),
        mix_ratio);
  }
}

void kuwahara(Array &array, int ir, const Array *p_mask, float mix_ratio)
{
  if (!p_mask)
    kuwahara(array, ir, mix_ratio);
  else
  {
    Array array_f = array;
    float forced_mix_ratio = 1.f;
    kuwahara(array_f, ir, forced_mix_ratio);
    array = lerp(array, array_f, *(p_mask));
  }
}

void laplace(Array &array, float sigma, int iterations)
{
  for (int it = 0; it < iterations; it++)
  {
    Array delta = laplacian(array);
    array += sigma * delta;
  }
}

void laplace(Array &array, const Array *p_mask, float sigma, int iterations)
{
  if (!p_mask)
    laplace(array, sigma, iterations);
  else
  {
    Array array_f = array;
    laplace(array_f, sigma, iterations);
    array = lerp(array, array_f, *(p_mask));
  }
}

void laplace_edge_preserving(Array &array,
                             float  talus,
                             float  sigma,
                             int    iterations)
{
  for (int it = 0; it < iterations; it++)
  {
    Array c = gradient_norm(array);
    c = 1.f / (1.f + c * c / (talus * talus));

    Array dcx = gradient_x(c);
    Array dcy = gradient_y(c);
    Array dzx = gradient_x(array);
    Array dzy = gradient_y(array);
    Array delta = laplacian(array);

    array += sigma * (dcx * dzx + dcy * dzy + c * delta);
  }
}

void laplace_edge_preserving(Array       &array,
                             float        talus,
                             const Array *p_mask,
                             float        sigma,
                             int          iterations)
{
  if (!p_mask)
    laplace_edge_preserving(array, talus, sigma, iterations);
  else
  {
    Array array_f = array;
    laplace_edge_preserving(array_f, talus, sigma, iterations);
    array = lerp(array, array_f, *(p_mask));
  }
}

void low_pass_high_order(Array &array, int order, float sigma)
{
  Array df = array;

  // filtering coefficients
  std::vector<float> kernel;

  switch (order)
  {
  case (5): kernel = {0.0625f, -0.25f, 0.375f, -0.25f, 0.0625f}; break;

  case (7):
    kernel = {-0.015625f,
              0.09375f,
              -0.234375f,
              0.3125f,
              -0.234375f,
              0.09375f,
              -0.015625f};
    break;

  case (9):
    kernel = {0.00390625f,
              -0.03125f,
              0.109375f,
              -0.21875f,
              0.2734375f,
              -0.21875f,
              0.109375f,
              -0.03125f,
              0.00390625f};
    break;
  }

  df = convolve1d_i(array, kernel);
  array -= sigma * df;

  df = convolve1d_j(array, kernel);
  array -= sigma * df;
}

void make_binary(Array &array, float threshold)
{
  auto lambda = [&threshold](float a) { return a > threshold ? 1.f : 0.f; };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void match_histogram(Array &array, const Array &array_reference)
{
  std::vector<size_t> ki = argsort(array.vector);
  std::vector<size_t> kr = argsort(array_reference.vector);

  for (size_t i = 0; i < ki.size(); i++)
    array.vector[ki[i]] = array_reference.vector[kr[i]];
}

Array mean_local(const Array &array, int ir)
{
  Array array_out = Array(array.shape);

  std::vector<float> k1d(2 * ir + 1);
  for (auto &v : k1d)
    v = 1.f / (float)(2 * ir + 1);

  array_out = convolve1d_i(array, k1d);
  array_out = convolve1d_j(array_out, k1d);

  return array_out;
}

Array mean_shift(const Array &array,
                 int          ir,
                 float        talus,
                 int          iterations,
                 bool         talus_weighted)
{
  const Vec2<int> shape = array.shape;
  Array           array_next = Array(shape);
  Array           array_prev = array;

  for (int it = 0; it < iterations; it++)
  {

    if (talus_weighted)
    {
      for (int j = 0; j < shape.y; j++)
        for (int i = 0; i < shape.x; i++)
        {
          float sum = 0.f;
          float norm = 0.f;

          for (int q = j - ir; q < j + ir + 1; ++q)
            for (int p = i - ir; p < i + ir + 1; ++p)
              if (p > 0 && p < shape.x && q > 0 && q < shape.y)
              {
                float dv = std::abs(array_prev(i, j) - array_prev(p, q));
                if (dv < talus)
                {
                  float weight = 1.f - dv / talus;
                  sum += array(p, q) * weight;
                  norm += weight;
                }
              }

          array_next(i, j) = sum / norm;
        }
    }
    else
    {
      for (int j = 0; j < shape.y; j++)
        for (int i = 0; i < shape.x; i++)
        {
          int   count = 0.f;
          float sum = 0.f;

          for (int q = j - ir; q < j + ir + 1; ++q)
            for (int p = i - ir; p < i + ir + 1; ++p)
              if (p > 0 && p < shape.x && q > 0 && q < shape.y)
              {
                float dv = std::abs(array_prev(i, j) - array_prev(p, q));
                if (dv < talus)
                {
                  sum += array(p, q);
                  count++;
                }
              }

          array_next(i, j) = sum / static_cast<float>(count);
        }
    }

    if (iterations > 1) array_prev = array_next;
  }

  return array_next;
}

void median_3x3(Array &array)
{
  Array array_out = Array(array.shape);

  std::vector<float> v(9);

  for (int j = 1; j < array.shape.y - 1; j++)
    for (int i = 1; i < array.shape.x - 1; i++)
    {
      v[0] = array(i - 1, j - 1);
      v[1] = array(i - 1, j);
      v[2] = array(i - 1, j + 1);
      v[3] = array(i, j - 1);
      v[4] = array(i, j);
      v[5] = array(i, j + 1);
      v[6] = array(i + 1, j - 1);
      v[7] = array(i + 1, j);
      v[8] = array(i + 1, j + 1);

      std::sort(v.begin(), v.end());
      array_out(i, j) = v[4];
    }

  for (int j = 1; j < array.shape.y - 1; j++)
    for (int i = 1; i < array.shape.x - 1; i++)
      array(i, j) = array_out(i, j);
}

void median_3x3(Array &array, const Array *p_mask)
{
  if (!p_mask)
    median_3x3(array);
  else
  {
    Array array_f = array;
    median_3x3(array_f);
    array = lerp(array, array_f, *(p_mask));
  }
}

void normal_displacement(Array &array, float amount, int ir, bool reverse)
{
  Array array_f = array;
  Array array_new = Array(array.shape);

  if (ir > 0) smooth_cpulse(array_f, ir);

  // add a shape factor to avoid artifacts close to the boundaries
  Array factor = biweight(array.shape); // smooth_cosine(array.shape);

  if (reverse) amount = -amount;

  for (int j = 1; j < array.shape.y - 1; j++)
    for (int i = 1; i < array.shape.x - 1; i++)
    {
      Vec3<float> n = array_f.get_normal_at(i, j);

      float x = (float)i - amount * array.shape.x * n.x * factor(i, j);
      float y = (float)j - amount * array.shape.y * n.y * factor(i, j);

      // bilinear interpolation parameters
      int ip = std::clamp((int)x, 0, array.shape.x - 1);
      int jp = std::clamp((int)y, 0, array.shape.y - 1);

      float u = std::clamp(x - (float)ip, 0.f, 1.f);
      float v = std::clamp(y - (float)jp, 0.f, 1.f);

      array_new(i, j) = array.get_value_bilinear_at(ip, jp, u, v);
    }
  fill_borders(array_new);

  array = array_new;
}

void normal_displacement(Array       &array,
                         const Array *p_mask,
                         float        amount,
                         int          ir,
                         bool         reverse)
{
  if (!p_mask)
    normal_displacement(array, amount, ir, reverse);
  else
  {
    Array array_f = array;
    normal_displacement(array_f, amount, ir, reverse);
    array = lerp(array, array_f, *(p_mask));
  }
}

void plateau(Array &array, int ir, float factor)
{
  Array amin = minimum_local(array, ir);
  Array amax = maximum_local(array, ir);

  smooth_cpulse(amin, ir);
  smooth_cpulse(amax, ir);

  array = (array - amin) / (amax - amin + std::numeric_limits<float>::min());
  clamp(array); // keep things under control...
  gain(array, factor);
  array = amin + (amax - amin) * array;
}

void plateau(Array &array, const Array *p_mask, int ir, float factor)
{
  if (!p_mask)
    plateau(array, ir, factor);
  else
  {
    Array array_f = array;
    plateau(array_f, ir, factor);
    array = lerp(array, array_f, *(p_mask));
  }
}

void sharpen(Array &array, float ratio)
{
  Array lp = Array(array.shape);

  for (int j = 1; j < array.shape.y - 1; j++)
    for (int i = 1; i < array.shape.x - 1; i++)
    {
      lp(i, j) = 5.f * array(i, j) - array(i + 1, j) - array(i - 1, j) -
                 array(i, j - 1) - array(i, j + 1);
    }
  extrapolate_borders(lp);
  array = (1.f - ratio) * array + ratio * lp;
}

void sharpen(Array &array, const Array *p_mask, float ratio)
{
  if (!p_mask)
    sharpen(array, ratio);
  else
  {
    Array array_f = array;
    sharpen(array_f, ratio);
    array = lerp(array, array_f, *(p_mask));
  }
}

void sharpen_cone(Array &array, int ir, float scale)
{
  Array array_low_pass = array;
  smooth_cone(array_low_pass, ir);
  array += scale * (array - array_low_pass);
}

void sharpen_cone(Array &array, const Array *p_mask, int ir, float scale)
{
  if (!p_mask)
    sharpen_cone(array, ir, scale);
  else
  {
    Array array_f = array;
    sharpen_cone(array_f, ir, scale);
    array = lerp(array, array_f, *(p_mask));
  }
}

void shrink(Array &array, int ir)
{
  float amax = array.max();
  array = amax - array;
  expand(array, ir);
  array = amax - array;
}

void shrink(Array &array, int ir, const Array *p_mask)
{
  if (!p_mask)
    shrink(array, ir);
  else
  {
    Array array_f = array;
    shrink(array_f, ir);
    array = lerp(array, array_f, *(p_mask));
  }
}

void shrink(Array &array, const Array &kernel)
{
  float amax = array.max();
  array = amax - array;
  expand(array, kernel);
  array = amax - array;
}

void shrink(Array &array, const Array &kernel, const Array *p_mask)
{
  if (!p_mask)
    shrink(array, kernel);
  else
  {
    Array array_f = array;
    shrink(array_f, kernel);
    array = lerp(array, array_f, *(p_mask));
  }
}

void shrink_directional(Array       &array,
                        int          ir,
                        float        angle,
                        float        aspect_ratio,
                        float        anisotropy,
                        const Array *p_mask)
{
  Array kernel = cubic_pulse_directional(Vec2<int>(2 * ir + 1, 2 * ir + 1),
                                         angle,
                                         aspect_ratio,
                                         anisotropy);
  shrink(array, kernel, p_mask);
}

void smooth_cone(Array &array, int ir)
{
  // define kernel
  const int          nk = 2 * ir + 1;
  std::vector<float> k(nk);

  float sum = 0.f;
  float x0 = (float)nk / 2.f;
  for (int i = 0; i < nk; i++)
  {
    float x = std::abs((float)i - x0) / (float)ir;
    k[i] = 1.f - x;
    sum += k[i];
  }

  // normalize
  for (int i = 0; i < nk; i++)
  {
    k[i] /= sum;
  }

  // eventually convolve
  array = convolve1d_i(array, k);
  array = convolve1d_j(array, k);
}

void smooth_cone(Array &array, int ir, const Array *p_mask)
{
  if (!p_mask)
    smooth_cone(array, ir);
  else
  {
    Array array_f = array;
    smooth_cone(array_f, ir);
    array = lerp(array, array_f, *(p_mask));
  }
}

void smooth_cpulse(Array &array, int ir)
{
  // define kernel
  const int          nk = 2 * ir + 1;
  std::vector<float> k(nk);

  float sum = 0.f;
  float x0 = (float)nk / 2.f;
  for (int i = 0; i < nk; i++)
  {
    float x = std::abs((float)i - x0) / (float)ir;
    k[i] = 1.f - x * x * (3.f - 2.f * x);
    sum += k[i];
  }

  // normalize
  for (int i = 0; i < nk; i++)
  {
    k[i] /= sum;
  }

  // eventually convolve
  array = convolve1d_i(array, k);
  array = convolve1d_j(array, k);
}

void smooth_cpulse(Array &array, int ir, const Array *p_mask)
{
  if (!p_mask)
    smooth_cpulse(array, ir);
  else
  {
    Array array_f = array;
    smooth_cpulse(array_f, ir);
    array = lerp(array, array_f, *(p_mask));
  }
}

void smooth_flat(Array &array, int ir)
{
  // define kernel
  const int          nk = 2 * ir + 1;
  std::vector<float> k(nk);

  std::fill(k.begin(), k.end(), 1.f / (2.f * nk + 1.f));

  // eventually convolve
  array = convolve1d_i(array, k);
  array = convolve1d_j(array, k);
}

void smooth_gaussian(Array &array, int ir)
{
  // define Gaussian kernel (we keep NSIGMA standard deviations of the
  // kernel support)
  const int          nk = NSIGMA * (2 * ir + 1);
  std::vector<float> k(nk);

  float sum = 0.f;
  float sig2 = (float)(ir * ir);
  float x0 = (float)nk / 2.f;
  for (int i = 0; i < nk; i++)
  {
    float x = (float)i - x0;
    k[i] = std::exp(-0.5f * std::pow(x, 2.f) / sig2);
    sum += k[i];
  }

  // normalize
  for (int i = 0; i < nk; i++)
  {
    k[i] /= sum;
  }

  // eventually convolve
  array = convolve1d_i(array, k);
  array = convolve1d_j(array, k);
}

void smooth_gaussian(Array &array, int ir, const Array *p_mask)
{
  if (!p_mask)
    smooth_gaussian(array, ir);
  else
  {
    Array array_f = array;
    smooth_gaussian(array_f, ir);
    array = lerp(array, array_f, *(p_mask));
  }
}

void smooth_fill(Array &array, int ir, float k, Array *p_deposition_map)
{
  // keep a backup of the input for the deposition map
  Array array_bckp = Array();
  if (p_deposition_map != nullptr) array_bckp = array;

  // smooth filling
  Array array_smooth = array;
  smooth_cpulse(array_smooth, ir);
  array = maximum_smooth(array, array_smooth, k);

  // update map
  if (p_deposition_map)
  {
    *p_deposition_map = array - array_bckp;
    clamp_min(*p_deposition_map, 0.f);
  }
}

void smooth_fill(Array       &array,
                 int          ir,
                 const Array *p_mask,
                 float        k,
                 Array       *p_deposition_map)
{
  if (!p_mask)
    smooth_fill(array, ir, k, p_deposition_map);
  else
  {
    Array array_f = array;
    smooth_fill(array_f, ir, k, p_deposition_map);
    array = lerp(array, array_f, *(p_mask));
  }
}

void smooth_fill_holes(Array &array, int ir)
{
  Array array_smooth = array;
  smooth_cpulse(array_smooth, ir);

  // mask based on concave regions
  Array mask = -curvature_mean(array_smooth);
  clamp_min(mask, 0.f);
  make_binary(mask);

  int ic = (int)((float)ir / 2.f);
  if (ic > 1) smooth_cpulse(mask, ic);

  array = lerp(array, array_smooth, mask);
}

void smooth_fill_holes(Array &array, int ir, const Array *p_mask)
{
  if (!p_mask)
    smooth_fill_holes(array, ir);
  else
  {
    Array array_f = array;
    smooth_fill_holes(array_f, ir);
    array = lerp(array, array_f, *(p_mask));
  }
}

void smooth_fill_smear_peaks(Array &array, int ir)
{
  Array array_smooth = array;
  smooth_cpulse(array_smooth, ir);

  // mask based on concave regions
  Array mask = curvature_mean(array_smooth);
  clamp_min(mask, 0.f);
  make_binary(mask);

  int ic = (int)((float)ir / 2.f);
  if (ic > 0) smooth_cpulse(mask, ic);

  array = lerp(array, array_smooth, mask);
}

void smooth_fill_smear_peaks(Array &array, int ir, const Array *p_mask)
{
  if (!p_mask)
    smooth_fill_smear_peaks(array, ir);
  else
  {
    Array array_f = array;
    smooth_fill_smear_peaks(array_f, ir);
    array = lerp(array, array_f, *(p_mask));
  }
}

void smoothstep_local(Array &array, int ir)
{
  Array amin = minimum_local(array, ir);
  Array amax = maximum_local(array, ir);

  smooth_cpulse(amin, ir);
  smooth_cpulse(amax, ir);

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      float v = (array(i, j) - amin(i, j)) / (amax(i, j) - amin(i, j) + 1e-30);
      array(i, j) = smoothstep3(v) * (amax(i, j) - amin(i, j)) + amin(i, j);
    }
}

void smoothstep_local(Array &array, int ir, const Array *p_mask)
{
  if (!p_mask)
    smoothstep_local(array, ir);
  else
  {
    Array array_f = array;
    smoothstep_local(array_f, ir);
    array = lerp(array, array_f, *(p_mask));
  }
}

void steepen(Array &array, float scale, int ir)
{
  Array dx = gradient_x(array) * ((float)array.shape.x * -scale);
  Array dy = gradient_y(array) * ((float)array.shape.y * -scale);

  smooth_cpulse(dx, ir);
  smooth_cpulse(dy, ir);

  warp(array, &dx, &dy);
}

void steepen(Array &array, float scale, const Array *p_mask, int ir)
{
  if (!p_mask)
    steepen(array, scale, ir);
  else
  {
    Array array_f = array;
    steepen(array_f, scale, ir);
    array = lerp(array, array_f, *(p_mask));
  }
}

void steepen_convective(Array &array,
                        float  angle,
                        int    iterations,
                        int    ir,
                        float  dt)
{
  Array dx = Array(array.shape);
  Array dy = Array(array.shape);
  float alpha = angle / 180.f * M_PI;
  float ca = std::cos(alpha);
  float sa = std::sin(alpha);

  for (int it = 0; it < iterations; it++)
  {
    if (ir > 0)
    {
      Array array_filtered = array;
      smooth_cpulse(array_filtered, ir);
      gradient_x(array_filtered, dx);
      gradient_y(array_filtered, dy);
    }
    else
    {
      gradient_x(array, dx);
      gradient_y(array, dy);
    }

    dx *= (float)array.shape.x;
    dy *= (float)array.shape.y;
    array *= 1.f - dt * (ca * dx + sa * dy); // == du / dt = - u * du / dx
  }
}

void steepen_convective(Array       &array,
                        float        angle,
                        const Array *p_mask,
                        int          iterations,
                        int          ir,
                        float        dt)
{
  if (!p_mask)
    steepen_convective(array, angle, iterations, ir, dt);
  else
  {
    Array array_f = array;
    steepen_convective(array_f, angle, iterations, ir, dt);
    array = lerp(array, array_f, *(p_mask));
  }
}

void terrace(Array       &array,
             uint         seed,
             int          nlevels,
             float        gain,
             float        noise_ratio,
             const Array *p_noise,
             float        vmin,
             float        vmax)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(-noise_ratio, noise_ratio);

  // redefine min/max if sentinels values are detected
  if (vmax < vmin)
  {
    vmin = array.min();
    vmax = array.max();
  }

  // defines levels
  std::vector<float> levels = linspace(vmin, vmax, nlevels + 1);
  float              delta = (vmax - vmin) / (float)nlevels;

  // add noise, except for the first and last levels
  for (size_t k = 1; k < levels.size() - 1; k++)
    levels[k] += dis(gen) * delta;

  // apply a gain like filter
  auto lambda = [gain, &levels, vmin, vmax](float x, float noise = 0.f)
  {
    // find level interval
    float y = x + noise;
    y = std::clamp(y, vmin, vmax);

    size_t n = 1;
    while (y > levels[n] && n < levels.size())
      n++;
    n--;

    // rescale value to [0, 1]
    y = (y - levels[n]) / (levels[n + 1] - levels[n]);

    // apply gain
    y = y < 0.5 ? 0.5f * std::pow(2.f * y, gain)
                : 1.f - 0.5f * std::pow(2.f * (1.f - y), gain);

    // rescale back to original ammplitude interval
    return y * (levels[n + 1] - levels[n]) + levels[n] - noise;
  };

  if (p_noise)
    std::transform(array.vector.begin(),
                   array.vector.end(),
                   p_noise->vector.begin(),
                   array.vector.begin(),
                   lambda);
  else
    std::transform(array.vector.begin(),
                   array.vector.end(),
                   array.vector.begin(),
                   lambda);
}

void terrace(Array       &array,
             uint         seed,
             int          nlevels,
             const Array *p_mask,
             float        gain,
             float        noise_ratio,
             const Array *p_noise,
             float        vmin,
             float        vmax)
{
  if (!p_mask)
    terrace(array, seed, nlevels, gain, noise_ratio, p_noise, vmin, vmax);
  else
  {
    Array array_f = array;
    terrace(array_f, seed, nlevels, gain, noise_ratio, p_noise, vmin, vmax);
    array = lerp(array, array_f, *(p_mask));
  }
}

void wrinkle(Array      &array,
             float       wrinkle_amplitude,
             float       wrinkle_angle,
             float       displacement_amplitude,
             int         ir,
             float       kw,
             uint        seed,
             int         octaves,
             float       weight,
             Vec4<float> bbox)
{
  Array dx = displacement_amplitude * array;

  if (ir > 0) smooth_cpulse(dx, ir);

  Array dy = std::sin(wrinkle_angle / 180.f * M_PI) * dx;
  dx *= std::cos(wrinkle_angle / 180.f * M_PI);

  Array w = noise_fbm(NoiseType::PERLIN,
                      array.shape,
                      Vec2<float>(kw, kw),
                      seed,
                      octaves,
                      weight,
                      0.5f,
                      2.f,
                      nullptr,
                      &dx,
                      &dy,
                      nullptr,
                      bbox);

  array += wrinkle_amplitude * gradient_norm(w) * array.shape.x;
}

void wrinkle(Array       &array,
             float        wrinkle_amplitude,
             const Array *p_mask,
             float        wrinkle_angle,
             float        displacement_amplitude,
             int          ir,
             float        kw,
             uint         seed,
             int          octaves,
             float        weight,
             Vec4<float>  bbox)
{
  if (!p_mask)
    wrinkle(array,
            wrinkle_amplitude,
            wrinkle_angle,
            displacement_amplitude,
            ir,
            kw,
            seed,
            octaves,
            weight,
            bbox);
  else
  {
    Array array_f = array;
    wrinkle(array_f,
            wrinkle_amplitude,
            wrinkle_angle,
            displacement_amplitude,
            ir,
            kw,
            seed,
            octaves,
            weight,
            bbox);
    array = lerp(array, array_f, *(p_mask));
  }
}

} // namespace hmap
