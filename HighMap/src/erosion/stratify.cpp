/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <functional>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/blending.hpp"
#include "highmap/math.hpp"
#include "highmap/operator.hpp"

namespace hmap
{

void stratify(Array             &z,
              std::vector<float> hs,
              std::vector<float> gamma,
              Array             *p_noise)
{

  for (int j = 0; j < z.shape.y; j++)
    for (int i = 0; i < z.shape.x; i++)
      for (uint k = 0; k < hs.size() - 1; k++)
      {
        float dh = hs[k + 1] - hs[k];

        float dn = p_noise ? (*p_noise)(i, j) * dh : 0.f;
        float zt = z(i, j) - dn;

        if ((zt >= hs[k]) and (zt < hs[k + 1]))
        {
          // scale to [0, 1], apply gamma correction and scale back
          float v = (zt - hs[k]) / dh;

          float ce = 50.f / gamma[k];
          v = std::pow(v, gamma[k]) * (1.f - std::exp(-ce * v));

          z(i, j) = dn + hs[k] + v * dh;
          break;
        }
      }
}

void stratify(Array             &z,
              Array             *p_mask,
              std::vector<float> hs,
              std::vector<float> gamma,
              Array             *p_noise)
{
  if (!p_mask)
    stratify(z, hs, gamma, p_noise);
  else
  {
    Array z_f = z;
    stratify(z_f, hs, gamma, p_noise);
    z = lerp(z, z_f, *(p_mask));
  }
}

void stratify_multiscale(Array             &z,
                         float              zmin,
                         float              zmax,
                         std::vector<int>   n_strata,
                         std::vector<float> strata_noise,
                         std::vector<float> gamma_list,
                         std::vector<float> gamma_noise,
                         uint               seed,
                         Array             *p_mask,
                         Array             *p_noise)
{
  std::vector<float> hs_prev = {};

  for (size_t k = 0; k < n_strata.size(); k++)
  {
    int nlevels = n_strata[k] + 1;

    float gamma_min = std::max(0.001f, gamma_list[k] * (1.f - gamma_noise[k]));
    float gamma_max = gamma_list[k] * (1.f + gamma_noise[k]);

    if (k == 0)
    {
      std::vector<float> hs = linspace_jitted(zmin,
                                              zmax,
                                              nlevels,
                                              strata_noise[k],
                                              seed++);
      std::vector<float> gamma = random_vector(gamma_min,
                                               gamma_max,
                                               nlevels - 1,
                                               seed++);

      stratify(z, p_mask, hs, gamma, p_noise);

      // backup elevations for next iterations
      hs_prev.resize(hs.size());
      for (size_t i = 0; i < hs.size(); i++)
        hs_prev[i] = hs[i];
    }
    else
    {
      std::vector<float> hs_bckp = {};

      for (size_t r = 0; r < hs_prev.size() - 1; r++)
      {
        std::vector<float> hs = linspace_jitted(hs_prev[r],
                                                hs_prev[r + 1],
                                                nlevels,
                                                strata_noise[k],
                                                seed++);
        std::vector<float> gamma = random_vector(gamma_min,
                                                 gamma_max,
                                                 (int)hs.size() - 1,
                                                 seed++);

        stratify(z, p_mask, hs, gamma, p_noise);

        for (auto &v : hs)
          hs_bckp.push_back(v);
      }

      // backup elevations for next iterations
      hs_prev.resize(hs_bckp.size());
      for (size_t i = 0; i < hs_bckp.size(); i++)
        hs_prev[i] = hs_bckp[i];
    }
  }
}

void stratify(Array &z, std::vector<float> hs, float gamma, Array *p_noise)
{
  // float to vector
  std::vector<float> gs(hs.size() - 1);
  for (auto &v : gs)
    v = gamma;

  stratify(z, hs, gs, p_noise);
}

void stratify(Array &z,
              Array &partition,
              int    nstrata,
              float  strata_noise,
              float  gamma,
              float  gamma_noise,
              int    npartitions,
              uint   seed,
              float  mixing_gain_factor,
              Array *p_noise,
              float  vmin,
              float  vmax)
{
  // redefine min/max if sentinels values are detected
  if (vmax < vmin)
  {
    vmin = z.min();
    vmax = z.max();
  }

  // generate the stratifications
  std::vector<Array> zs = {};

  for (int k = 0; k < npartitions; k++)
  {
    std::vector<float> hs = linspace_jitted(vmin,
                                            vmax,
                                            nstrata + 1,
                                            strata_noise,
                                            seed++);

    float gmin = std::max(0.01f, gamma * (1.f - gamma_noise));
    float gmax = gamma * (1.f + gamma_noise);

    std::vector<float> gs = random_vector(gmin, gmax, nstrata, seed++);

    Array ztmp = z;
    stratify(ztmp, hs, gs, p_noise);
    zs.push_back(ztmp);
  }

  // mix everything
  std::vector<const Array *> zs_ptr = {};
  for (auto &a : zs)
    zs_ptr.push_back(&a);

  z = mixer(partition, zs_ptr, mixing_gain_factor);
}

void stratify_oblique(Array             &z,
                      std::vector<float> hs,
                      std::vector<float> gamma,
                      float              talus,
                      float              angle,
                      Array             *p_noise)
{
  // elevation shift due to the oblicity
  Array shift = Array(z.shape);

  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  for (int j = 0; j < z.shape.y; j++)
    for (int i = 0; i < z.shape.x; i++)
      shift(i, j) = talus * (ca * (float)i + sa * (float)j);

  Array zs = z + shift;

  // add data to the strata elevations (and gamma) to ensure they span the whole
  // range of elevation after the shift
  float zs_min = zs.min();
  float zs_max = zs.max();

  std::vector<float> hs_o = hs;
  std::vector<float> gamma_o = gamma;

  // reset elevation values
  for (auto &v : hs_o)
    v = v - hs[0] + zs_min;

  float hs_o_min = *std::min_element(hs_o.begin(), hs_o.end());
  float hs_o_max = *std::max_element(hs_o.begin(), hs_o.end());

  if (hs_o.size() > 1)
  {
    int   nstrata = hs_o.size();
    float dh = hs_o[1] - hs_o[0];
    for (int n = 0; n < std::ceil((zs_max - hs_o_max) / (hs_o_max - hs_o_min));
         n++)
      for (int k = 0; k < nstrata; k++)
      {
        hs_o.push_back(hs_o[k] + (hs_o_max - hs_o_min + dh) * (n + 1));
        gamma_o.push_back(gamma_o[k]);
      }
  }

  stratify(zs, hs_o, gamma_o, p_noise);
  z = zs - shift;
}

void stratify_oblique(Array             &z,
                      Array             *p_mask,
                      std::vector<float> hs,
                      std::vector<float> gamma,
                      float              talus,
                      float              angle,
                      Array             *p_noise)
{
  if (!p_mask)
    stratify_oblique(z, hs, gamma, talus, angle, p_noise);
  else
  {
    Array z_f = z;
    stratify_oblique(z_f, hs, gamma, talus, angle, p_noise);
    z = lerp(z, z_f, *(p_mask));
  }
}

} // namespace hmap
