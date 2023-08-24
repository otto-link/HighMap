/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>
#include <random>

#include "Interpolate.hpp"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/kernels.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

#include "op/vector_utils.hpp"

#define NSIGMA 2

namespace hmap
{

void expand(Array &array, int ir)
{
  Array array_new = array;
  int   ni = array.shape.x;
  int   nj = array.shape.y;
  Array k = cubic_pulse({2 * ir + 1, 2 * ir + 1});

  for (int i = 0; i < ni; i++)
  {
    int p1 = std::max(0, i - ir) - i;
    int p2 = std::min(ni, i + ir + 1) - i;
    for (int j = 0; j < nj; j++)
    {
      int q1 = std::max(0, j - ir) - j;
      int q2 = std::min(nj, j + ir + 1) - j;
      for (int p = p1; p < p2; p++)
        for (int q = q1; q < q2; q++)
        {
          float v = array(i + p, j + q) * k(p + ir, q + ir);
          array_new(i, j) = std::max(array_new(i, j), v);
        }
    }
  }

  array = array_new;
}

void expand(Array &array, int ir, Array *p_mask)
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

void expand(Array &array, Array &kernel)
{
  Array array_new = array;
  int   ni = array.shape.x;
  int   nj = array.shape.y;

  int ri1 = (int)(0.5f * kernel.shape.x);
  int ri2 = kernel.shape.x - ri1 - 1;
  int rj1 = (int)(0.5f * kernel.shape.y);
  int rj2 = kernel.shape.y - rj1 - 1;

  for (int i = 0; i < ni; i++)
  {
    int p1 = std::max(0, i - ri1) - i;
    int p2 = std::min(ni, i + ri2 + 1) - i;
    for (int j = 0; j < nj; j++)
    {
      int q1 = std::max(0, j - rj1) - j;
      int q2 = std::min(nj, j + rj2 + 1) - j;
      for (int p = p1; p < p2; p++)
        for (int q = q1; q < q2; q++)
        {
          float v = array(i + p, j + q) * kernel(p + ri1, q + rj1);
          array_new(i, j) = std::max(array_new(i, j), v);
        }
    }
  }
  array = array_new;
}

void expand(Array &array, Array &kernel, Array *p_mask)
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

void fill_talus(Array &z, float talus, uint seed, float noise_ratio)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(1.f - noise_ratio,
                                            1.f + noise_ratio);

  std::vector<int>   di = DI;
  std::vector<int>   dj = DJ;
  std::vector<float> c = C;
  const uint         nb = di.size();

  // populate queue
  std::vector<int>   queue_i = {};
  std::vector<int>   queue_j = {};
  std::vector<float> queue_z = {};

  // trick to exclude border cells, to avoid checking out of bounds
  // indices
  set_borders(z, 10.f * z.max(), 2);

  for (int i = 2; i < z.shape.x - 2; i++)
    for (int j = 2; j < z.shape.y - 2; j++)
    {
      queue_i.push_back(i);
      queue_j.push_back(j);
      queue_z.push_back(z(i, j));
    }

  // sort queue by elevation
  std::vector<size_t> idx = argsort(queue_z);
  reindex_vector(queue_i, idx);
  reindex_vector(queue_j, idx);
  reindex_vector(queue_z, idx);

  // fill
  while (queue_i.size() > 0)
  {
    int i = queue_i.back();
    int j = queue_j.back();

    queue_i.pop_back();
    queue_j.pop_back();
    queue_z.pop_back();

    for (uint k = 0; k < nb; k++) // loop over neighbors
    {
      int   p = i + di[k];
      int   q = j + dj[k];
      float rd = dis(gen);
      float h = z(i, j) - c[k] * talus * rd;

      if (h > z(p, q))
      {
        z(p, q) = h;

        // sorting should be performed to insert this new cells at the
        // right position but it is much faster to put it at the end
        // (and does not change much the result)
        queue_i.push_back(p);
        queue_j.push_back(q);
        queue_z.push_back(z(p, q));
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

void gain(Array &array, float factor, Array *p_mask)
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

void gamma_correction(Array &array, float gamma, Array *p_mask)
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

  if (k != 0) // with smoothing
  {
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float v = (array(i, j) - amin(i, j)) / (amax(i, j) - amin(i, j));
        v = std::sqrt(v * v + k);
        array(i, j) = std::pow(v, gamma) * (amax(i, j) - amin(i, j)) +
                      amin(i, j);
      }
  }
  else // without smoothing
  {
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float v = (array(i, j) - amin(i, j)) / (amax(i, j) - amin(i, j));
        array(i, j) = std::pow(v, gamma) * (amax(i, j) - amin(i, j)) +
                      amin(i, j);
      }
  }
}

void gamma_correction_local(Array &array,
                            float  gamma,
                            int    ir,
                            Array *p_mask,
                            float  k)
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

void laplace(Array &array, float sigma, int iterations)
{
  for (int it = 0; it < iterations; it++)
  {
    Array delta = laplacian(array);
    array += sigma * delta;
  }
}

void laplace(Array &array, Array *p_mask, float sigma, int iterations)
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

void laplace_edge_preserving(Array &array,
                             float  talus,
                             Array *p_mask,
                             float  sigma,
                             int    iterations)
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
  case (5):
    kernel = {0.0625f, -0.25f, 0.375f, -0.25f, 0.0625f};
    break;

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

