/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <random>
#include <vector>

#include "cl_wrapper/device_manager.hpp"
#include "cl_wrapper/run.hpp"

#include "highmap/array.hpp"
#include "highmap/filters.hpp"
#include "highmap/internal/validation.hpp"
#include "highmap/math/array.hpp"
#include "highmap/morphology.hpp"
#include "highmap/operator.hpp"
#include "highmap/range.hpp"
#include "highmap/transform.hpp"

namespace
{

float sample_bilinear(const hmap::Array &arr, float x, float y)
{
  int ix0 = static_cast<int>(std::floor(x));
  int iy0 = static_cast<int>(std::floor(y));

  ix0 = std::clamp(ix0, 0, arr.shape.x - 1);
  iy0 = std::clamp(iy0, 0, arr.shape.y - 1);

  int ix1 = std::min(ix0 + 1, arr.shape.x - 1);
  int iy1 = std::min(iy0 + 1, arr.shape.y - 1);

  float fx = x - static_cast<float>(ix0);
  float fy = y - static_cast<float>(iy0);

  float v00 = arr(ix0, iy0);
  float v10 = arr(ix1, iy0);
  float v01 = arr(ix0, iy1);
  float v11 = arr(ix1, iy1);

  float top = (1.f - fx) * v00 + fx * v10;
  float bottom = (1.f - fx) * v01 + fx * v11;

  return (1.f - fy) * top + fy * bottom;
}

void compute_directional_parameters(int                 directions,
                                    float               angle_deg,
                                    float               falloff,
                                    float               skew,
                                    float               angular_jitter,
                                    float               falloff_jitter,
                                    std::uint32_t       seed,
                                    std::vector<float> &thetas,
                                    std::vector<float> &slopes)
{
  thetas.resize(directions);
  slopes.resize(directions);

  float base_angle_rad = angle_deg * (static_cast<float>(M_PI) / 180.f);

  std::mt19937                          rng(seed);
  std::uniform_real_distribution<float> dist_jitter(-0.5f, 0.5f);

  float dir_step = 2.f * static_cast<float>(M_PI) / static_cast<float>(directions);

  for (int k = 0; k < directions; ++k)
  {
    float th = base_angle_rad + static_cast<float>(k) * dir_step;

    if (angular_jitter > 0.f)
    {
      float delta_th = dist_jitter(rng) * dir_step * angular_jitter;
      th += delta_th;
    }

    float delta_from_base = th - base_angle_rad;
    float q = std::cos(delta_from_base);
    float s = falloff * std::exp(-skew * q);

    if (falloff_jitter > 0.f)
    {
      float delta_s = dist_jitter(rng) * falloff_jitter;
      s *= (1.f + delta_s);
    }

    thetas[k] = th;
    slopes[k] = std::max(s, 1e-6f);
  }
}

} // namespace

namespace hmap::gpu
{

Array distance_shaper(const Array         &array,
                      float                falloff,
                      int                  directions,
                      float                skew,
                      float                angle,
                      hmap::DistanceMode   mode,
                      hmap::DistanceMethod method,
                      float                angular_jitter,
                      float                falloff_jitter,
                      float                threshold,
                      bool                 invert_input,
                      bool                 invert_output,
                      float                mix_ratio,
                      std::uint32_t        seed,
                      const Array         *p_mask,
                      const Array         *p_dx,
                      const Array         *p_dy)
{
  if (!validate_non_empty(array)) return Array();
  if (p_mask && !validate_same_shape(array, *p_mask)) return Array(array.shape);
  if (p_dx && !validate_same_shape(array, *p_dx)) return Array(array.shape);
  if (p_dy && !validate_same_shape(array, *p_dy)) return Array(array.shape);

  directions = std::max(1, directions);

  // --- Prepare work array

  Array work = array;

  if (invert_input)
  {
    float vmin = work.min();
    float vmax = work.max();
    work = (vmax + vmin) - work;
  }

  Array out(array.shape);

  // --- Classic distance transform method

  if (method == hmap::DistanceMethod::CLASSIC)
  {
    Array mask(work.shape);

    for (int idx = 0; idx < work.size(); ++idx)
    {
      mask.vector[idx] = (work.vector[idx] >= threshold) ? 1.f : 0.f;
    }

    Array dist = gpu::distance_transform_jfa(mask);

    out = work - falloff * dist;
    clamp_min(out, threshold);
  }
  else
  {
    // --- Real-Time directional distance projection method

    std::vector<float> thetas;
    std::vector<float> slopes;

    compute_directional_parameters(directions,
                                   angle,
                                   falloff,
                                   skew,
                                   angular_jitter,
                                   falloff_jitter,
                                   seed,
                                   thetas,
                                   slopes);

    float z_max = work.max();
    int   mode_int = (mode == hmap::DistanceMode::ASTERISK) ? 0 : 1;

    auto run = clwrapper::Run("distance_shaper_rt");

    run.bind_imagef("in", work.vector, array.shape.x, array.shape.y);
    run.bind_imagef("out", out.vector, array.shape.x, array.shape.y, true);

    run.bind_buffer<float>("thetas", thetas);
    run.bind_buffer<float>("slopes", slopes);

    run.bind_arguments(array.shape.x,
                       array.shape.y,
                       directions,
                       threshold,
                       z_max,
                       mode_int);

    run.write_buffer("thetas");
    run.write_buffer("slopes");

    run.execute({array.shape.x, array.shape.y});

    run.read_imagef("out");
  }

  // --- Invert output if requested

  if (invert_output)
  {
    float vmin = out.min();
    float vmax = out.max();
    out = (vmax + vmin) - out;
  }

  // --- Blend with original input

  if (mix_ratio < 1.f)
  {
    out = lerp(array, out, mix_ratio);
  }

  // --- Optional domain warping

  if (p_dx || p_dy)
  {
    gpu::warp(out, p_dx, p_dy);
  }

  // --- Optional masking

  if (p_mask)
  {
    out = lerp(array, out, *p_mask);
  }

  return out;
}

} // namespace hmap::gpu

