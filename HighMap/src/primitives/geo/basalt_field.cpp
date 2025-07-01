/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/array.hpp"
#include "highmap/filters.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

namespace hmap::gpu
{

Array basalt_field(Vec2<int>    shape,
                   Vec2<float>  kw,
                   uint         seed,
                   float        warp_kw,
                   float        large_scale_warp_amp,
                   float        large_scale_gain,
                   float        large_scale_amp,
                   float        medium_scale_kw_ratio,
                   float        medium_scale_warp_amp,
                   float        medium_scale_gain,
                   float        medium_scale_amp,
                   float        small_scale_kw_ratio,
                   float        small_scale_overlay_amp,
                   float        rugosity_kw_ratio,
                   float        rugosity_amp,
                   bool         flatten_activate,
                   float        flatten_kw_ratio,
                   float        flatten_amp,
                   const Array *p_noise_x,
                   const Array *p_noise_y,
                   Vec4<float>  bbox)
{
  // --- large scales

  Array z_large;

  {
    int   octaves = 8;
    float weight = 0.5f;
    float persistence = 0.5f;
    float lacunarity = 2.f;

    Array dx = gpu::noise_fbm(NoiseType::PERLIN,
                              shape,
                              Vec2<float>(warp_kw, warp_kw),
                              seed++,
                              octaves,
                              weight,
                              persistence,
                              lacunarity,
                              nullptr,
                              p_noise_x,
                              p_noise_y,
                              nullptr,
                              bbox);
    dx *= large_scale_warp_amp;

    // base
    Vec2<float>       jitter(1.f, 1.f);
    float             k_smoothing = 0.9f;
    VoronoiReturnType return_type = VoronoiReturnType::EDGE_DISTANCE_SQUARED;

    lacunarity = 1.7f;

    z_large = voronoi_fbm(shape,
                          kw,
                          seed++,
                          jitter,
                          k_smoothing,
                          return_type,
                          octaves,
                          weight,
                          persistence,
                          lacunarity,
                          nullptr,
                          &dx,
                          &dx,
                          bbox);

    // rescale to [0, 1] (roughly)
    z_large += 0.25f;
    z_large *= 2.f;

    z_large = sqrt_safe(z_large);
    gain(z_large, large_scale_gain);
    z_large = maximum(z_large, 0.3f);

    // rescale
    remap(z_large, 0.f, large_scale_amp, 0.5f, 1.f);
  }

  // --- medium scales

  Array z_medium;

  {
    Vec2<float>       jitter(1.f, 1.f);
    float             k_smoothing = 0.f;
    VoronoiReturnType return_type = VoronoiReturnType::EDGE_DISTANCE_SQUARED;
    int               octaves = 4;
    float             weight = 0.5f;
    float             persistence = 0.5f;
    float             lacunarity = 2.3f;

    Array dx = gpu::voronoi_fbm(shape,
                                0.5f * Vec2<float>(warp_kw, warp_kw),
                                seed++,
                                jitter,
                                k_smoothing,
                                return_type,
                                octaves,
                                weight,
                                persistence,
                                lacunarity,
                                nullptr,
                                p_noise_x,
                                p_noise_y,
                                bbox);
    dx *= medium_scale_warp_amp;

    // base
    lacunarity = 1.7f;
    k_smoothing = 0.9f;

    z_medium = voronoi_fbm(shape,
                           medium_scale_kw_ratio * kw,
                           seed++,
                           jitter,
                           k_smoothing,
                           return_type,
                           octaves,
                           weight,
                           persistence,
                           lacunarity,
                           nullptr,
                           &dx,
                           &dx,
                           bbox);

    // rescale to [0, 1] (roughly)
    z_medium += 0.25f;
    z_medium *= 2.f;

    z_medium = sqrt_safe(z_medium);
    gain(z_medium, medium_scale_gain);

    remap(z_medium, -1.f + medium_scale_amp, medium_scale_amp, 0.f, 1.f);
  }

  // --- small scales

  Array z_small;

  {
    // some parameters are the same as medium scales

    Vec2<float>       jitter(1.f, 1.f);
    float             k_smoothing = 0.f;
    VoronoiReturnType return_type = VoronoiReturnType::EDGE_DISTANCE_SQUARED;
    int               octaves = 4;
    float             weight = 0.5f;
    float             persistence = 0.5f;
    float             lacunarity = 2.f;

    Array dx = gpu::voronoi_fbm(shape,
                                Vec2<float>(warp_kw, warp_kw),
                                seed++,
                                jitter,
                                k_smoothing,
                                return_type,
                                octaves,
                                weight,
                                persistence,
                                lacunarity,
                                nullptr,
                                p_noise_x,
                                p_noise_y,
                                bbox);
    dx *= medium_scale_warp_amp;

    // base
    lacunarity = 1.7f;
    k_smoothing = 0.9f;

    z_small = voronoi_fbm(shape,
                          small_scale_kw_ratio * kw,
                          seed++,
                          jitter,
                          k_smoothing,
                          return_type,
                          octaves,
                          weight,
                          persistence,
                          lacunarity,
                          nullptr,
                          &dx,
                          &dx,
                          bbox);

    // rescale to [0, 1] (roughly)
    z_small += 0.25f;
    z_small *= 2.f;

    z_small = sqrt_safe(z_small);
    gain(z_small, medium_scale_gain);
    remap(z_small,
          -1.f + 0.5f * medium_scale_amp,
          0.5f * medium_scale_amp,
          0.f,
          1.f);
  }

  Array z = hmap::maximum_smooth(z_large, z_medium, 0.04f);
  z = hmap::maximum_smooth(z, z_small, 0.08f);

  // small scales overlay
  z += small_scale_overlay_amp * z_small;

  // --- last steps...

  // multiply add
  {
    int   octaves = 8;
    float weight = 0.5f;
    float persistence = 0.5f;
    float lacunarity = 2.f;

    Array rugosity = gpu::noise_fbm(NoiseType::PERLIN,
                                    shape,
                                    rugosity_kw_ratio * kw,
                                    seed++,
                                    octaves,
                                    weight,
                                    persistence,
                                    lacunarity,
                                    nullptr,
                                    p_noise_x,
                                    p_noise_y,
                                    nullptr,
                                    bbox);
    rugosity += 0.5f;
    z += rugosity_amp * rugosity * z;
  }

  // flattening pass
  if (flatten_activate)
  {
    int   octaves = 8;
    float weight = 0.5f;
    float persistence = 0.5f;
    float lacunarity = 2.f;

    Array z_flatten = gpu::noise_fbm(NoiseType::PERLIN,
                                     shape,
                                     flatten_kw_ratio * kw,
                                     seed++,
                                     octaves,
                                     weight,
                                     persistence,
                                     lacunarity,
                                     nullptr,
                                     p_noise_x,
                                     p_noise_y,
                                     nullptr,
                                     bbox);

    z_flatten += 0.5f;
    remap(z_flatten, -flatten_amp, 0.5f * large_scale_amp, 0.f, 1.f);
    z = hmap::minimum_smooth(z, z_flatten, 0.3f);
  }

  return z;
}

} // namespace hmap::gpu
