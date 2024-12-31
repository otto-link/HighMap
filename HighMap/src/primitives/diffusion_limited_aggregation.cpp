/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/filters.hpp"
#include "highmap/math.hpp"

namespace hmap
{

Array diffusion_limited_aggregation(Vec2<int> shape,
                                    float     scale,
                                    uint      seed,
                                    float     seeding_radius,
                                    float     seeding_outer_radius_ratio,
                                    float     slope,
                                    float     noise_ratio)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0.f, 1.f);

  // neighbor search
  std::vector<int> di = {-1, 0, 0, 1, -1, -1, 1, 1};
  std::vector<int> dj = {0, 1, -1, 0, -1, 1, -1, 1};

  // --- work on a grid with a resolution defined by the 'scale'
  int ncells = std::max(1, (int)(1.f / scale));

  hmap::Vec2<int> shape_wrk = {ncells, ncells};
  Array           wrk = Array(shape_wrk);

  int   nwalkers = ncells * ncells;
  float ratio = std::pow(0.01f, 1.f / ncells);

  // seed the diffusion process (center of domain)
  int ic = (int)(0.5f * ncells);
  int jc = (int)(0.5f * ncells);
  wrk(ic, jc) = 1.f;

  for (int k = 0; k < nwalkers; k++)
  {
    // pick a random cell on a circle
    float theta = 2.f * M_PI * dis(gen);
    int   i = (int)(0.5f * shape_wrk.x +
                  seeding_radius *
                      (1.f + seeding_outer_radius_ratio * dis(gen)) *
                      (shape_wrk.x - 1.0) * std::cos(theta));
    int   j = (int)(0.5f * shape_wrk.y +
                  seeding_radius *
                      (1.f + seeding_outer_radius_ratio * dis(gen)) *
                      (shape_wrk.y - 1.0) * std::sin(theta));

    // random_walk
    bool keep_walking = true;

    while (i > 0 && j > 0 && i < shape_wrk.x - 1 && j < shape_wrk.y - 1 &&
           keep_walking)
    {
      // check neighbors for encounter with an already cell
      // touched by diffusion
      for (size_t p = 0; p < di.size(); p++)
        if (wrk(i + di[p], j + dj[p]) > 0.f)
        {
          wrk(i, j) = ratio * wrk(i + di[p], j + dj[p]);
          keep_walking = false;
          break;
        }

      // next step in random direction
      int p = (int)(std::floor(8.f * dis(gen)));

      i += di[p];
      j += dj[p];
    }
  }

  // clean-up, remove spurious values outward the seeding radius
  for (int j = 0; j < shape_wrk.y; j++)
    for (int i = 0; i < shape_wrk.x; i++)
    {
      float dx = (float)(i - ic) / (shape_wrk.x - 1);
      float dy = (float)(j - jc) / (shape_wrk.y - 1);
      float r = std::hypot(dx, dy);

      if (r > 0.95f * seeding_radius) wrk(i, j) = 0.f;
    }

  fill_borders(wrk);
  fill_talus(wrk, slope / (float)wrk.shape.x, seed, noise_ratio);
  extrapolate_borders(wrk, 2, 0.75f);

  // --- generate the output array within the requested shape

  Array z = wrk.resample_to_shape(shape); // output

  return z;
}

} // namespace hmap