namespace hmap
{

Array distance_shaper(const Array   &array,
                      float          falloff,
                      int            directions,
                      float          skew,
                      float          angle,
                      DistanceMode   mode,
                      DistanceMethod method,
                      float          angular_jitter,
                      float          falloff_jitter,
                      float          threshold,
                      bool           invert_input,
                      bool           invert_output,
                      float          mix_ratio,
                      std::uint32_t  seed,
                      const Array   *p_mask,
                      const Array   *p_dx,
                      const Array   *p_dy)
{
  if (!validate_non_empty(array)) return Array();

  if (clwrapper::DeviceManager::get_instance().is_ready())
  {
    return gpu::distance_shaper(array,
                                falloff,
                                directions,
                                skew,
                                angle,
                                mode,
                                method,
                                angular_jitter,
                                falloff_jitter,
                                threshold,
                                invert_input,
                                invert_output,
                                mix_ratio,
                                seed,
                                p_mask,
                                p_dx,
                                p_dy);
  }

  // --- CPU OpenMP Implementation

  directions = std::max(1, directions);

  Array work = array;

  if (invert_input)
  {
    float vmin = work.min();
    float vmax = work.max();
    work = (vmax + vmin) - work;
  }

  Array out(array.shape);

  if (method == DistanceMethod::CLASSIC)
  {
    Array mask(work.shape);

    for (int idx = 0; idx < work.size(); ++idx)
    {
      mask.vector[idx] = (work.vector[idx] >= threshold) ? 1.f : 0.f;
    }

    Array dist = distance_transform(mask);

    out = work - falloff * dist;
    clamp_min(out, threshold);
  }
  else
  {
    std::vector<float> thetas;
    std::vector<float> slopes;

    compute_directional_parameters(directions,
                                   angle,
                                   falloff,
                                   skew,
                                   angular_jitter,
                                   falloff_jitter,
                                   seed,
                                   thetas,
                                   slopes);

    float z_max = work.max();
    float max_dist = 1.41421356f * static_cast<float>(std::max(array.shape.x, array.shape.y));

    #pragma omp parallel for collapse(2)
    for (int j = 0; j < array.shape.y; ++j)
    {
      for (int i = 0; i < array.shape.x; ++i)
      {
        float val_orig = work(i, j);
        float best_val = val_orig;
        float second_val = -1e20f;

        for (int k = 0; k < directions; ++k)
        {
          float th = thetas[k];
          float s = slopes[k];

          float dir_x = std::cos(th);
          float dir_y = std::sin(th);
          float dir_max = val_orig;

          for (float r = 1.0f; r <= max_dist; r += 1.0f)
          {
            float px = static_cast<float>(i) - r * dir_x;
            float py = static_cast<float>(j) - r * dir_y;

            if (px < 0.f || px >= static_cast<float>(array.shape.x) ||
                py < 0.f || py >= static_cast<float>(array.shape.y))
            {
              break;
            }

            float h_up = sample_bilinear(work, px, py);

            if (h_up >= threshold)
            {
              float cand = h_up - r * s;

              if (cand > dir_max)
              {
                dir_max = cand;
              }
            }

            if (z_max - r * s <= dir_max)
            {
              break;
            }
          }

          if (dir_max > best_val)
          {
            second_val = best_val;
            best_val = dir_max;
          }
          else if (dir_max > second_val)
          {
            second_val = dir_max;
          }
        }

        float res = best_val;

        if (mode == DistanceMode::ASTERISK)
        {
          if (second_val > -1e19f && best_val > threshold)
          {
            float num = second_val - threshold;
            float den = best_val - threshold;

            if (den > 1e-5f)
            {
              float ratio = std::clamp(num / den, 0.0f, 1.0f);
              float cut = 0.35f * ratio * ratio * (second_val - threshold);
              res = std::max(val_orig, best_val - cut);
            }
          }
        }

        out(i, j) = res;
      }
    }
  }

  // --- Invert output if requested

  if (invert_output)
  {
    float vmin = out.min();
    float vmax = out.max();
    out = (vmax + vmin) - out;
  }

  // --- Blend with original input

  if (mix_ratio < 1.f)
  {
    out = lerp(array, out, mix_ratio);
  }

  // --- Optional domain warping

  if (p_dx || p_dy)
  {
    gpu::warp(out, p_dx, p_dy);
  }

  // --- Optional masking

  if (p_mask)
  {
    out = lerp(array, out, *p_mask);
  }

  return out;
}

} // namespace hmap
