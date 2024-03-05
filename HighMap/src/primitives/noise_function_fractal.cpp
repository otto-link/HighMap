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
