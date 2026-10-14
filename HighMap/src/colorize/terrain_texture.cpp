/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <cmath>
#include <numbers>
#include <vector>

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/filters.hpp"
#include "highmap/hydrology.hpp"
#include "highmap/internal/validation.hpp"
#include "highmap/local_metrics.hpp"
#include "highmap/math.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"
#include "highmap/texture.hpp"

namespace hmap
{

Texture terrain_texture(const Array                &z,
                        const Array                *p_deposition,
                        const TerrainTextureParams &p,
                        const glm::vec4            &bbox)
{
  if (!validate_non_empty(z)) return Texture();

  const glm::ivec2 shape = z.shape;
  const int        nx    = shape.x;
  const int        ny    = shape.y;
  const int        n     = nx * ny;
  const auto       px    = [nx](float frac) { return std::max(1, int(frac * nx)); };

  const auto [it_min, it_max] = std::minmax_element(z.vector.begin(), z.vector.end());
  const float zmin            = *it_min;
  const float zrange          = std::max(*it_max - zmin, 1e-6f);

  // --- Terrain analysis

  // slope and fall lines, read from a lightly smoothed surface
  Array zs = z;
  smooth_cpulse(zs, px(1.f / 512.f));

  std::vector<float> gx(n), gy(n), slope(n);

#pragma omp parallel for
  for (int j = 0; j < ny; ++j)
    for (int i = 0; i < nx; ++i)
    {
      const int   ip = std::min(i + 1, nx - 1), im = std::max(i - 1, 0);
      const int   jp = std::min(j + 1, ny - 1), jm = std::max(j - 1, 0);
      const float dx = (zs(ip, j) - zs(im, j)) / float(ip - im) * nx;
      const float dy = (zs(i, jp) - zs(i, jm)) / float(jp - jm) * nx;
      const int   k  = j * nx + i;

      gx[k]    = dx;
      gy[k]    = dy;
      slope[k] = std::hypot(dx, dy);
    }

  const Array fine   = topographic_residual(z, px(1.f / 400.f), true);
  const Array crest  = topographic_residual(z, px(1.f / 100.f), true);
  const Array region = topographic_residual(z, px(1.f / 20.f), true);

  // drainage on a log scale: 0 on the ridges, 1 in the main river
  Array flow = flow_accumulation_dinf(zs, 0.1f);
  {
    const float lmax = std::log(std::max(flow.max(), 2.f));
    for (float &v : flow.vector)
      v = std::log(std::max(v, 1.f)) / lmax;
  }
  Array moisture = flow;
  smooth_cpulse(moisture, px(1.f / 100.f));

  // deposits relative to the thick ones (among the cells that have any), so a
  // few strong spots do not wash out the rest and sparse maps stay sparse
  std::vector<float> deposit(n, 0.f);
  if (p_deposition && p_deposition->shape == shape)
  {
    std::vector<float> positive;
    for (float v : p_deposition->vector)
      if (v > 0.f)
        positive.push_back(v);

    if (!positive.empty())
    {
      auto it = positive.begin() + (size_t)(0.9 * (positive.size() - 1));
      std::nth_element(positive.begin(), it, positive.end());
      const float ref = std::max(*it, 1e-6f);
      for (int k = 0; k < n; ++k)
        deposit[k] = std::clamp(p_deposition->vector[k] / ref, 0.f, 1.f);
    }
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
  const Array n_mid   = noise(12.f, 1, 5);
  const Array n_fine  = noise(64.f, 2, 3);
  const Array n_rill  = noise(160.f, 3, 2);
  const Array n_grain = noise(320.f, 4, 2);

  // --- Fall lines

  // bare high crests: their colour runs down the slopes below them
  std::vector<float> source(n);
  for (int k = 0; k < n; ++k)
  {
    const float h = (z.vector[k] - zmin) / zrange;
    source[k]     = smoothstep3(0.6f * crest.vector[k] + 0.6f * region.vector[k],
                                0.05f,
                                0.35f) *
                    smoothstep3(h + 0.15f * n_mid.vector[k], 0.3f, 0.75f) *
                    (0.75f + 0.25f * n_fine.vector[k]);
  }

  const float streak_px    = p.streak_length * nx;
  const int   streak_steps = std::clamp(int(streak_px), 0, 64);
  const float streak_step  = streak_steps ? streak_px / streak_steps : 0.f;
  const int   rill_steps   = std::clamp(nx / 96, 3, 24);

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
          const float g = std::hypot(gx[k], gy[k]);
          if (g < 1e-6f)
            return;

          x += step * gx[k] / g;
          y += step * gy[k] / g;
          if (x < 0.f || y < 0.f || x > nx - 1.f || y > ny - 1.f)
            return;

          visit(s, int(y + 0.5f) * nx + int(x + 0.5f));
        }
      };

      const int k = j * nx + i;

      // fine noise averaged along the fall line: rills and gullies
      float sum   = n_rill.vector[k];
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
           { best = std::max(best, source[kk] * (1.f - s / (streak_steps + 1.f))); });
      streak[k] = best;
    }

  // --- Colours

  Texture out(shape, 4);

  const float cn     = p.color_noise;
  const float two_pi = 2.f * std::numbers::pi_v<float>;

#pragma omp parallel for
  for (int k = 0; k < n; ++k)
  {
    const float h  = (z.vector[k] - zmin) / zrange;
    const float s  = slope[k] / p.rock_slope; // 1 where rock takes over
    const float c  = crest.vector[k];
    const float r  = region.vector[k];
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
    col              = glm::mix(col,
                   shade_color(p.soil, cn * (0.25f * rl + 0.3f * nm + 0.15f * ng)),
                   soil);

    // sediment along the drainage lines and in crisp deposits
    const float t0 = 0.8f - 0.5f * p.flow_width;
    float sed = p.flow * smoothstep3(flow.vector[k] + 0.05f * nf, t0, t0 + 0.08f);
    sed       = std::max(sed,
                   p.deposition * smoothstep3(deposit[k] + 0.15f * nf, 0.4f, 0.8f));
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
    col              = glm::mix(col,
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
    col = glm::mix(col,
                   shade_color(p.ridge, cn * (0.3f * nf + 0.15f * rl + 0.15f * ng)),
                   spill);

    // snow on high ground, sliding off the steep parts
    const float snow = p.snow_amount *
                       smoothstep3(h - p.snow_line + 0.08f * nm + 0.04f * nf -
                                       0.15f * std::max(0.f, s - 0.9f) - 0.05f * c,
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
