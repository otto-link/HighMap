/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file noise_function.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <functional>

#include "FastNoiseLite.h"
#include "macrologger.h"

#include "highmap/op.hpp"

#define HMAP_NOISE_FCT_TYPE std::function<float(float, float, float)>

#define HMAP_GRADIENT_OFFSET 0.001f

namespace hmap
{

//----------------------------------------
// Base function class
//----------------------------------------

class NoiseFunction
{
public:
  Vec2<float>         kw;
  uint                seed;
  HMAP_NOISE_FCT_TYPE function = [](float, float, float initial_value)
  { return initial_value; };

  NoiseFunction() = default;

  NoiseFunction(Vec2<float> kw) : kw(kw), seed(0)
  {
  }

  NoiseFunction(Vec2<float> kw, uint seed) : kw(kw), seed(seed)
  {
  }

  HMAP_NOISE_FCT_TYPE get_function() const
  {
    return this->function;
  }

  NoiseFunction *get_base_ref() const
  {
    return (NoiseFunction *)this;
  }

  virtual void set_seed(uint new_seed)
  {
    this->seed = new_seed;
  }

  virtual void set_kw(Vec2<float> new_kw)
  {
    this->kw = new_kw;
  }
};

//----------------------------------------
// Actual functions
//----------------------------------------

/**
 * @brief Perlin (x, y) function class.
 */
class PerlinFunction : public NoiseFunction
{
public:
  /**
   * @brief Construct a new Perlin Function object.
   *
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param seed Random seed number.
   */
  PerlinFunction(Vec2<float> kw, uint seed);

  /**
   * @brief Set the seed attribute.
   *
   * @param new_seed New seed.
   */
  void set_seed(uint new_seed)
  {
    this->seed = new_seed;
    this->noise.SetSeed(new_seed);
  }

private:
  /**
   * @brief FastNoiseLite noise generator object.
   */
  FastNoiseLite noise;
};

/**
 * @brief Perlin 'billow' (x, y) function class.
 */
class PerlinBillowFunction : public NoiseFunction
{
public:
  /**
   * @brief Construct a new Perlin Function object.
   *
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param seed Random seed number.
   */
  PerlinBillowFunction(Vec2<float> kw, uint seed);

  /**
   * @brief Set the seed attribute.
   *
   * @param new_seed New seed.
   */
  void set_seed(uint new_seed)
  {
    this->seed = new_seed;
    this->noise.SetSeed(new_seed);
  }

private:
  /**
   * @brief FastNoiseLite noise generator object.
   */
  FastNoiseLite noise;
};

/**
 * @brief Perlin 'half' (x, y) function class.
 */
class PerlinHalfFunction : public NoiseFunction
{
public:
  float k;

  /**
   * @brief Construct a new Perlin Function object.
   *
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param seed Random seed number.
   * @param k Smoothing factor.
   */
  PerlinHalfFunction(Vec2<float> kw, uint seed, float k);

  /**
   * @brief Set the seed attribute.
   *
   * @param new_seed New seed.
   */
  void set_seed(uint new_seed)
  {
    this->seed = new_seed;
    this->noise.SetSeed(new_seed);
  }

private:
  /**
   * @brief FastNoiseLite noise generator object.
   */
  FastNoiseLite noise;
};

/**
 * @brief Perlin 'mix' (x, y) function class.
 */
class PerlinMixFunction : public NoiseFunction
{
public:
  /**
   * @brief Construct a new Perlin Function object.
   *
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param seed Random seed number.
   */
  PerlinMixFunction(Vec2<float> kw, uint seed);

  /**
   * @brief Set the seed attribute.
   *
   * @param new_seed New seed.
   */
  void set_seed(uint new_seed)
  {
    this->seed = new_seed;
    this->noise.SetSeed(new_seed);
  }

private:
  /**
   * @brief FastNoiseLite noise generator object.
   */
  FastNoiseLite noise;
};

/**
 * @brief OpenSimplex2 (x, y) function class.
 */
class Simplex2Function : public NoiseFunction
{
public:
  /**
   * @brief Construct a new Perlin Function object.
   *
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param seed Random seed number.
   */
  Simplex2Function(Vec2<float> kw, uint seed);

