/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <stdexcept>
#include <vector>

#include "highmap/array.hpp"
#include "highmap/kernels.hpp"
#include "highmap/math.hpp"
#include "highmap/operator.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array biweight(Vec2<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape.x - 1.f));
  int   rj = (int)(0.5f * ((float)shape.y - 1.f));

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      float xi = ((float)i - ri) / ((float)(ri + 1));
      float yi = ((float)j - rj) / ((float)(rj + 1));
      float r2 = xi * xi + yi * yi;
      if (r2 < 1.f) array(i, j) = (1.f - r2) * (1.f - r2);
    }

  return array;
}

Array blackman(Vec2<int> shape)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(0.f, 2.f * M_PI, shape.x);
  std::vector<float> y = linspace(0.f, 2.f * M_PI, shape.y);
  std::vector<float> wx(shape.x);
  std::vector<float> wy(shape.y);

  for (int i = 0; i < array.shape.x; i++)
    wx[i] = 0.42f - 0.5f * std::cos(x[i]) + 0.08f * std::cos(2.f * x[i]);

  for (int j = 0; j < array.shape.y; j++)
    wy[j] = 0.42f - 0.5f * std::cos(y[j]) + 0.08f * std::cos(2.f * y[j]);

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      array(i, j) = wx[i] * wy[j];

  return array;
}

Array cone(Vec2<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape.x - 1.f));
  int   rj = (int)(0.5f * ((float)shape.y - 1.f));

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      float r = std::hypot(xi / float(ri + 1), yi / float(rj + 1));
      array(i, j) = std::max(0.f, 1.f - r);
    }

  return array;
}

Array cone_smooth(Vec2<int> shape)
{
  Array array = cone(shape);
  array = almost_unit_identity(array);
  return array;
}

Array cone_talus(float height, float talus)
{
  // define output array size so that starting from an amplitude h,
  // zero is indeed reached with provided slope (talus) over the
  // half-width of the domain (since we build a cone)
  int   n = std::max(1, (int)(2.f * height / talus));
  Array array = Array(Vec2<int>(n, n));

  if (n > 0)
    array = height * cone({n, n});
  else
    array = 1.f;

  return array;
}

Array cubic_pulse(Vec2<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape.x - 1.f));
  int   rj = (int)(0.5f * ((float)shape.y - 1.f));

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      float r = std::hypot(xi / float(ri + 1), yi / float(rj + 1));

      if (r < 1.f) array(i, j) = 1.f - r * r * (3.f - 2.f * r);
    }

  return array;
}

std::vector<float> cubic_pulse_1d(int nk)
{
  std::vector<float> kernel_1d(nk);

  float sum = 0.f;
  float x0 = (float)nk / 2.f;
  for (int i = 0; i < nk; i++)
  {
    float x = std::abs((float)i - x0) / x0;
    kernel_1d[i] = 1.f - x * x * (3.f - 2.f * x);
    sum += kernel_1d[i];
  }

  // normalize
  for (int i = 0; i < nk; i++)
  {
    kernel_1d[i] /= sum;
  }

  return kernel_1d;
}

Array cubic_pulse_directional(Vec2<int> shape,
                              float     angle,
                              float     aspect_ratio,
                              float     anisotropy)
{
  Array array = Array(shape);

  // center and radii
  int ci = (int)(0.5f * ((float)shape.x - 1.f));
  int cj = (int)(0.5f * ((float)shape.y - 1.f));
  int ri = ci;
  int rj = cj * aspect_ratio;

  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      float xi = (float)i - ci;
      float yi = (float)j - cj;

      float xt = ca * xi + sa * yi;
      float yt = sa * xi - ca * yi;

      if (xt < 0.f) xt *= (1.f + anisotropy);

      float r = std::hypot(xt / float(ri + 1), yt / float(rj + 1));

      if (r < 1.f) array(i, j) = 1.f - r * r * (3.f - 2.f * r);
    }

  return array;
}

Array cubic_pulse_truncated(Vec2<int> shape, float slant_ratio, float angle)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape.x - 1.f));
  int   rj = (int)(0.5f * ((float)shape.y - 1.f));

  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      float xi = ((float)i - ri) / float(ri + 1);
      float yi = ((float)j - rj) / float(rj + 1);
      float r = std::hypot(xi, yi);

      float pulse = r < 1.f ? 1.f - r * r * (3.f - 2.f * r) : 0.f;

      float v = 1.f - (1.f / slant_ratio) * (xi * ca + yi * sa);
      float line = v < 0.f ? 0.f : (v < 1.f ? v * v * (3.f - 2.f * v) : 1.f);

      array(i, j) = std::max(0.f, line * pulse);
    }

  return array;
}

Array disk(Vec2<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape.x - 1.f));
  int   rj = (int)(0.5f * ((float)shape.y - 1.f));

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      if ((i - ri) * (i - ri) + (j - rj) * (j - rj) <= ri * rj)
        array(i, j) = 1.f;
    }

  return array;
}

Array gabor(Vec2<int> shape, float kw, float angle, bool quad_phase_shift)
{
  Array array = Array(shape);

  std::vector<float> x = linspace(-1.f, 1.f, array.shape.x, false);
  std::vector<float> y = linspace(-1.f, 1.f, array.shape.y, false);

  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  // gaussian_decay shape approximate using a cubic pulse
  Array cpulse = cubic_pulse(shape);

  if (!quad_phase_shift)
  {
    for (int j = 0; j < array.shape.y; j++)
      for (int i = 0; i < array.shape.x; i++)
        // "kw" and not "2 kw" since the domain is [-1, 1]
        array(i, j) = cpulse(i, j) *
                      std::cos(M_PI * kw * (x[i] * ca + y[j] * sa));
  }
  else
  {
    for (int j = 0; j < array.shape.y; j++)
      for (int i = 0; i < array.shape.x; i++)
        // "kw" and not "2 kw" since the domain is [-1, 1]
        array(i, j) = cpulse(i, j) *
                      std::sin(M_PI * kw * (x[i] * ca + y[j] * sa));
  }

  return array;
}

