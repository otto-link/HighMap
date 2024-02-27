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

float compute_fractal_bounding(int octaves, float persistence)
{
  float amp = persistence;
  float amp_fractal = 1.0f;
  for (int i = 1; i < octaves; i++)
  {
    amp_fractal += amp;
    amp *= persistence;
  }
  return 1.f / amp_fractal;
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

  // fractal parameters
  noise.SetFractalType(static_cast<FastNoiseLite::FractalType>(fractal_type));

  if (fractal_type != fractal_type::fractal_none)
  {
    noise.SetFractalOctaves(octaves);
    noise.SetFractalLacunarity(lacunarity);
    noise.SetFractalGain(persistence);
    noise.SetFractalWeightedStrength(weight);
  }

  // noise primitive parameters
  std::function<float(float, float)> noise_fct;

  // frequency
  if (noise_type == noise_type::noise_simplex2 ||
      noise_type == noise_type::noise_simplex2s ||
      noise_type == noise_type::noise_perlin_billow ||
      noise_type == noise_type::noise_perlin_cliff)
    noise.SetFrequency(0.5f);
  else
    noise.SetFrequency(1.0f);

  // adjust noise function
  switch (noise_type)
  {
  case (noise_type::noise_perlin_billow):
  {
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise_fct = [&noise](float x_, float y_)
    {
      float smoothing_parameter = 0.05f;
      return 2.f * abs_smooth(noise.GetNoise(x_, y_), smoothing_parameter) -
             1.f;
    };
    break;
  }
  case (noise_type::noise_perlin_cliff):
  {
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise_fct = [&noise](float x_, float y_)
    {
      float smoothing_parameter = 0.01f;
      float value = noise.GetNoise(x_, y_);
      return 0.5f * value - abs_smooth(value, smoothing_parameter) - 0.5f;
    };
    break;
  }
  default:
  {
    noise.SetNoiseType(static_cast<FastNoiseLite::NoiseType>(noise_type));
    noise_fct = [&noise](float x_, float y_) { return noise.GetNoise(x_, y_); };
  }
  }

  std::vector<float> x = linspace(kw.x * shift.x,
                                  kw.x * (shift.x + scale.x),
                                  shape.x,
                                  false);
  std::vector<float> y = linspace(kw.y * shift.y,
                                  kw.y * (shift.y + scale.y),
                                  shape.y,
                                  false);

  // --- fill output array
  Array array = Array(shape);
  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               noise_fct);

  return array;
}

} // namespace hmap
