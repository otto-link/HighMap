/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#define _USE_MATH_DEFINES
#include <cmath>
#include <functional>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap
{

//----------------------------------------------------------------------
// Main operator
//----------------------------------------------------------------------

void stratify(Array             &z,
              std::vector<float> hs,
              std::vector<float> gamma,
              Array             *p_noise)
{

  if (p_noise)
    for (uint k = 0; k < hs.size() - 1; k++)
    {
      float dh = hs[k + 1] - hs[k];
      for (int i = 0; i < z.shape.x; i++)
        for (int j = 0; j < z.shape.y; j++)
        {
          float zt = z(i, j) - (*p_noise)(i, j);
          if ((zt >= hs[k]) and (zt < hs[k + 1]))
          {
            // scale to [0, 1], apply gamma correction and scale back
            float v = (zt - hs[k]) / dh;
            v = std::pow(v, gamma[k]);
            z(i, j) = hs[k] + v * dh + (*p_noise)(i, j);
          }
        }
    }
  else
    for (uint k = 0; k < hs.size() - 1; k++)
    {
      float dh = hs[k + 1] - hs[k];
      for (int i = 0; i < z.shape.x; i++)
        for (int j = 0; j < z.shape.y; j++)
          if ((z(i, j) >= hs[k]) and (z(i, j) < hs[k + 1]))
          {
            float v = (z(i, j) - hs[k]) / dh;
            v = std::pow(v, gamma[k]);
            z(i, j) = hs[k] + v * dh;
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

    if (k == 0)
    {
      std::vector<float> hs =
          linspace_jitted(zmin, zmax, nlevels, strata_noise[k], seed++);
      std::vector<float> gamma = random_vector(gamma_list[k] - gamma_noise[k],
                                               gamma_list[k] - gamma_noise[k],
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
        std::vector<float> gamma = random_vector(gamma_list[k] - gamma_noise[k],
                                                 gamma_list[k] - gamma_noise[k],
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

  for (int i = 0; i < z.shape.x; i++)
    for (int j = 0; j < z.shape.y; j++)
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

//----------------------------------------------------------------------
// Overloading
//----------------------------------------------------------------------

void stratify(Array &z, std::vector<float> hs, float gamma, Array *p_noise)
{
  // float to vector
  std::vector<float> gs(hs.size() - 1);
  for (auto &v : gs)
    v = gamma;

  stratify(z, hs, gs, p_noise);
}

} // namespace hmap