  df = convolve1d_i(df, kernel);
  df = convolve1d_j(df, kernel);

  array = array - sigma * df;
}

void make_binary(Array &array, float threshold)
{
  auto lambda = [&threshold](float a)
  { return std::abs(a) > threshold ? 1.f : 0.f; };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void recast_canyon(Array &array, const Array &vcut, float gamma)
{
  auto lambda = [&gamma](float a, float b)
  { return a > b ? a : b * std::pow(a / b, gamma); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 vcut.vector.begin(),
                 array.vector.begin(),
                 lambda);
}

void recast_canyon(Array &array, const Array &vcut, Array *p_mask, float gamma)
{
  if (!p_mask)
    recast_canyon(array, vcut, gamma);
  else
  {
    Array array_f = array;
    recast_canyon(array_f, vcut, gamma);
    array = lerp(array, array_f, *(p_mask));
  }
}

void recast_canyon(Array &array, float vcut, float gamma)
{
  auto lambda = [&vcut, &gamma](float a)
  { return a > vcut ? a : vcut * std::pow(a / vcut, gamma); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void recast_canyon(Array &array, float vcut, Array *p_mask, float gamma)
{
  if (!p_mask)
    recast_canyon(array, vcut, gamma);
  else
  {
    Array array_f = array;
    recast_canyon(array_f, vcut, gamma);
    array = lerp(array, array_f, *(p_mask));
  }
}

void recast_peak(Array &array, int ir, float gamma, float k)
{
  Array ac = array;
  smooth_cpulse(ac, ir);
  array = maximum_smooth(array, ac, k);
  clamp_min(array, 0.f);
  array = ac * pow(array, gamma);
}

void recast_peak(Array &array, int ir, Array *p_mask, float gamma, float k)
{
  if (!p_mask)
    recast_peak(array, ir, gamma, k);
  else
  {
    Array array_f = array;
    recast_peak(array_f, ir, gamma, k);
    array = lerp(array, array_f, *(p_mask));
  }
}

void recast_rocky_slopes(Array &array,
                         float  talus,
                         int    ir,
                         float  amplitude,
                         uint   seed,
                         float  kw,
                         float  gamma,
                         Array *p_noise)
{
  // slope-based criteria
  Array c = select_gradient_binary(array, talus);
  smooth_cpulse(c, ir);

  if (!p_noise)
  {
    Array noise = fbm_perlin(array.shape, {kw, kw}, seed, 4, 0.f);
    gamma_correction_local(noise, gamma, ir, 0.1f);
    {
      int ir2 = (int)(ir / 4.f);
      if (ir2 > 1)
        gamma_correction_local(noise, gamma, ir2, 0.1f);
    }
    array += amplitude * noise * c;
  }
  else
    array += amplitude * (*p_noise) * c;
}

void recast_rocky_slopes(Array &array,
                         float  talus,
                         int    ir,
                         float  amplitude,
                         uint   seed,
                         float  kw,
                         Array *p_mask,
                         float  gamma,
                         Array *p_noise)
{
  {
    if (!p_mask)
      recast_rocky_slopes(array,
                          talus,
                          ir,
                          amplitude,
                          seed,
                          kw,
                          gamma,
                          p_noise);
    else
    {
      Array array_f = array;
      recast_rocky_slopes(array_f,
                          talus,
                          ir,
                          amplitude,
                          seed,
                          kw,
                          gamma,
                          p_noise);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

void recurve(Array                    &array,
             const std::vector<float> &t,
             const std::vector<float> &v)
{
  _1D::MonotonicInterpolator<float> interp;
  interp.setData(t, v);

  auto lambda = [&interp](float a) { return interp(a); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void recurve(Array                    &array,
             const std::vector<float> &t,
             const std::vector<float> &v,
             Array                    *p_mask)
{
  {
    if (!p_mask)
      recurve(array, t, v);
    else
    {
      Array array_f = array;
      recurve(array_f, t, v);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

void recurve_bexp(Array &array, float tau)
{
  float c = -1.f / tau;
  auto  lambda = [&c](float a) { return 1.f - std::exp(c * a); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void recurve_bexp(Array &array, float tau, Array *p_mask)
{
  {
    if (!p_mask)
      recurve_bexp(array, tau);
    else
    {
      Array array_f = array;
      recurve_bexp(array_f, tau);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

void recurve_exp(Array &array, float tau)
{
  float c = -1.f / tau;
  auto  lambda = [&c](float a) { return std::exp(c * (1.f - a)); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void recurve_exp(Array &array, float tau, Array *p_mask)
{
  {
    if (!p_mask)
      recurve_exp(array, tau);
    else
    {
      Array array_f = array;
      recurve_exp(array_f, tau);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

void recurve_kura(Array &array, float a, float b)
{
  auto lambda = [&a, &b](float v)
  { return 1.f - std::pow(1.f - std::pow(v, a), b); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void recurve_kura(Array &array, float a, float b, Array *p_mask)
{
  {
    if (!p_mask)
      recurve_kura(array, a, b);
    else
    {
      Array array_f = array;
      recurve_kura(array_f, a, b);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

void recurve_s(Array &array)
{
  auto lambda = [](float a) { return a * a * (3.f - 2.f * a); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void recurve_s(Array &array, Array *p_mask)
{
  {
    if (!p_mask)
      recurve_s(array);
    else
    {
      Array array_f = array;
      recurve_s(array_f);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

void recurve_smoothstep_rational(Array &array, float n)
{
  auto lambda = [&n](float a)
  {
    float an = std::pow(a, n);
    return an / (an + std::pow(1.f - a, n));
  };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void recurve_smoothstep_rational(Array &array, float n, Array *p_mask)
{
  {
    if (!p_mask)
      recurve_smoothstep_rational(array, n);
    else
    {
      Array array_f = array;
      recurve_smoothstep_rational(array_f, n);
      array = lerp(array, array_f, *(p_mask));
    }
  }
}

void sharpen(Array &array, float ratio)
{
  Array lp = Array(array.shape);

  for (int i = 1; i < array.shape.x - 1; i++)
    for (int j = 1; j < array.shape.y - 1; j++)
    {
      lp(i, j) = 5.f * array(i, j) - array(i + 1, j) - array(i - 1, j) -
                 array(i, j - 1) - array(i, j + 1);
    }
  extrapolate_borders(lp);
  array = (1.f - ratio) * array + ratio * lp;
}

void sharpen(Array &array, Array *p_mask, float ratio)
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

void shrink(Array &array, int ir)
{
  float amax = array.max();
  array = amax - array;
  expand(array, ir);
  array = amax - array;
}

void shrink(Array &array, int ir, Array *p_mask)
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

void shrink(Array &array, Array &kernel)
{
  float amax = array.max();
  array = amax - array;
  expand(array, kernel);
  array = amax - array;
}

void shrink(Array &array, Array &kernel, Array *p_mask)
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

void smooth_cpulse(Array &array, int ir, Array *p_mask)
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

void smooth_gaussian(Array &array, int ir, Array *p_mask)
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

void smooth_fill(Array &array, int ir, float k)
{
  Array array_smooth = array;
  smooth_cpulse(array_smooth, ir);
  array = maximum_smooth(array, array_smooth, k);
}

void smooth_fill(Array &array, int ir, Array *p_mask, float k)
{
  if (!p_mask)
    smooth_fill(array, ir, k);
  else
  {
    Array array_f = array;
    smooth_fill(array_f, ir, k);
    array = lerp(array, array_f, *(p_mask));
  }
}

void smooth_fill_holes(Array &array, int ir)
{
  Array array_smooth = mean_local(array, ir);

  // mask based on concave regions
  Array mask = curvature_mean(array_smooth);
  clamp_min(mask, 0.f);
  make_binary(mask);

  int ic = (int)((float)ir / 2.f);
  if (ic > 1)
    smooth_cpulse(mask, ic);

  array = lerp(array, array_smooth, mask);
}

void smooth_fill_holes(Array &array, int ir, Array *p_mask)
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
  Array array_smooth = mean_local(array, ir);

  // mask based on concave regions
  Array mask = curvature_mean(array_smooth);
  clamp_max(mask, 0.f);
  make_binary(mask);

  int ic = (int)((float)ir / 2.f);
  if (ic > 0)
    smooth_cpulse(mask, ic);

  array = lerp(array, array_smooth, mask);
}

void smooth_fill_smear_peaks(Array &array, int ir, Array *p_mask)
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

void steepen(Array &array, float scale, int ir)
{
  Array dx = gradient_x(array) * ((float)array.shape.x * -scale);
  Array dy = gradient_y(array) * ((float)array.shape.y * -scale);

  smooth_cpulse(dx, ir);
  smooth_cpulse(dy, ir);

  warp(array, dx, dy);
}

void steepen(Array &array, float scale, Array *p_mask, int ir)
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
    array *= 1.f - dt * (ca * dx + sa * dy); // == du / dt = - u * du / dx
  }
}

void steepen_convective(Array &array,
                        float  angle,
                        Array *p_mask,
                        int    iterations,
                        int    ir,
                        float  dt)
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

} // namespace hmap