  /**
   * @brief Set the seed attribute.
   *
   * @param new_seed New seed.
   */
  void set_seed(uint new_seed)
  {
    this->seed = new_seed;
    this->noise.SetSeed(new_seed);
  }

private:
  /**
   * @brief FastNoiseLite noise generator object.
   */
  FastNoiseLite noise;
};

/**
 * @brief OpenSimplex2S (x, y) function class.
 */
class Simplex2SFunction : public NoiseFunction
{
public:
  /**
   * @brief Construct a new Perlin Function object.
   *
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param seed Random seed number.
   */
  Simplex2SFunction(Vec2<float> kw, uint seed);

  /**
   * @brief Set the seed attribute.
   *
   * @param new_seed New seed.
   */
  void set_seed(uint new_seed)
  {
    this->seed = new_seed;
    this->noise.SetSeed(new_seed);
  }

private:
  /**
   * @brief FastNoiseLite noise generator object.
   */
  FastNoiseLite noise;
};

/**
 * @brief Slope (x, y) function class.
 */
class SlopeFunction : public NoiseFunction
{
public:
  /**
   * @brief Overall rotation angle (in degree).
   */
  float angle;

  /**
   * @brief Slope.
   */
  float slope;

  /**
   * @brief Primitive reference center.
   */
  Vec2<float> center;

  /**
   * @brief Construct a new Slope Function object
   *
   * @param angle Overall rotation angle (in degree).
   * @param slope Step slope.
   * @param center Primitive reference center.
   */
  SlopeFunction(float angle, float slope, Vec2<float> center);

  /**
   * @brief Set the angle.
   *
   * @param new_angle New angle
   */
  void set_angle(float new_angle)
  {
    this->angle = new_angle;
    this->ca = std::cos(angle / 180.f * M_PI);
    this->sa = std::sin(angle / 180.f * M_PI);
  }

private:
  float ca;
  float sa;
};

/**
 * @brief Step (x, y) function class.
 */
class StepFunction : public NoiseFunction
{
public:
  /**
   * @brief Overall rotation angle (in degree).
   */
  float angle;

  /**
   * @brief Step slope.
   */
  float slope;

  /**
   * @brief Primitive reference center.
   */
  Vec2<float> center;

  /**
   * @brief Construct a new Step Function object
   *
   * @param angle Overall rotation angle (in degree).
   * @param slope Step slope.
   * @param center Primitive reference center.
   */
  StepFunction(float angle, float slope, Vec2<float> center);

  /**
   * @brief Set the angle.
   *
   * @param new_angle New angle
   */
  void set_angle(float new_angle)
  {
    this->angle = new_angle;
    this->ca = std::cos(angle / 180.f * M_PI);
    this->sa = std::sin(angle / 180.f * M_PI);
  }

private:
  float ca;
  float sa;
};

/**
 * @brief Value noise (x, y) function class.
 */
class ValueNoiseFunction : public NoiseFunction
{
public:
  /**
   * @brief Construct a new ValueNoiseFunction object.
   *
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param seed Random seed number.
   */
  ValueNoiseFunction(Vec2<float> kw, uint seed);

  /**
   * @brief Set the seed attribute.
   *
   * @param new_seed New seed.
   */
  void set_seed(uint new_seed)
  {
    this->seed = new_seed;
    this->noise.SetSeed(new_seed);
  }

private:
  /**
   * @brief FastNoiseLite noise generator object.
   */
  FastNoiseLite noise;
};

/**
 * @brief Worley (x, y) function class.
 */
class WorleyFunction : public NoiseFunction
{
public:
  /**
   * @brief Construct a new Worley Function object.
   *
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param seed Random seed number.
   */
  WorleyFunction(Vec2<float> kw, uint seed, bool return_cell_value = false);

