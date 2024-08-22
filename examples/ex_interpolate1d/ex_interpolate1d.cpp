#include <iostream>

#include "highmap.hpp"

int main(void)
{
  std::vector<float> x = {0.f, 0.1f, 0.5f, 0.7f, 1.f};
  std::vector<float> y = {0.2f, 0.1f, 0.5f, 0.6f, 0.4f};

  std::vector<float> xi = hmap::linspace(0.f, 1.f, 50);

  hmap::Interpolator1D fl = hmap::Interpolator1D(
      x,
      y,
      hmap::InterpolationMethod1D::LINEAR);

  hmap::Interpolator1D fc = hmap::Interpolator1D(
      x,
      y,
      hmap::InterpolationMethod1D::CUBIC);

  hmap::Interpolator1D fa = hmap::Interpolator1D(
      x,
      y,
      hmap::InterpolationMethod1D::AKIMA);

  std::vector<float> yl, yc, ya;

  for (auto &x_ : xi)
  {
    yl.push_back(fl(x_));
    yc.push_back(fc(x_));
    ya.push_back(fa(x_));
  }

  // plots
  std::vector<float> v = std::vector<float>(xi.size(), 1.f);

  hmap::Path(xi, yl, v).to_png("ex_interpolate1d_0.png", {128, 128});
  hmap::Path(xi, yc, v).to_png("ex_interpolate1d_1.png", {128, 128});
  hmap::Path(xi, ya, v).to_png("ex_interpolate1d_2.png", {128, 128});
}
