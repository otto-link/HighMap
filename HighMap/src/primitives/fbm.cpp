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
// Helper(s)
//----------------------------------------------------------------------

static float ping_pong(float t)
{
  t -= (int)(t * 0.5f) * 2;
  return t < 1 ? t : 2 - t;
}

//----------------------------------------------------------------------
// Main operator(s)
//----------------------------------------------------------------------

namespace hmap
{

Array fbm(Vec2<int>   shape,
          Vec2<float> kw,
          uint        seed,
          int         noise_type,
          int         fractal_type,
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

  switch (fractal_type)
  {
  //
  case (fractal_type::fractal_none):
  {
    fractal_fct = [&noise_fct, &seed](float x_, float y_, float)
    { return noise_fct(x_, y_, seed); };
  }
  break;
  //
  case (fractal_type::fractal_fbm):
  {
    fractal_fct = [&amp0,
                   &seed,
                   &octaves,
                   &weight,
                   &persistence,
                   &lacunarity,
                   &noise_fct](float x_, float y_, float initial_value)
    {
      float sum = initial_value;
      float amp = amp0;
      float ki = 1.f;
      float kj = 1.f;
      int   kseed = seed;

      for (int k = 0; k < octaves; k++)
      {
        float value = noise_fct(ki * x_, kj * y_, kseed++);
        sum += value * amp;
        amp *= (1.f - weight) + weight * std::min(value + 1.f, 2.f) * 0.5f;

        ki *= lacunarity;
        kj *= lacunarity;
        amp *= persistence;
      }
      return sum;
    };
  }
  break;
  //
  case (fractal_type::fractal_ridged):
  {
    fractal_fct = [&amp0,
                   &seed,
                   &octaves,
                   &weight,
                   &persistence,
                   &lacunarity,
                   &noise_fct](float x_, float y_, float initial_value)
    {
      float sum = initial_value;
      float amp = amp0;
      float ki = 1.f;
      float kj = 1.f;
      int   kseed = seed;

      for (int k = 0; k < octaves; k++)
      {
        // float value = std::abs(noise_fct(ki * x_, kj * y_, kseed++));

        float value = abs_smooth(noise_fct(ki * x_, kj * y_, kseed++), 0.25f);
        sum += (value * -2.f + 1.f) * amp;
        amp *= (1.f - weight) + weight * (1.f - value);

        ki *= lacunarity;
        kj *= lacunarity;
        amp *= persistence;
      }
      return sum;
    };
  }
  break;
  //
  case (fractal_type::fractal_pingpong):
  {
    fractal_fct = [&amp0,
                   &seed,
                   &octaves,
                   &weight,
                   &persistence,
                   &lacunarity,
                   &noise_fct](float x_, float y_, float initial_value)
    {
      float sum = initial_value;
      float amp = amp0;
      float ki = 1.f;
      float kj = 1.f;
      int   kseed = seed;

      for (int k = 0; k < octaves; k++)
      {
        float value = ping_pong((noise_fct(ki * x_, kj * y_, kseed++) + 1.f) *
                                2.f);
        value = smoothstep5(value);
        sum += (value - 0.5f) * 2.f * amp;
        amp *= (1.f - weight) + weight * value;

        ki *= lacunarity;
        kj *= lacunarity;
        amp *= persistence;
      }
      return sum;
    };
  }
  break;
  //
  case (fractal_type::fractal_max):
  {
    fractal_fct = [&amp0,
                   &seed,
                   &octaves,
                   &weight,
                   &persistence,
                   &lacunarity,
                   &noise_fct](float x_, float y_, float initial_value)
    {
      float sum = initial_value;
      float amp = amp0;
      float ki = 1.f;
      float kj = 1.f;
      int   kseed = seed;

      for (int k = 0; k < octaves; k++)
      {
        float value = noise_fct(ki * x_, kj * y_, kseed++);
        sum = maximum_smooth(sum, sum + value * amp, 0.1f);
        amp *= (1.f - weight) + weight * std::min(value + 1.f, 2.f) * 0.5f;

        ki *= lacunarity;
        kj *= lacunarity;
        amp *= persistence;
      }
      return sum;
    };
  }
  break;
  //
  case (fractal_type::fractal_min):
  {
    fractal_fct = [&amp0,
                   &seed,
                   &octaves,
                   &weight,
                   &persistence,
                   &lacunarity,
                   &noise_fct](float x_, float y_, float initial_value)
    {
      float sum = initial_value;
      float amp = amp0;
      float ki = 1.f;
      float kj = 1.f;
      int   kseed = seed;

      for (int k = 0; k < octaves; k++)
      {
        float value = noise_fct(ki * x_, kj * y_, kseed++);
        sum = minimum_smooth(sum, sum + value * amp, 0.1f);
        amp *= (1.f - weight) + weight * std::min(value + 1.f, 2.f) * 0.5f;

        ki *= lacunarity;
        kj *= lacunarity;
        amp *= persistence;
      }
      return sum;
    };
  }
  break;
  //
  default:
  {
    LOG_ERROR("unknown fractal type");
    throw std::runtime_error("unknown fractal type");
  }
  }

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
