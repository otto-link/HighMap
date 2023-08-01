/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <vector>

#include "macrologger.h"

#include "highmap/geometry.hpp"

namespace hmap
{

void expand_grid(std::vector<float> &x,
                 std::vector<float> &y,
                 std::vector<float> &value,
                 Vec4<float>         bbox)
{
  size_t n = x.size();
  x.resize(9 * n);
  y.resize(9 * n);
  value.resize(9 * n);

  float lx = bbox.b - bbox.a;
  float ly = bbox.d - bbox.c;

  int kshift = 0;
  for (int i = -1; i < 2; i++)
    for (int j = -1; j < 2; j++)
      if ((i != 0) or (j != 0))
      {
        kshift++;
        for (size_t k = 0; k < n; k++)
        {
          x[k + kshift * n] = x[k] + (float)i * lx;
          y[k + kshift * n] = y[k] + (float)j * ly;
          value[k + kshift * n] = value[k];
        }
      }
}

void random_grid(std::vector<float> &x,
                 std::vector<float> &y,
                 uint                seed,
                 Vec4<float>         bbox)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0.f, 1.f);
  size_t                                n = x.size();

  for (size_t k = 0; k < n; k++)
  {
    x[k] = dis(gen) * (bbox.b - bbox.a) + bbox.a;
    y[k] = dis(gen) * (bbox.d - bbox.c) + bbox.c;
  }
}

void random_grid(std::vector<float> &x,
                 std::vector<float> &y,
                 std::vector<float> &value,
                 uint                seed,
                 Vec4<float>         bbox)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0.f, 1.f);
  size_t                                n = x.size();

  for (size_t k = 0; k < n; k++)
  {
    x[k] = dis(gen) * (bbox.b - bbox.a) + bbox.a;
    y[k] = dis(gen) * (bbox.d - bbox.c) + bbox.c;
    value[k] = dis(gen);
  }
}

void random_grid_density(std::vector<float> &x,
                         std::vector<float> &y,
                         Array              &density,
                         uint                seed,
                         Vec4<float>         bbox)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0.f, 1.f);
  size_t                                n = x.size();

  size_t k = 0;
  while (k < n)
  {
    // take a random point within the grid and pick a random value:
    // if the value is smaller than the local density, the point is kept,
    // if not, try again...
    float xr = dis(gen);
    float yr = dis(gen);
    float rd = dis(gen);

    int i = (int)(xr * (density.shape.x - 1));
    int j = (int)(yr * (density.shape.y - 1));

    if (rd < density(i, j))
    {
      x[k] = xr * (bbox.b - bbox.a) + bbox.a;
      y[k] = yr * (bbox.d - bbox.c) + bbox.c;
      k++;
    }
  }
}

void random_grid_jittered(std::vector<float> &x,
                          std::vector<float> &y,
                          float               scale,
                          uint                seed,
                          Vec4<float>         bbox)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(-1.f, 1.f);
  size_t                                n = x.size();
  float                                 nsq = (float)std::pow((float)n, 0.5f);
  float                                 dx = 1.f / (nsq - 1.f);

  for (size_t k = 0; k < n; k++)
  {
    int j = (int)((float)k / nsq);

    x[k] = dx * ((float)(k - j * nsq) + scale * dis(gen));
    y[k] = dx * ((float)j + scale * dis(gen));

    x[k] = std::clamp(x[k], 0.f, 1.f);
    y[k] = std::clamp(y[k], 0.f, 1.f);

    x[k] = x[k] * (bbox.b - bbox.a) + bbox.a;
    y[k] = y[k] * (bbox.d - bbox.c) + bbox.c;
  }
}

} // namespace hmap
