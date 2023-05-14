#include <cmath>

#include "highmap/array.hpp"
#include "highmap/math.hpp"
#include "highmap/op.hpp"

namespace hmap
{

void gain(Array &array, float gain)
{
  auto lambda = [&gain](float x)
  {
    return x < 0.5 ? 0.5f * std::pow(2.f * x, gain)
                   : 1.f - 0.5f * std::pow(2.f * (1.f - x), gain);
  };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void gamma_correction(Array &array, float gamma)
{
  auto lambda = [&gamma](float x) { return std::pow(x, gamma); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void laplace(Array &array, float sigma, int iterations)
{
  Array lp = Array(array.shape);

  for (int it = 0; it < iterations; it++)
  {
    for (int i = 1; i < array.shape[0] - 1; i++)
    {
      for (int j = 1; j < array.shape[1] - 1; j++)
      {
        lp(i, j) = 4.f * array(i, j) - array(i + 1, j) - array(i - 1, j) -
                   array(i, j - 1) - array(i, j + 1);
      }
    }
    extrapolate_borders(lp);
    array = array - sigma * lp;
  }
}

void sharpen(Array &array, float ratio)
{
  Array lp = Array(array.shape);

  for (int i = 1; i < array.shape[0] - 1; i++)
  {
    for (int j = 1; j < array.shape[1] - 1; j++)
    {
      lp(i, j) = 5.f * array(i, j) - array(i + 1, j) - array(i - 1, j) -
                 array(i, j - 1) - array(i, j + 1);
    }
  }
  extrapolate_borders(lp);
  array = (1.f - ratio) * array + ratio * lp;
}

void steepen_convective(Array &array, float angle, int iterations, float dt)
{
  for (int it = 0; it < iterations; it++)
  {
    float alpha = angle / 180.f * M_PI;
    float ca = std::cos(alpha);
    float sa = std::sin(alpha);

    Array dx = gradient_x(array);
    Array dy = gradient_y(array);

    array = array + dt * (ca * dx + sa * dy);
  }
}

} // namespace hmap