  /**
   * @brief Set the seed attribute.
   *
   * @param new_seed New seed.
   */
  void set_seed(uint new_seed)
  {
    this->seed = new_seed;
    this->noise.SetSeed(new_seed);
  }

private:
  /**
   * @brief FastNoiseLite noise generator object.
   */
  FastNoiseLite noise;
};

/**
 * @brief Worley (x, y) function class.
 */
class WorleyDoubleFunction : public NoiseFunction
{
public:
  /**
   * @brief Amplitude ratio between each Worley noise.
   */
  float ratio;

  /**
   * @brief Transition smoothing parameter.
   */
  float k;

  /**
   * @brief Construct a new Worley Double Function object.
   *
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param seed Random seed number.
   * @param ratio Amplitude ratio between each Worley noise.
   * @param k Transition smoothing parameter.
   */
  WorleyDoubleFunction(Vec2<float> kw, uint seed, float ratio, float k);

  /**
   * @brief Set the seed attribute.
   *
   * @param new_seed New seed.
   */
  void set_seed(uint new_seed)
  {
    this->seed = new_seed;
    this->noise1.SetSeed(new_seed);
    this->noise2.SetSeed(new_seed + 1);
  }

private:
  /**
   * @brief FastNoiseLite noise generator objects.
   */
  FastNoiseLite noise1, noise2;
};

/**
 * @brief Wave dune (x, y) function class.
 */
class WaveDuneFunction : public NoiseFunction
{
public:
  /**
   * @brief Overall rotation angle (in degree).
   */
  float angle;

  /**
   * @brief Relative location of the top of the dune profile (in [0, 1]).
   */
  float xtop;

  /**
   * @brief Relative location of the foot of the dune profile (in [0, 1]).
   */
  float xbottom;

  /**
   * @brief hase shift (in radians).
   */
  float phase_shift;

  /**
   * @brief Construct a new Wave Dune Function object.
   *
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param angle Overall rotation angle (in degree).
   * @param slant_ratio Relative location of the triangle apex, in [0, 1].
   * @param phase_shift Phase shift (in radians).
   */
  WaveDuneFunction(Vec2<float> kw,
                   float       angle,
                   float       xtop,
                   float       xbottom,
                   float       phase_shift);

  /**
   * @brief Set the angle.
   *
   * @param new_angle New angle
   */
  void set_angle(float new_angle)
  {
    this->angle = new_angle;
    this->ca = std::cos(angle / 180.f * M_PI);
    this->sa = std::sin(angle / 180.f * M_PI);
  }

private:
  float ca;
  float sa;
};

/**
 * @brief Wave sine (x, y) function class.
 */
class WaveSineFunction : public NoiseFunction
{
public:
  /**
   * @brief Overall rotation angle (in degree).
   */
  float angle;

  /**
   * @brief hase shift (in radians).
   */
  float phase_shift;

  /**
   * @brief Construct a new Wave Sine Function object.
   *
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param kw Wavenumber with respect to a unit domain.
   * @param angle Overall rotation angle (in degree).
   * @param phase_shift Phase shift (in radians).
   */
  WaveSineFunction(Vec2<float> kw, float angle, float phase_shift);

  /**
   * @brief Set the angle.
   *
   * @param new_angle New angle
   */
  void set_angle(float new_angle)
  {
    this->angle = new_angle;
    this->ca = std::cos(angle / 180.f * M_PI);
    this->sa = std::sin(angle / 180.f * M_PI);
  }

private:
  float ca;
  float sa;
};

/**
 * @brief Wave square (x, y) function class.
 */
class WaveSquareFunction : public NoiseFunction
{
public:
  /**
   * @brief Overall rotation angle (in degree).
   */
  float angle;

  /**
   * @brief hase shift (in radians).
   */
  float phase_shift;

  /**
   * @brief Construct a new Wave Square Function object.
   *
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param kw Wavenumber with respect to a unit domain.
   * @param angle Overall rotation angle (in degree).
   * @param phase_shift Phase shift (in radians).
   */
  WaveSquareFunction(Vec2<float> kw, float angle, float phase_shift);

