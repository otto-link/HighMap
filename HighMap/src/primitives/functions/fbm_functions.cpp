/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "highmap/functions.hpp"
#include "highmap/math.hpp"

namespace hmap
{

FbmFunction::FbmFunction(std::unique_ptr<NoiseFunction> p_base,
                         int                            octaves,
                         float                          weight,
                         float                          persistence,
                         float                          lacunarity)
    : GenericFractalFunction(std::move(p_base),
                             octaves,
                             weight,
                             persistence,
                             lacunarity)
{
  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        float sum = 0.f;
        float amp = this->amp0;
        float ki = 1.f;
        float kj = 1.f;
        int   kseed = this->seed;
        float local_weight = (1.f - ctrl_param) + this->weight * ctrl_param;

        for (int k = 0; k < this->octaves; k++)
        {
          this->p_base->set_seed(kseed);
          float value = this->p_base->get_value(ki * x, kj * y, 0.f);
          sum += value * amp;
          amp *= (1.f - local_weight) +
                 local_weight * std::min(value + 1.f, 2.f) * 0.5f;

          ki *= this->lacunarity;
          kj *= this->lacunarity;
          amp *= this->persistence;
          kseed++;
        }
        return sum;
      });
}

FbmIqFunction::FbmIqFunction(std::unique_ptr<NoiseFunction> p_base,
                             int                            octaves,
                             float                          weight,
                             float                          persistence,
                             float                          lacunarity,
                             float                          gradient_scale)
    : GenericFractalFunction(std::move(p_base),
                             octaves,
                             weight,
                             persistence,
                             lacunarity),
      gradient_scale(gradient_scale)
{
  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        float sum = 0.f;
        float dx_sum = 0.f;
        float dy_sum = 0.f;
        float amp = this->amp0;
        float ki = 1.f;
        float kj = 1.f;
        int   kseed = this->seed;
        float local_weight = (1.f - ctrl_param) + this->weight * ctrl_param;

        for (int k = 0; k < this->octaves; k++)
        {
          this->p_base->set_seed(kseed);

          float xw = ki * x;
          float yw = kj * y;

          float value = this->p_base->get_value(xw, yw, 0.f);
          float dvdx =
              (this->p_base->get_value(xw + HMAP_GRADIENT_OFFSET, yw, 0.f) -
               this->p_base->get_value(xw - HMAP_GRADIENT_OFFSET, yw, 0.f)) /
              HMAP_GRADIENT_OFFSET;
          float dvdy =
              (this->p_base->get_value(xw, yw + HMAP_GRADIENT_OFFSET, 0.f) -
               this->p_base->get_value(xw, yw - HMAP_GRADIENT_OFFSET, 0.f)) /
              HMAP_GRADIENT_OFFSET;

          value = smoothstep3(0.5f + value);

          dx_sum += dvdx;
          dy_sum += dvdy;

          sum += value * amp /
                 (1.f +
                  this->gradient_scale * (dx_sum * dx_sum + dy_sum * dy_sum));
          amp *= (1.f - local_weight) +
                 local_weight * std::min(value + 1.f, 2.f) * 0.5f;

          ki *= this->lacunarity;
          kj *= this->lacunarity;
          amp *= this->persistence;
          kseed++;
        }
        return sum;
      });
}

