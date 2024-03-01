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

Array fbm_iq(Vec2<int>   shape,
             Vec2<float> kw,
             uint        seed,
             int         noise_type,
             float       gradient_scale,
             int         octaves,
             float       weight,
             float       persistence,
             float       lacunarity,
             Array      *p_base_elevation,
             Array      *p_noise_x,
             Array      *p_noise_y,
             Array      *p_stretching,
             Vec2<float> shift,
             Vec2<float> scale)
{
  FastNoiseLite noise(seed);

  // --- define base noise function

  std::function<float(float, float, uint)> noise_fct;

  // frequency
  if (noise_type == noise_type::noise_simplex2 ||
      noise_type == noise_type::noise_simplex2s)
    noise.SetFrequency(0.5f);
  else
    noise.SetFrequency(1.0f);

  // noise function
  noise.SetNoiseType(static_cast<FastNoiseLite::NoiseType>(noise_type));
  noise_fct = [&noise](float x_, float y_, uint seed_)
  {
    noise.SetSeed(seed_);
    return noise.GetNoise(x_, y_);
  };

  std::vector<float> x = linspace(kw.x * shift.x,
                                  kw.x * (shift.x + scale.x),
                                  shape.x,
                                  false);
  std::vector<float> y = linspace(kw.y * shift.y,
                                  kw.y * (shift.y + scale.y),
                                  shape.y,
                                  false);

  // --- fractal layering function

  // determine initial amplitude so that the final field has roughly a
  // unit peak-to-peak amplitude
  float amp0;
  {
    float amp = persistence;
    float amp_fractal = 1.0f;
    for (int i = 1; i < octaves; i++)
    {
      amp_fractal += amp;
      amp *= persistence;
    }
    amp0 = 1.f / amp_fractal;
  }

  // define function
  std::function<float(float, float, float)> fractal_fct;

  fractal_fct = [&amp0,
                 &seed,
                 &gradient_scale,
                 &octaves,
                 &weight,
                 &persistence,
                 &lacunarity,
                 &noise_fct](float x_, float y_, float initial_value)
  {
    // based on https://www.shadertoy.com/view/MdX3Rr
    float sum = initial_value;
    float dx_sum = 0.f;
    float dy_sum = 0.f;
    float amp = amp0;
    float ki = 1.f;
    float kj = 1.f;
    int   kseed = seed;

    for (int k = 0; k < octaves; k++)
    {
      float xw = ki * x_;
      float yw = kj * y_;

      float value = noise_fct(xw, yw, kseed);
      float dvdx = (noise_fct(xw + HMAP_GRADIENT_OFFSET, yw, kseed) -
                    noise_fct(xw - HMAP_GRADIENT_OFFSET, yw, kseed)) /
                   HMAP_GRADIENT_OFFSET;
      float dvdy = (noise_fct(xw, yw + HMAP_GRADIENT_OFFSET, kseed) -
                    noise_fct(xw, yw - HMAP_GRADIENT_OFFSET, kseed)) /
                   HMAP_GRADIENT_OFFSET;

      value = smoothstep3(0.5f + value);

      dx_sum += dvdx;
      dy_sum += dvdy;

      sum += value * amp /
             (1.f + gradient_scale * (dx_sum * dx_sum + dy_sum * dy_sum));
      amp *= (1.f - weight) + weight * std::min(value + 1.f, 2.f) * 0.5f;

      ki *= lacunarity;
      kj *= lacunarity;
      amp *= persistence;
      kseed++;
    }
    return sum;
  };

  // --- fill output array
  Array array = Array(shape);

  if (p_base_elevation)
    array = *p_base_elevation;

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               fractal_fct);

  return array;
}

} // namespace hmap
