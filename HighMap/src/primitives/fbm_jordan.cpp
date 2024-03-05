/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>
#include <functional>

#include "FastNoiseLite.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

//----------------------------------------------------------------------
// Main operator(s)
//----------------------------------------------------------------------

namespace hmap
{

// Array fbm_jordan(Vec2<int>   shape,
//                  Vec2<float> kw,
//                  uint        seed,
//                  int         noise_type,
//                  float       warp0,
//                  float       damp0,
//                  float       warp_scale,
//                  float       damp_scale,
//                  int         octaves,
//                  float       weight,
//                  float       persistence,
//                  float       lacunarity,
//                  Array      *p_base_elevation,
//                  Array      *p_noise_x,
//                  Array      *p_noise_y,
//                  Array      *p_stretching,
//                  Vec2<float> shift,
//                  Vec2<float> scale)
// {
//   FastNoiseLite noise(seed);

//   // --- define base noise function

//   std::function<float(float, float, uint)> noise_fct;

//   // frequency
//   if (noise_type == noise_type::noise_simplex2 ||
//       noise_type == noise_type::noise_simplex2s)
//     noise.SetFrequency(0.5f);
//   else
//     noise.SetFrequency(1.0f);

//   // noise function
//   noise.SetNoiseType(static_cast<FastNoiseLite::NoiseType>(noise_type));
//   noise_fct = [&noise](float x_, float y_, uint seed_)
//   {
//     noise.SetSeed(seed_);
//     return noise.GetNoise(x_, y_);
//   };

//   // divide wavenumber by 2 to get the proper since the noise function
//   // is squared in the layering process
//   std::vector<float> x = linspace(0.5f * kw.x * shift.x,
//                                   0.5f * kw.x * (shift.x + scale.x),
//                                   shape.x,
//                                   false);
//   std::vector<float> y = linspace(0.5f * kw.y * shift.y,
//                                   0.5f * kw.y * (shift.y + scale.y),
//                                   shape.y,
//                                   false);

//   // --- fractal layering function

//   // determine initial amplitude so that the final field has roughly a
//   // unit peak-to-peak amplitude
//   float amp0;
//   {
//     float amp = persistence;
//     float amp_fractal = 1.0f;
//     for (int i = 1; i < octaves; i++)
//     {
//       amp_fractal += amp;
//       amp *= persistence;
//     }
//     amp0 = 1.f / amp_fractal;
//   }

//   // define function
//   std::function<float(float, float, float)> fractal_fct;

//   fractal_fct = [&amp0,
//                  &seed,
//                  &warp0,
//                  &damp0,
//                  &warp_scale,
//                  &damp_scale,
//                  &octaves,
//                  &weight,
//                  &persistence,
//                  &lacunarity,
//                  &noise_fct](float x_, float y_, float initial_value)
//   {
//     // based on https://www.decarpentier.nl/scape-procedural-extensions
//     float sum = initial_value;
//     float amp = amp0;
//     float amp_damp = amp0;
//     float ki = 1.f;
//     float kj = 1.f;
//     int   kseed = seed;

//     // --- 1st octave

//     float value = noise_fct(x_, y_, kseed);
//     float dvdx = (noise_fct(x_ + HMAP_GRADIENT_OFFSET, y_, kseed) -
//                   noise_fct(x_ - HMAP_GRADIENT_OFFSET, y_, kseed)) /
//                  HMAP_GRADIENT_OFFSET;
//     float dvdy = (noise_fct(x_, y_ + HMAP_GRADIENT_OFFSET, kseed) -
//                   noise_fct(x_, y_ - HMAP_GRADIENT_OFFSET, kseed)) /
//                  HMAP_GRADIENT_OFFSET;

//     sum += value * value;
//     float dx_sum_warp = warp0 * value * dvdx;
//     float dy_sum_warp = warp0 * value * dvdy;
//     float dx_sum_damp = damp0 * value * dvdx;
//     float dy_sum_damp = damp0 * value * dvdy;

//     amp *= (1.f - weight) + weight * std::min(value * value + 1.f, 2.f) *
//     0.5f;

//     ki *= lacunarity;
//     kj *= lacunarity;
//     amp *= persistence;
//     amp_damp *= persistence;
//     kseed++;

//     // --- other octaves

//     for (int k = 0; k < octaves; k++)
//     {
//       float xw = ki * x_ + warp_scale * dx_sum_warp;
//       float yw = kj * y_ + warp_scale * dy_sum_warp;

//       float value = noise_fct(xw, yw, kseed);
//       float dvdx = (noise_fct(xw + HMAP_GRADIENT_OFFSET, yw, kseed) -
//                     noise_fct(xw - HMAP_GRADIENT_OFFSET, yw, kseed)) /
//                    HMAP_GRADIENT_OFFSET;
//       float dvdy = (noise_fct(xw, yw + HMAP_GRADIENT_OFFSET, kseed) -
//                     noise_fct(xw, yw - HMAP_GRADIENT_OFFSET, kseed)) /
//                    HMAP_GRADIENT_OFFSET;

//       sum += amp_damp * value * value;
//       dx_sum_warp += warp0 * value * dvdx;
//       dy_sum_warp += warp0 * value * dvdy;
//       dx_sum_damp += damp0 * value * dvdx;
//       dy_sum_damp += damp0 * value * dvdy;

//       amp *= (1.f - weight) +
//              weight * std::min(value * value + 1.f, 2.f) * 0.5f;

//       ki *= lacunarity;
//       kj *= lacunarity;
//       amp *= persistence;
//       amp_damp = amp * (1.f - damp_scale / (1.f + dx_sum_damp * dx_sum_damp +
//                                             dy_sum_damp * dy_sum_damp));
//       kseed++;
//     }
//     return sum;
//   };

//   // --- fill output array
//   Array array = Array(shape);

//   if (p_base_elevation)
//     array = *p_base_elevation;

//   fill_array_using_xy_function(array,
//                                x,
//                                y,
//                                p_noise_x,
//                                p_noise_y,
//                                p_stretching,
//                                fractal_fct);

//   return array;
// }

} // namespace hmap