FbmJordanFunction::FbmJordanFunction(std::unique_ptr<NoiseFunction> p_base,
                                     int                            octaves,
                                     float                          weight,
                                     float                          persistence,
                                     float                          lacunarity,
                                     float                          warp0,
                                     float                          damp0,
                                     float                          warp_scale,
                                     float                          damp_scale)
    : GenericFractalFunction(std::move(p_base),
                             octaves,
                             weight,
                             persistence,
                             lacunarity),
      warp0(warp0),
      damp0(damp0),
      warp_scale(warp_scale),
      damp_scale(damp_scale)
{
  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        // based on https://www.decarpentier.nl/scape-procedural-extensions
        float sum = 0.f;
        float amp = this->amp0;
        float amp_damp = this->amp0;
        float ki = 1.f;
        float kj = 1.f;
        int   kseed = this->seed;
        float local_weight = (1.f - ctrl_param) + this->weight * ctrl_param;

        // --- 1st octave

        this->p_base->set_seed(kseed);
        float value = this->p_base->get_value(x, y, 0.f);
        float dvdx =
            (this->p_base->get_value(x + HMAP_GRADIENT_OFFSET, y, 0.f) -
             this->p_base->get_value(x - HMAP_GRADIENT_OFFSET, y, 0.f)) /
            HMAP_GRADIENT_OFFSET;
        float dvdy =
            (this->p_base->get_value(x, y + HMAP_GRADIENT_OFFSET, 0.f) -
             this->p_base->get_value(x, y - HMAP_GRADIENT_OFFSET, 0.f)) /
            HMAP_GRADIENT_OFFSET;

        sum += value * value;
        float dx_sum_warp = this->warp0 * value * dvdx;
        float dy_sum_warp = this->warp0 * value * dvdy;
        float dx_sum_damp = this->damp0 * value * dvdx;
        float dy_sum_damp = this->damp0 * value * dvdy;

        amp *= (1.f - local_weight) +
               local_weight * std::min(value * value + 1.f, 2.f) * 0.5f;

        ki *= this->lacunarity;
        kj *= this->lacunarity;
        amp *= this->persistence;
        amp_damp *= this->persistence;
        kseed++;

        // --- other octaves

        for (int k = 0; k < this->octaves; k++)
        {
          this->p_base->set_seed(kseed);

          float xw = ki * x + this->warp_scale * dx_sum_warp;
          float yw = kj * y + this->warp_scale * dy_sum_warp;

          float value = this->p_base->get_value(xw, yw, 0.f);
          float dvdx =
              (this->p_base->get_value(xw + HMAP_GRADIENT_OFFSET, yw, 0.f) -
               this->p_base->get_value(xw - HMAP_GRADIENT_OFFSET, yw, 0.f)) /
              HMAP_GRADIENT_OFFSET;
          float dvdy =
              (this->p_base->get_value(xw, yw + HMAP_GRADIENT_OFFSET, 0.f) -
               this->p_base->get_value(xw, yw - HMAP_GRADIENT_OFFSET, 0.f)) /
              HMAP_GRADIENT_OFFSET;

          sum += amp_damp * value * value;
          dx_sum_warp += this->warp0 * value * dvdx;
          dy_sum_warp += this->warp0 * value * dvdy;
          dx_sum_damp += this->damp0 * value * dvdx;
          dy_sum_damp += this->damp0 * value * dvdy;

          amp *= (1.f - local_weight) +
                 local_weight * std::min(value * value + 1.f, 2.f) * 0.5f;

          ki *= this->lacunarity;
          kj *= this->lacunarity;
          amp *= this->persistence;
          amp_damp = amp * (1.f - this->damp_scale /
                                      (1.f + dx_sum_damp * dx_sum_damp +
                                       dy_sum_damp * dy_sum_damp));
          kseed++;
        }
        return sum;
      });
}

FbmPingpongFunction::FbmPingpongFunction(std::unique_ptr<NoiseFunction> p_base,
                                         int                            octaves,
                                         float                          weight,
                                         float persistence,
                                         float lacunarity)
    : GenericFractalFunction(std::move(p_base),
                             octaves,
                             weight,
                             persistence,
                             lacunarity)
{
  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        float sum = 0.f;
        float amp = this->amp0;
        float ki = 1.f;
        float kj = 1.f;
        int   kseed = this->seed;
        float local_weight = (1.f - ctrl_param) + this->weight * ctrl_param;

        for (int k = 0; k < this->octaves; k++)
        {
          this->p_base->set_seed(kseed);
          float value = (this->p_base->get_value(ki * x, kj * y, 0.f) + 1.f) *
                        2.f;
          value -= (int)(value * 0.5f) * 2;
          value = value < 1 ? value : 2 - value;
          value = smoothstep5(value);

          sum += (value - 0.5f) * 2.f * amp;
          amp *= (1.f - local_weight) + local_weight * value;

          ki *= this->lacunarity;
          kj *= this->lacunarity;
          amp *= this->persistence;
          kseed++;
        }
        return sum;
      });
}

