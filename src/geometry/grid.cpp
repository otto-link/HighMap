/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <vector>

#include "macrologger.h"

#include "highmap/geometry.hpp"

namespace hmap
{

void random_grid(std::vector<float> &x,
                 std::vector<float> &y,
                 uint                seed,
                 std::vector<float>  bbox)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0.f, 1.f);
  size_t                                n = x.size();

  for (size_t k = 0; k < n; k++)
  {
    x[k] = dis(gen) * (bbox[1] - bbox[0]) + bbox[0];
    y[k] = dis(gen) * (bbox[3] - bbox[2]) + bbox[2];
  }
}

void random_grid_jittered(std::vector<float> &x,
                          std::vector<float> &y,
                          float               scale,
                          uint                seed,
                          std::vector<float>  bbox)
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

    x[k] = x[k] * (bbox[1] - bbox[0]) + bbox[0];
    y[k] = y[k] * (bbox[3] - bbox[2]) + bbox[2];
  }
}

} // namespace hmap
