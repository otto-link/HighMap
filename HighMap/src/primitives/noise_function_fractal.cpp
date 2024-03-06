/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "highmap/noise_function.hpp"

namespace hmap
{

FbmFunction::FbmFunction(NoiseFunction *p_base,
                         int            octaves,
                         float          weight,
                         float          persistence,
                         float          lacunarity)
    : GenericFractalFunction(p_base, octaves, weight, persistence, lacunarity)
{
  this->function = [this](float x, float y, float initial_value)
  {
    float sum = initial_value;
    float amp = this->amp0;
    float ki = 1.f;
    float kj = 1.f;
    int   kseed = this->seed;

    for (int k = 0; k < this->octaves; k++)
    {
      this->p_base->set_seed(kseed);
      float value = this->p_base->function(ki * x, kj * y, 0.f);
      sum += value * amp;
      amp *= (1.f - this->weight) +
             this->weight * std::min(value + 1.f, 2.f) * 0.5f;

      ki *= this->lacunarity;
      kj *= this->lacunarity;
      amp *= this->persistence;
      kseed++;
    }
    return sum;
  };
}

FbmIqFunction::FbmIqFunction(NoiseFunction *p_base,
                             int            octaves,
                             float          weight,
                             float          persistence,
                             float          lacunarity,
                             float          gradient_scale)
    : GenericFractalFunction(p_base, octaves, weight, persistence, lacunarity),
      gradient_scale(gradient_scale)
{
  this->function = [this](float x, float y, float initial_value)
  {
    float sum = initial_value;
    float dx_sum = 0.f;
    float dy_sum = 0.f;
    float amp = this->amp0;
    float ki = 1.f;
    float kj = 1.f;
    int   kseed = this->seed;

    for (int k = 0; k < this->octaves; k++)
    {
      float xw = ki * x;
      float yw = kj * y;

      float value = this->p_base->function(xw, yw, kseed);
      float dvdx =
          (this->p_base->function(xw + HMAP_GRADIENT_OFFSET, yw, kseed) -
           this->p_base->function(xw - HMAP_GRADIENT_OFFSET, yw, kseed)) /
          HMAP_GRADIENT_OFFSET;
      float dvdy =
          (this->p_base->function(xw, yw + HMAP_GRADIENT_OFFSET, kseed) -
           this->p_base->function(xw, yw - HMAP_GRADIENT_OFFSET, kseed)) /
          HMAP_GRADIENT_OFFSET;

      value = smoothstep3(0.5f + value);

      dx_sum += dvdx;
      dy_sum += dvdy;

      sum += value * amp /
             (1.f + this->gradient_scale * (dx_sum * dx_sum + dy_sum * dy_sum));
      amp *= (1.f - this->weight) +
             this->weight * std::min(value + 1.f, 2.f) * 0.5f;

      ki *= this->lacunarity;
      kj *= this->lacunarity;
      amp *= this->persistence;
      kseed++;
    }
    return sum;
  };
}

FbmJordanFunction::FbmJordanFunction(NoiseFunction *p_base,
                                     int            octaves,
                                     float          weight,
                                     float          persistence,
                                     float          lacunarity,
                                     float          warp0,
                                     float          damp0,
                                     float          warp_scale,
                                     float          damp_scale)
    : GenericFractalFunction(p_base, octaves, weight, persistence, lacunarity),
      warp0(warp0), damp0(damp0), warp_scale(warp_scale), damp_scale(damp_scale)
{
  this->function = [this](float x, float y, float initial_value)
  {
    // based on https://www.decarpentier.nl/scape-procedural-extensions
    float sum = initial_value;
    float amp = this->amp0;
    float amp_damp = this->amp0;
    float ki = 1.f;
    float kj = 1.f;
    int   kseed = this->seed;

    // --- 1st octave

    float value = this->p_base->function(x, y, kseed);
    float dvdx = (this->p_base->function(x + HMAP_GRADIENT_OFFSET, y, kseed) -
                  this->p_base->function(x - HMAP_GRADIENT_OFFSET, y, kseed)) /
                 HMAP_GRADIENT_OFFSET;
    float dvdy = (this->p_base->function(x, y + HMAP_GRADIENT_OFFSET, kseed) -
                  this->p_base->function(x, y - HMAP_GRADIENT_OFFSET, kseed)) /
                 HMAP_GRADIENT_OFFSET;

    sum += value * value;
    float dx_sum_warp = this->warp0 * value * dvdx;
    float dy_sum_warp = this->warp0 * value * dvdy;
    float dx_sum_damp = this->damp0 * value * dvdx;
    float dy_sum_damp = this->damp0 * value * dvdy;

    amp *= (1.f - this->weight) +
           this->weight * std::min(value * value + 1.f, 2.f) * 0.5f;

    ki *= this->lacunarity;
    kj *= this->lacunarity;
    amp *= this->persistence;
    amp_damp *= this->persistence;
    kseed++;

    // --- other octaves

    for (int k = 0; k < this->octaves; k++)
    {
      float xw = ki * x + this->warp_scale * dx_sum_warp;
      float yw = kj * y + this->warp_scale * dy_sum_warp;

      float value = this->p_base->function(xw, yw, kseed);
      float dvdx =
          (this->p_base->function(xw + HMAP_GRADIENT_OFFSET, yw, kseed) -
           this->p_base->function(xw - HMAP_GRADIENT_OFFSET, yw, kseed)) /
          HMAP_GRADIENT_OFFSET;
      float dvdy =
          (this->p_base->function(xw, yw + HMAP_GRADIENT_OFFSET, kseed) -
           this->p_base->function(xw, yw - HMAP_GRADIENT_OFFSET, kseed)) /
          HMAP_GRADIENT_OFFSET;

      sum += amp_damp * value * value;
      dx_sum_warp += this->warp0 * value * dvdx;
      dy_sum_warp += this->warp0 * value * dvdy;
      dx_sum_damp += this->damp0 * value * dvdx;
      dy_sum_damp += this->damp0 * value * dvdy;

      amp *= (1.f - this->weight) +
             this->weight * std::min(value * value + 1.f, 2.f) * 0.5f;

      ki *= this->lacunarity;
      kj *= this->lacunarity;
      amp *= this->persistence;
      amp_damp = amp *
                 (1.f - this->damp_scale / (1.f + dx_sum_damp * dx_sum_damp +
                                            dy_sum_damp * dy_sum_damp));
      kseed++;
    }
    return sum;
  };
}

FbmPingpongFunction::FbmPingpongFunction(NoiseFunction *p_base,
                                         int            octaves,
                                         float          weight,
                                         float          persistence,
                                         float          lacunarity)
    : GenericFractalFunction(p_base, octaves, weight, persistence, lacunarity)
{
  this->function = [this](float x, float y, float initial_value)
  {
    float sum = initial_value;
    float amp = this->amp0;
    float ki = 1.f;
    float kj = 1.f;
    int   kseed = this->seed;

    for (int k = 0; k < this->octaves; k++)
    {
      this->p_base->set_seed(kseed);
      float value = (this->p_base->function(ki * x, kj * y, kseed++) + 1.f) *
                    2.f;
      value -= (int)(value * 0.5f) * 2;
      value = value < 1 ? value : 2 - value;
      value = smoothstep5(value);

      sum += (value - 0.5f) * 2.f * amp;
      amp *= (1.f - this->weight) + this->weight * value;

      ki *= this->lacunarity;
      kj *= this->lacunarity;
      amp *= this->persistence;
      kseed++;
    }
    return sum;
  };
}

FbmRidgedFunction::FbmRidgedFunction(NoiseFunction *p_base,
                                     int            octaves,
                                     float          weight,
                                     float          persistence,
                                     float          lacunarity,
                                     float          k_smoothing)
    : GenericFractalFunction(p_base, octaves, weight, persistence, lacunarity),
      k_smoothing(k_smoothing)
{
  this->function = [this](float x, float y, float initial_value)
  {
    float sum = initial_value;
    float amp = this->amp0;
    float ki = 1.f;
    float kj = 1.f;
    int   kseed = this->seed;

    if (this->k_smoothing == 0.f)
      for (int k = 0; k < this->octaves; k++)
      {
        this->p_base->set_seed(kseed);
        float value = std::abs(this->p_base->function(ki * x, kj * y, 0.f));
        sum += (1.f - 2.f * value) * amp;
        amp *= 1.f - this->weight * value;

        ki *= this->lacunarity;
        kj *= this->lacunarity;
        amp *= this->persistence;
        kseed++;
      }
    else
      for (int k = 0; k < this->octaves; k++)
      {
        this->p_base->set_seed(kseed);
        float value = this->p_base->function(ki * x, kj * y, 0.f);
        value = abs_smooth(value, this->k_smoothing);
        sum += (1.f - 2.f * value) * amp;
        amp *= 1.f - this->weight * value;

        ki *= this->lacunarity;
        kj *= this->lacunarity;
        amp *= this->persistence;
        kseed++;
      }

    return sum;
  };
}

FbmSwissFunction::FbmSwissFunction(NoiseFunction *p_base,
                                   int            octaves,
                                   float          weight,
                                   float          persistence,
                                   float          lacunarity,
                                   float          warp_scale)
    : GenericFractalFunction(p_base, octaves, weight, persistence, lacunarity)
{
  this->set_warp_scale(warp_scale);

  this->function = [this](float x, float y, float initial_value)
  {
    // based on https://www.decarpentier.nl/scape-procedural-extensions
    float sum = initial_value;
    float amp = this->amp0;
    float ki = 1.f;
    float kj = 1.f;
    int   kseed = this->seed;

    float dx_sum = 0.f;
    float dy_sum = 0.f;

    for (int k = 0; k < this->octaves; k++)
    {
      float xw = ki * x + this->warp_scale_normalized * dx_sum;
      float yw = kj * y + this->warp_scale_normalized * dy_sum;

      float value = this->p_base->function(xw, yw, 0.f);
      float dvdx = (this->p_base->function(xw + HMAP_GRADIENT_OFFSET, yw, 0.f) -
                    this->p_base->function(xw - HMAP_GRADIENT_OFFSET,
                                           yw,
                                           0.f)) /
                   HMAP_GRADIENT_OFFSET;
      float dvdy = (this->p_base->function(xw, yw + HMAP_GRADIENT_OFFSET, 0.f) -
                    this->p_base->function(xw,
                                           yw - HMAP_GRADIENT_OFFSET,
                                           0.f)) /
                   HMAP_GRADIENT_OFFSET;

      sum += value * amp;
      dx_sum += amp * dvdx * -(value + 0.5f);
      dy_sum += amp * dvdy * -(value + 0.5f);

      amp *= (1.f - this->weight) +
             this->weight * std::min(value + 1.f, 2.f) * 0.5f;

      ki *= this->lacunarity;
      kj *= this->lacunarity;
      amp *= this->persistence;
      kseed++;
    }
    return sum;
  };
}

} // namespace hmap
