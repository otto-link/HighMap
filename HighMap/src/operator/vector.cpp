/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <algorithm>
#include <list>
#include <numeric>
#include <random>
#include <vector>

#include "macrologger.h"

namespace hmap
{

std::vector<float> gradient1d(const std::vector<float> &v)
{
  size_t             n = v.size();
  std::vector<float> dv(n);

  for (size_t i = 1; i < n; i++)
    dv[i] = 0.5f * (v[i + 1] - v[i - 1]);
  dv[0] = v[1] - v[0];
  dv[n - 1] = v[n - 1] - v[n - 2];
  return dv;
}

void laplace1d(std::vector<float> &v, float sigma = 0.5f, int iterations = 1)
{
  size_t             n = v.size();
  std::vector<float> d(n);

  for (int it = 0; it < iterations; it++)
  {
    for (size_t i = 1; i < n - 1; i++)
      d[i] = 2.f * v[i] - v[i - 1] - v[i + 1];

    for (size_t i = 1; i < n - 1; i++)
      v[i] -= sigma * d[i];
  }
}

std::vector<float> linspace(float start, float stop, int num, bool endpoint)
{
  std::vector<float> v(num);
  float              dv;

  if (endpoint)
    dv = (stop - start) / (float)(num - 1);
  else
    dv = (stop - start) / (float)num;

  if (stop != start)
    for (int i = 0; i < num; i++)
      v[i] = start + (float)i * dv;
  else
    for (auto &r : v)
      r = start;

  return v;
}

std::vector<float> linspace_jitted(float start,
                                   float stop,
                                   int   num,
                                   float ratio,
                                   int   seed,
                                   bool  endpoint)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(-0.5f, 0.5f);
  std::vector<float>                    v(num);
  float                                 dv;

  if (endpoint)
    dv = (stop - start) / (float)(num - 1);
  else
    dv = (stop - start) / (float)num;

  for (int i = 0; i < num; i++)
  {
    v[i] = start + (float)i * dv;

    if (i > 0) v[i] += ratio * dis(gen) * dv;
  }
  return v;
}

std::vector<float> random_vector(float min, float max, int num, int seed)
{
  std::vector<float>                    v(num);
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(min, max);

  for (auto &v : v)
    v = dis(gen);
  return v;
}

} // namespace hmap
