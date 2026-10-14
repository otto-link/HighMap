/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <cmath>
#include <vector>

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/hydrology.hpp"
#include "highmap/internal/validation.hpp"
#include "highmap/local_metrics.hpp"
#include "highmap/math.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"
#include "highmap/texture.hpp"

#include <numbers>

namespace hmap
{

Texture terrain_texture(const Array                &z,
                        const Array                *p_deposition,
                        const TerrainTextureParams &p,
                        const glm::vec4            &bbox,
                        float                       zmin,
                        float                       zmax)
{
  if (!validate_non_empty(z)) return Texture();

  // resolve elevation range
  if (zmin > zmax)
  {
    const glm::vec2 z_range = z.range();
    zmin = z_range.x;
    zmax = z_range.y;
  }

  const glm::ivec2 shape = z.shape;
  const int        nx = shape.x;
  const int        ny = shape.y;
  const int        n = nx * ny;

  // pixel radii computed from a single reference scale (region radius)
  const int ir_region = std::max(1, int(p.region_radius * float(nx)));
  const int ir_crest = std::max(1, int(ir_region / 5.f));
  const int ir_fine = std::max(1, int(ir_region / 20.f));
  const int ir_smooth = std::max(1, int(ir_region / 25.6f));

  const float zrange = std::max(zmax - zmin, 1e-6f);

  // --- Terrain analysis

  // slope and fall lines, read from a lightly smoothed surface
  Array zs = z;
  smooth_cpulse(zs, ir_smooth);

  Array gx, gy;
  Array slope = gradient_norm(zs, &gx, &gy) * float(nx);
  gx *= float(nx);
  gy *= float(nx);

  const Array fine = topographic_residual(z, ir_fine, true);
  const Array crest = topographic_residual(z, ir_crest, true);
  const Array region = topographic_residual(z, ir_region, true);

  // drainage on a log scale: 0 on the ridges, 1 in the main river
  Array flow = flow_accumulation_dinf(zs, 0.1f);
  {
    const float lmax = std::log(std::max(flow.max(), 2.f));
    for (float &v : flow.vector)
      v = std::log(std::max(v, 1.f)) / lmax;
  }
  Array moisture = flow;
  smooth_cpulse(moisture, ir_crest);

  // normalize deposition into [0, 1] relative to the 90th percentile
  // of non-zero cells; this prevents extreme outlier deposits from
  // dominating the scale while preserving sparsity where no sediment
  // accumulated
  Array deposit(shape, 0.f);
  if (p_deposition && p_deposition->shape == shape)
  {
    // reference scale: 90th percentile among active deposits
    // (ignoring zero cells)
    const float ref = p_deposition->range_percentile(0.f, 0.9f, 1024, true).y;
    deposit = *p_deposition / std::max(ref, 1e-6f);
    clamp(deposit, 0.f, 1.f);
  }

  // --- Noise

  const auto noise = [&](float kw, int seed_offset, int octaves)
  {
    const float k = kw * p.noise_scale;
    Array       a = noise_fbm(NoiseType::PERLIN,
                        shape,
                              {k, k},
                        p.seed + seed_offset,
                        octaves,
                        0.f,
                        0.5f,
                        2.f,
                        nullptr,
                        nullptr,
                        nullptr,
                        bbox);
    normalize_and_clip(a);
    return a;
  };

  const Array n_large = noise(3.f, 0, 4);
  const Array n_mid = noise(12.f, 1, 5);
  const Array n_fine = noise(64.f, 2, 3);
  const Array n_rill = noise(160.f, 3, 2);
  const Array n_grain = noise(320.f, 4, 2);

  // --- Fall lines

  // bare high crests: their colour runs down the slopes below them
  std::vector<float> source(n);
  for (int k = 0; k < n; ++k)
  {
    const float h = (z.vector[k] - zmin) / zrange;
    source[k] = smoothstep3(0.6f * crest.vector[k] + 0.6f * region.vector[k],
                            0.05f,
                            0.35f) *
                smoothstep3(h + 0.15f * n_mid.vector[k], 0.3f, 0.75f) *
                (0.75f + 0.25f * n_fine.vector[k]);
  }

  const float streak_px = p.streak_length * nx;
  const int   streak_steps = std::clamp(int(streak_px), 0, nx);
  const float streak_step = streak_steps ? streak_px / streak_steps : 0.f;
  const int   rill_steps = std::clamp(nx / 96, 3, 24);

  std::vector<float> streak(n), rill(n);

#pragma omp parallel for
  for (int j = 0; j < ny; ++j)
    for (int i = 0; i < nx; ++i)
    {
      // walk up the fall line, one cell per step when step is 1
      const auto walk = [&](int steps, float step, auto &&visit)
      {
        float x = (float)i, y = (float)j;
        for (int s = 1; s <= steps; ++s)
        {
          const int   k = int(y + 0.5f) * nx + int(x + 0.5f);
          const float g = std::hypot(gx.vector[k], gy.vector[k]);
          if (g < 1e-6f) return;

          x += step * gx.vector[k] / g;
          y += step * gy.vector[k] / g;
          if (x < 0.f || y < 0.f || x > nx - 1.f || y > ny - 1.f) return;

          visit(s, int(y + 0.5f) * nx + int(x + 0.5f));
        }
      };

      const int k = j * nx + i;

      // fine noise averaged along the fall line: rills and gullies
      float sum = n_rill.vector[k];
      int   count = 1;
      walk(rill_steps,
           1.f,
           [&](int, int kk)
           {
             sum += n_rill.vector[kk];
             ++count;
           });
      rill[k] = std::clamp(sum / std::sqrt((float)count), -1.f, 1.f);

      float best = source[k];
      walk(streak_steps,
           streak_step,
           [&](int s, int kk)
           {
             best = std::max(best,
                             source[kk] * (1.f - s / (streak_steps + 1.f)));
           });
      streak[k] = best;
    }

  // --- Colours

  Texture out(shape, 4);

  const float cn = p.color_noise;
  const float two_pi = 2.f * std::numbers::pi_v<float>;

#pragma omp parallel for
  for (int k = 0; k < n; ++k)
  {
    const float h = (z.vector[k] - zmin) / zrange;
    const float s = slope.vector[k] / p.rock_slope; // 1 where rock takes over
    const float c = crest.vector[k];
    const float r = region.vector[k];
    const float nl = n_large.vector[k];
    const float nm = n_mid.vector[k];
    const float nf = n_fine.vector[k];
    const float ng = n_grain.vector[k];
    const float rl = rill[k];

    // grass: lush where water gathers, dry on high convex ground; a narrow,
    // noisy transition keeps the patches crisp
    const float wet = 0.5f + 0.6f * moisture.vector[k] - 0.35f * r - 0.2f * c +
                      0.15f * nl;
    const float dry = smoothstep3(1.f - wet + 0.35f * (h - 0.4f) + 0.12f * nm +
                                      0.08f * nf,
                                  0.42f,
                                  0.58f);
    glm::vec3   col = glm::mix(
        shade_color(p.grass, cn * (0.3f * nm + 0.3f * nf + 0.2f * ng)),
        shade_color(p.dry, cn * (0.3f * nm + 0.3f * nf + 0.2f * ng)),
        dry);

    // soil on moderate slopes and bare bumps
    const float soil = smoothstep3(s + 0.2f * nm + 0.1f * nf +
                                       0.25f * std::max(c, 0.f),
                                   0.35f,
                                   0.6f);
    col = glm::mix(
        col,
        shade_color(p.soil, cn * (0.25f * rl + 0.3f * nm + 0.15f * ng)),
        soil);

    // sediment along the drainage lines and in crisp deposits
    const float t0 = 0.8f - 0.5f * p.flow_width;
    float       sed = p.flow *
                smoothstep3(flow.vector[k] + 0.05f * nf, t0, t0 + 0.08f);
    sed = std::max(sed,
                   p.deposition *
                       smoothstep3(deposit.vector[k] + 0.15f * nf, 0.4f, 0.8f));
    sed *= 1.f - smoothstep3(s, 0.6f, 1.f);
    col = glm::mix(
        col,
        shade_color(p.sediment, cn * (0.4f * nf + 0.2f * nm + 0.2f * ng)),
        sed);

    // rock on steep ground: ragged edges, strata, a hint of rills
    const float edge = p.rock_edge;
    const float rock = smoothstep3(s + 0.25f * nm + 0.1f * nf +
                                       0.3f * std::max(c, 0.f) * h,
                                   1.f - edge,
                                   1.f + edge);
    const float band = std::sin(two_pi * (14.f * h + 0.4f * nl + 0.1f * nm));
    col = glm::mix(col,
                   shade_color(p.rock,
                               0.35f * p.strata * band +
                                   cn * (0.2f * rl + 0.3f * nf + 0.15f * ng) +
                                   0.25f * c),
                   rock);

    // ridge colour spilling down the slopes
    const float spill = p.streaks *
                        smoothstep3(streak[k] * (0.75f + 0.25f * (1.f + nf)),
                                    0.25f,
                                    0.65f) *
                        smoothstep3(s, 0.15f, 0.4f);
    col = glm::mix(
        col,
        shade_color(p.ridge, cn * (0.3f * nf + 0.15f * rl + 0.15f * ng)),
        spill);

    // snow on high ground, sliding off the steep parts
    const float snow = p.snow_amount *
                       smoothstep3(h - p.snow_line + 0.08f * nm + 0.04f * nf -
                                       0.15f * std::max(0.f, s - 0.9f) -
                                       0.05f * c,
                                   -0.04f,
                                   0.04f);
    col = glm::mix(col,
                   shade_color(p.snow, 0.15f * nf - 0.3f * std::max(-c, 0.f)),
                   snow);

    // relief: dark gullies, crisp detail, grain, then a slow warm / cool drift
    col *= 1.f - p.cavity * 0.35f * std::clamp(-0.7f * c - 0.4f * r, 0.f, 1.f);
    col *= 1.f + p.detail * 0.22f * fine.vector[k];
    col *= 1.f + cn * (0.05f * nm + 0.06f * nf + 0.05f * ng);
    col += cn * 0.015f * nl * glm::vec3(1.f, 0.6f, 0.1f);

    out.channels[0].vector[k] = std::clamp(col.r, 0.f, 1.f);
    out.channels[1].vector[k] = std::clamp(col.g, 0.f, 1.f);
    out.channels[2].vector[k] = std::clamp(col.b, 0.f, 1.f);
    out.channels[3].vector[k] = 1.f;
  }

  return out;
}

} // namespace hmap
