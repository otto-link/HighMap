/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "FastNoiseLite.h"

#include "highmap/array.hpp"
#include "highmap/noise_function.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

// --- (x, y) function definitions

WaveDuneFunction::WaveDuneFunction(Vec2<float> kw,
                                   float       angle,
                                   float       xtop,
                                   float       xbottom,
                                   float       phase_shift)
    : NoiseFunction(kw, 0), xtop(xtop), xbottom(xbottom),
      phase_shift(phase_shift)
{
  this->set_angle(angle);

  this->function = [this](float x, float y, float)
  {
    float r = ca * this->kw.x * x + sa * this->kw.y * y;
    float xp = std::fmod(r + this->phase_shift + 10.f, 1.f);
    float yp = 0.f;

    if (xp < this->xtop)
    {
      float r = xp / this->xtop;
      yp = r * r * (3.f - 2.f * r);
    }
    else if (xp < this->xbottom)
    {
      float r = (xp - this->xbottom) / (this->xtop - this->xbottom);
      yp = r * r * (2.f - r);
    }
    return yp;
  };
}

WaveSineFunction::WaveSineFunction(Vec2<float> kw,
                                   float       angle,
                                   float       phase_shift)
    : NoiseFunction(kw, 0), phase_shift(phase_shift)
{
  this->set_angle(angle);

  this->function = [this](float x, float y, float)
  {
    float r = ca * this->kw.x * x + sa * this->kw.y * y;
    return std::cos(2.f * M_PI * r + this->phase_shift);
  };
}

WaveSquareFunction::WaveSquareFunction(Vec2<float> kw,
                                       float       angle,
                                       float       phase_shift)
    : NoiseFunction(kw, 0), phase_shift(phase_shift)
{
  this->set_angle(angle);

  this->function = [this](float x, float y, float)
  {
    float r = ca * this->kw.x * x + sa * this->kw.y * y;
    return r = 2.f * (int)r - (int)(2.f * r) + 1.f;
  };
}

WaveTriangularFunction::WaveTriangularFunction(Vec2<float> kw,
                                               float       angle,
                                               float       slant_ratio,
                                               float       phase_shift)
    : NoiseFunction(kw, 0), slant_ratio(slant_ratio), phase_shift(phase_shift)
{
  this->set_angle(angle);

  this->function = [this](float x, float y, float)
  {
    float r = ca * this->kw.x * x + sa * this->kw.y * y;

    r = r - (int)r;
    if (r < this->slant_ratio)
      r /= this->slant_ratio;
    else
      r = 1.f - (r - this->slant_ratio) / (1.f - this->slant_ratio);
    return r * r * (3.f - 2.f * r);
  };
}

// --- wrappers for array filling

Array wave_dune(Vec2<int>   shape,
                float       kw,
                float       angle,
                float       xtop,
                float       xbottom,
                float       phase_shift,
                Array      *p_noise_x,
                Array      *p_noise_y,
                Array      *p_stretching,
                Vec2<float> shift,
                Vec2<float> scale)
{
  Array                  array = Array(shape);
  hmap::WaveDuneFunction f = hmap::WaveDuneFunction({kw, kw},
                                                    angle,
                                                    xtop,
                                                    xbottom,
                                                    phase_shift);
  std::vector<float>     x, y;
  hmap::Vec4<float>      bbox = {0.f + shift.x,
                                 scale.x + shift.x,
                                 0.f + shift.y,
                                 scale.y + shift.y};

  grid_xy_vector(x, y, shape, bbox);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

Array wave_sine(Vec2<int>   shape,
                float       kw,
                float       angle,
                float       phase_shift,
                Array      *p_noise_x,
                Array      *p_noise_y,
                Array      *p_stretching,
                Vec2<float> shift,
                Vec2<float> scale)
{
  Array                  array = Array(shape);
  hmap::WaveSineFunction f = hmap::WaveSineFunction({kw, kw},
                                                    angle,
                                                    phase_shift);
  std::vector<float>     x, y;
  hmap::Vec4<float>      bbox = {0.f + shift.x,
                                 scale.x + shift.x,
                                 0.f + shift.y,
                                 scale.y + shift.y};

  grid_xy_vector(x, y, shape, bbox);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

Array wave_square(Vec2<int>   shape,
                  float       kw,
                  float       angle,
                  float       phase_shift,
                  Array      *p_noise_x,
                  Array      *p_noise_y,
                  Array      *p_stretching,
                  Vec2<float> shift,
                  Vec2<float> scale)
{
  Array                    array = Array(shape);
  hmap::WaveSquareFunction f = hmap::WaveSquareFunction({kw, kw},
                                                        angle,
                                                        phase_shift);
  std::vector<float>       x, y;
  hmap::Vec4<float>        bbox = {0.f + shift.x,
                                   scale.x + shift.x,
                                   0.f + shift.y,
                                   scale.y + shift.y};

  grid_xy_vector(x, y, shape, bbox);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

Array wave_triangular(Vec2<int>   shape,
                      float       kw,
                      float       angle,
                      float       slant_ratio,
                      float       phase_shift,
                      Array      *p_noise_x,
                      Array      *p_noise_y,
                      Array      *p_stretching,
                      Vec2<float> shift,
                      Vec2<float> scale)
{
  Array                        array = Array(shape);
  hmap::WaveTriangularFunction f = hmap::WaveTriangularFunction({kw, kw},
                                                                angle,
                                                                slant_ratio,
                                                                phase_shift);
  std::vector<float>           x, y;
  hmap::Vec4<float>            bbox = {0.f + shift.x,
                                       scale.x + shift.x,
                                       0.f + shift.y,
                                       scale.y + shift.y};

  grid_xy_vector(x, y, shape, bbox);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

} // namespace hmap
