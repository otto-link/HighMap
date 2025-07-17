/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>

#include "macrologger.h"

#include "highmap/interpolate1d.hpp"

namespace hmap
{

// helper
bool is_monotonic(const std::vector<float> &data)
{
  return std::is_sorted(data.begin(), data.end()) ||
         std::is_sorted(data.begin(), data.end(), std::greater<float>());
}

Interpolator1D::Interpolator1D(const std::vector<float> &x,
                               const std::vector<float> &y,
                               InterpolationMethod1D     method)
{
  if (x.size() != y.size() || x.size() < 2)
  {
    throw std::invalid_argument(
        "x and y must have the same size and contain at least two points.");
  }

  if (method == InterpolationMethod1D::STEFFEN && !is_monotonic(y))
  {
    throw std::invalid_argument(
        "Steffen interpolation requires monotonic y data.");
  }

  this->x_data = std::vector<double>(x.begin(), x.end());
  this->y_data = std::vector<double>(y.begin(), y.end());

  const size_t size = this->x_data.size();

  // store min and max values after double conversion to clamp input
  // interpolation values and avoid rounding issues
  this->xmin = *std::min_element(this->x_data.begin(), this->x_data.end());
  this->xmax = *std::max_element(this->x_data.begin(), this->x_data.end());

  // initialize the GSL interpolation accelerator
  this->accel_ = gsl_interp_accel_alloc();

  // select the appropriate interpolation method
  switch (method)
  {

  case InterpolationMethod1D::AKIMA:
    this->interp = gsl_spline_alloc(gsl_interp_akima, size);
    break;

  case InterpolationMethod1D::AKIMA_PERIODIC:
    this->interp = gsl_spline_alloc(gsl_interp_akima_periodic, size);
    break;

  case InterpolationMethod1D::CUBIC:
    this->interp = gsl_spline_alloc(gsl_interp_cspline, size);
    break;

  case InterpolationMethod1D::CUBIC_PERIODIC:
    this->interp = gsl_spline_alloc(gsl_interp_cspline_periodic, size);
    break;

  case InterpolationMethod1D::LINEAR:
    this->interp = gsl_spline_alloc(gsl_interp_linear, size);
    break;

  case InterpolationMethod1D::POLYNOMIAL:
    this->interp = gsl_spline_alloc(gsl_interp_polynomial, size);
    break;

  case InterpolationMethod1D::STEFFEN:
    this->interp = gsl_spline_alloc(gsl_interp_steffen, size);
    break;

  default: throw std::invalid_argument("Unsupported interpolation method.");
  }

  gsl_spline_init(this->interp, this->x_data.data(), this->y_data.data(), size);
}

Interpolator1D::~Interpolator1D()
{
  gsl_spline_free(this->interp);
  gsl_interp_accel_free(this->accel_);
}

float Interpolator1D::operator()(float x) const
{
  return this->interpolate(x);
}

float Interpolator1D::interpolate(float x) const
{
  double xd = static_cast<double>(x);
  xd = std::clamp(xd, this->xmin, this->xmax);
  double result = gsl_spline_eval(this->interp, xd, accel_);
  return static_cast<float>(result);
}

} // namespace hmap