  /**
   * @brief Set the angle.
   *
   * @param new_angle New angle
   */
  void set_angle(float new_angle)
  {
    this->angle = new_angle;
    this->ca = std::cos(angle / 180.f * M_PI);
    this->sa = std::sin(angle / 180.f * M_PI);
  }

private:
  float ca;
  float sa;
};

/**
 * @brief Wave triangular (x, y) function class.
 */
class WaveTriangularFunction : public NoiseFunction
{
public:
  /**
   * @brief Overall rotation angle (in degree).
   */
  float angle;

  /**
   * @brief Relative location of the triangle apex, in [0, 1].
   */
  float slant_ratio;

  /**
   * @brief hase shift (in radians).
   */
  float phase_shift;

  /**
   * @brief Construct a new Wave Triangular Function object.
   *
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param angle Overall rotation angle (in degree).
   * @param slant_ratio Relative location of the triangle apex, in [0, 1].
   * @param phase_shift Phase shift (in radians).
   */
  WaveTriangularFunction(Vec2<float> kw,
                         float       angle,
                         float       slant_ratio,
                         float       phase_shift);

  /**
   * @brief Set the angle.
   *
   * @param new_angle New angle
   */
  void set_angle(float new_angle)
  {
    this->angle = new_angle;
    this->ca = std::cos(angle / 180.f * M_PI);
    this->sa = std::sin(angle / 180.f * M_PI);
  }

private:
  float ca;
  float sa;
};

//----------------------------------------
// Fractal layering functions
//----------------------------------------

class GenericFractalFunction : public NoiseFunction
{
public:
  GenericFractalFunction(NoiseFunction *p_base,
                         int            octaves,
                         float          weight,
                         float          persistence,
                         float          lacunarity)
      : p_base(p_base), octaves(octaves), weight(weight),
        persistence(persistence), lacunarity(lacunarity)
  {
    // backup base noise infos
    this->set_seed(this->p_base->seed);
    this->set_kw(this->p_base->kw);
    this->update_amp0();
  }

  void set_kw(Vec2<float> new_kw)
  {
    this->kw = new_kw;
    this->p_base->kw = new_kw;
  }

  void set_lacunarity(float new_lacunarity)
  {
    this->lacunarity = new_lacunarity;
  }

  void set_octaves(int new_octaves)
  {
    this->octaves = new_octaves;
    this->update_amp0();
  }

  void set_persistence(float new_persistence)
  {
    this->persistence = new_persistence;
    this->update_amp0();
  }

  void set_seed(uint new_seed)
  {
    this->seed = new_seed;
    this->p_base->seed = new_seed;
  }

  void update_amp0()
  {
    // determine initial amplitude so that the final field has roughly
    // a unit peak-to-peak amplitude
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
  }

protected:
  NoiseFunction *p_base;
  int            octaves;
  float          weight;
  float          persistence;
  float          lacunarity;
  float          amp0;
};

class FbmFunction : public GenericFractalFunction
{
public:
  FbmFunction(NoiseFunction *p_base,
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
};

class FbmRidgedFunction : public GenericFractalFunction
{
public:
  FbmRidgedFunction(NoiseFunction *p_base,
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
        float value = std::abs(this->p_base->function(ki * x, kj * y, 0.f));
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
};

class FbmSwissFunction : public GenericFractalFunction
{
public:
  FbmSwissFunction(NoiseFunction *p_base,
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
        float dvdx =
            (this->p_base->function(xw + HMAP_GRADIENT_OFFSET, yw, 0.f) -
             this->p_base->function(xw - HMAP_GRADIENT_OFFSET, yw, 0.f)) /
            HMAP_GRADIENT_OFFSET;
        float dvdy =
            (this->p_base->function(xw, yw + HMAP_GRADIENT_OFFSET, 0.f) -
             this->p_base->function(xw, yw - HMAP_GRADIENT_OFFSET, 0.f)) /
            HMAP_GRADIENT_OFFSET;

        // LOG_DEBUG("%f", value);

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

  void set_warp_scale(float new_warp_scale)
  {
    this->warp_scale = new_warp_scale;
    this->warp_scale_normalized = new_warp_scale / kw.x;
  }

protected:
  float warp_scale;
  float warp_scale_normalized;
};

} // namespace hmap