Array gabor_dune(Vec2<int> shape,
                 float     kw,
                 float     angle,
                 float     xtop,
                 float     xbottom)
{
  Array array = Array(shape);

  // do not start at '0' to avoid issues with modulo operator
  std::vector<float> x = linspace(1.f, 2.f, array.shape.x, false);
  std::vector<float> y = linspace(1.f, 2.f, array.shape.y, false);

  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  // gaussian_decay shape approximate using a cubic pulse
  Array cpulse = cubic_pulse(shape);

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      float xp = std::fmod(kw * (x[i] * ca + y[j] * sa), 1.f);
      float yp = 0.f;

      if (xp < xtop)
      {
        float r = xp / xtop;
        yp = r * r * (3.f - 2.f * r);
      }
      else if (xp < xbottom)
      {
        float r = (xp - xbottom) / (xtop - xbottom);
        yp = r * r * (2.f - r);
      }

      array(i, j) = cpulse(i, j) * yp;
    }

  return array;
}

Array lorentzian(Vec2<int> shape, float footprint_threshold)
{
  Array array = Array(shape);
  float cross_width = std::sqrt(1.f / (1.f / footprint_threshold - 1.f));
  float cw2 = 1.f / (cross_width * cross_width);

  for (int j = 0; j < shape.y; j++)
    for (int i = 0; i < shape.x; i++)
    {
      float x = 2.f * (float)i / (float)shape.x - 1.f;
      float y = 2.f * (float)j / (float)shape.y - 1.f;
      float r2 = x * x + y * y;
      array(i, j) = 1.f / (1.f + r2 * cw2);
    }

  return array;
}

Array hann(Vec2<int> shape)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(0.f, 2.f * M_PI, shape.x);
  std::vector<float> y = linspace(0.f, 2.f * M_PI, shape.y);

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      array(i, j) = (0.5f - 0.5f * std::cos(x[i])) *
                    (0.5f - 0.5f * std::cos(y[j]));

  array.infos();

  return array;
}

Array lorentzian_compact(Vec2<int> shape)
{
  Array array = Array(shape);

  for (int j = 0; j < shape.y; j++)
    for (int i = 0; i < shape.x; i++)
    {
      float x = 2.f * (float)i / (float)shape.x - 1.f;
      float y = 2.f * (float)j / (float)shape.y - 1.f;
      float r2 = x * x + y * y;
      array(i, j) = r2 < 1.f ? (1.f - r2) / (1.f + 4.f * r2) : 0.f;
    }

  return array;
}

Array sinc_radial(Vec2<int> shape, float kw)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(-kw * M_PI, kw * M_PI, shape.x);
  std::vector<float> y = linspace(-kw * M_PI, kw * M_PI, shape.y);

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      float r = std::hypot(x[i], y[j]);
      array(i, j) = r == 0.f ? 1.f : std::sin(r) / r;
    }

  return array;
}

Array sinc_separable(Vec2<int> shape, float kw)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(-kw * M_PI, kw * M_PI, shape.x);
  std::vector<float> y = linspace(-kw * M_PI, kw * M_PI, shape.y);
  std::vector<float> wx(shape.x);
  std::vector<float> wy(shape.y);

  for (int i = 0; i < array.shape.x; i++)
    wx[i] = x[i] == 0.f ? 1.f : std::sin(x[i]) / x[i];

  for (int j = 0; j < array.shape.y; j++)
    wy[j] = y[j] == 0.f ? 1.f : std::sin(y[j]) / y[j];

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      array(i, j) = wx[i] * wy[j];

  return array;
}

Array smooth_cosine(Vec2<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape.x - 1.f));
  int   rj = (int)(0.5f * ((float)shape.y - 1.f));

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      float r = M_PI * std::hypot(xi / float(ri + 1), yi / float(rj + 1));
      if (r < M_PI) array(i, j) = 0.5f + 0.5f * std::cos(r);
    }

  return array;
}

Array square(Vec2<int> shape)
{
  return constant(shape, 1.f);
}

Array tricube(Vec2<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape.x - 1.f));
  int   rj = (int)(0.5f * ((float)shape.y - 1.f));

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      float r = std::hypot(xi / float(ri + 1), yi / float(rj + 1));
      if (r < 1.f) array(i, j) = std::pow(1.f - std::pow(r, 3.f), 3.f);
    }

  return array;
}

// generic function

Array get_kernel(Vec2<int> shape, KernelType kernel_type)
{
  switch (kernel_type)
  {
  case KernelType::BIWEIGHT: return biweight(shape);
  case KernelType::CUBIC_PULSE: return cubic_pulse(shape);
  case KernelType::CONE: return cone(shape);
  case KernelType::CONE_SMOOTH: return cone_smooth(shape);
  case KernelType::DISK: return disk(shape);
  case KernelType::LORENTZIAN: return lorentzian(shape);
  case KernelType::SMOOTH_COSINE: return smooth_cosine(shape);
  case KernelType::SQUARE: return square(shape);
  case KernelType::TRICUBE: return tricube(shape);
  default: throw std::invalid_argument("Unknown kernel type");
  }
}

} // namespace hmap