FbmRidgedFunction::FbmRidgedFunction(std::unique_ptr<NoiseFunction> p_base,
                                     int                            octaves,
                                     float                          weight,
                                     float                          persistence,
                                     float                          lacunarity,
                                     float                          k_smoothing)
    : GenericFractalFunction(std::move(p_base),
                             octaves,
                             weight,
                             persistence,
                             lacunarity),
      k_smoothing(k_smoothing)
{
  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        float sum = 0.f;
        float amp = this->amp0;
        float ki = 1.f;
        float kj = 1.f;
        int   kseed = this->seed;
        float local_weight = (1.f - ctrl_param) + this->weight * ctrl_param;

        if (this->k_smoothing == 0.f)
          for (int k = 0; k < this->octaves; k++)
          {
            this->p_base->set_seed(kseed);
            float value = std::abs(
                this->p_base->get_value(ki * x, kj * y, 0.f));
            sum += (1.f - 2.f * value) * amp;
            amp *= 1.f - local_weight * value;

            ki *= this->lacunarity;
            kj *= this->lacunarity;
            amp *= this->persistence;
            kseed++;
          }
        else
          for (int k = 0; k < this->octaves; k++)
          {
            this->p_base->set_seed(kseed);
            float value = this->p_base->get_value(ki * x, kj * y, 0.f);
            value = abs_smooth(value, this->k_smoothing);
            sum += (1.f - 2.f * value) * amp;
            amp *= 1.f - local_weight * value;

            ki *= this->lacunarity;
            kj *= this->lacunarity;
            amp *= this->persistence;
            kseed++;
          }

        return sum;
      });
}

FbmSwissFunction::FbmSwissFunction(std::unique_ptr<NoiseFunction> p_base,
                                   int                            octaves,
                                   float                          weight,
                                   float                          persistence,
                                   float                          lacunarity,
                                   float                          warp_scale)
    : GenericFractalFunction(std::move(p_base),
                             octaves,
                             weight,
                             persistence,
                             lacunarity)
{
  this->set_warp_scale(warp_scale);

  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        // based on https://www.decarpentier.nl/scape-procedural-extensions
        float sum = 0.f;
        float amp = this->amp0;
        float ki = 1.f;
        float kj = 1.f;
        int   kseed = this->seed;
        float local_weight = (1.f - ctrl_param) + this->weight * ctrl_param;

        float dx_sum = 0.f;
        float dy_sum = 0.f;

        for (int k = 0; k < this->octaves; k++)
        {
          this->p_base->set_seed(kseed);

          float xw = ki * x + this->warp_scale_normalized * dx_sum;
          float yw = kj * y + this->warp_scale_normalized * dy_sum;

          float value = this->p_base->get_value(xw, yw, 0.f);
          float dvdx =
              (this->p_base->get_value(xw + HMAP_GRADIENT_OFFSET, yw, 0.f) -
               this->p_base->get_value(xw - HMAP_GRADIENT_OFFSET, yw, 0.f)) /
              HMAP_GRADIENT_OFFSET;
          float dvdy =
              (this->p_base->get_value(xw, yw + HMAP_GRADIENT_OFFSET, 0.f) -
               this->p_base->get_value(xw, yw - HMAP_GRADIENT_OFFSET, 0.f)) /
              HMAP_GRADIENT_OFFSET;

          sum += value * amp;
          dx_sum += amp * dvdx * -(value + 0.5f);
          dy_sum += amp * dvdy * -(value + 0.5f);

          amp *= (1.f - local_weight) +
                 local_weight * std::min(value + 1.f, 2.f) * 0.5f;

          ki *= this->lacunarity;
          kj *= this->lacunarity;
          amp *= this->persistence;
          kseed++;
        }
        return sum;
      });
}

GenericFractalFunction::GenericFractalFunction(
    std::unique_ptr<NoiseFunction> p_base,
    int                            octaves,
    float                          weight,
    float                          persistence,
    float                          lacunarity)
    : p_base(std::move(p_base)),
      octaves(octaves),
      weight(weight),
      persistence(persistence),
      lacunarity(lacunarity)
{
  if (!this->p_base)
  {
    throw std::invalid_argument("Base noise function must not be null.");
  }
  this->set_seed(this->p_base->get_seed());
  this->set_kw(this->p_base->get_kw());
  this->update_amp0();
}

void GenericFractalFunction::update_amp0()
{
  float amp = this->persistence;
  float amp_fractal = 1.0f;
  for (int i = 1; i < this->octaves; i++)
  {
    amp_fractal += amp;
    amp *= this->persistence;
  }
  this->amp0 = 1.f / amp_fractal;
}

} // namespace hmap
