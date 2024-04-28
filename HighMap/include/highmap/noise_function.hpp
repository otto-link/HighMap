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
#include "libInterpolate/AnyInterpolator.hpp"
#include "libInterpolate/Interpolate.hpp"
#include "macrologger.h"

#include "highmap/op.hpp"

#define HMAP_NOISE_FCT_TYPE std::function<float(float, float, float)>

#define HMAP_GRADIENT_OFFSET 0.001f

namespace hmap
{

/**
 * @brief Noise type.
 */
enum NoiseType : int
{
  n_perlin,          ///< Perlin
  n_perlin_billow,   ///< Perlin billow
  n_perlin_half,     ///< Perlin half
  n_simplex2,        ///< OpenSimplex2
  n_simplex2s,       ///< OpenSimplex2S
  n_value,           ///< Value
  n_value_cubic,     ///< Value (cubic)
  n_value_delaunay,  ///< Value (delaunay)
  n_value_linear,    ///< Value (linear)
  n_value_thinplate, ///< Value (thinplate)
  n_worley,          ///< Worley
  n_worley_double,   ///< Worley double
  n_worley_value,    ///< Worley (cell value return)
};

enum FractalType : int
{
  n_none,
  n_fbm,
  n_
};

//----------------------------------------
// Base function class
//----------------------------------------

class NoiseFunction
{
public:
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

  Vec2<float> get_kw() const
  {
    return this->kw;
  }

  uint get_seed() const
  {
    return this->seed;
  }

  virtual void set_seed(uint new_seed)
  {
    this->seed = new_seed;
  }

  virtual void set_kw(Vec2<float> new_kw)
  {
    this->kw = new_kw;
  }

protected:
  Vec2<float> kw;
  uint        seed;
};

// helper

std::unique_ptr<hmap::NoiseFunction> create_noise_function_from_type(
    NoiseType   noise_type,
    Vec2<float> kw,
    uint        seed);

//----------------------------------------
// Actual functions
//----------------------------------------

/**
 * @brief Array (x, y) function class.
 */
class ArrayFunction : public NoiseFunction
{
public:
  /**
   * @brief Construct a new Array Function object
   *
   * @param array Data array.
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param periodic Wether the domain is periodic or not.
   */
  ArrayFunction(hmap::Array array, Vec2<float> kw, bool periodic = true);

  /**
   * @brief Set the array object.
   *
   * @param new_array New data array.
   */
  void set_array(hmap::Array new_array)
  {
    this->array = new_array;
  }

private:
  hmap::Array array;
};

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
 * @brief Value Cubic noise (x, y) function class.
 */
class ValueCubicNoiseFunction : public NoiseFunction
{
public:
  /**
   * @brief Construct a new ValueCubicNoiseFunction object.
   *
   * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
   * a unit domain.
   * @param seed Random seed number.
   */
  ValueCubicNoiseFunction(Vec2<float> kw, uint seed);

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
 * @brief ValueDelaunayNoise (x, y) function class.
 */
class ValueDelaunayNoiseFunction : public NoiseFunction
{
public:
  /**
   * @brief Construct a new ValueNoiseFunction object.
   *
   * @param kw Noise wavenumber, with respect to
   * a unit domain.
   * @param seed Random seed number.
   */
  ValueDelaunayNoiseFunction(Vec2<float> kw, uint seed);

  /**
   * @brief Set the wavenumber attribute.
   *
   * @param new_kw New kw.
   */
  void set_kw(Vec2<float> new_kw)
  {
    this->kw = new_kw;
    this->update_interpolation_function();
  }

  /**
   * @brief Set the seed attribute.
   *
   * @param new_seed New seed.
   */
  void set_seed(uint new_seed)
  {
    this->seed = new_seed;
    this->update_interpolation_function();
  }

  /**
   * @brief Update base interpolation.
   */
  void update_interpolation_function()
  {
    // generate 'n' random grid points
    int n = (int)(this->kw.x * this->kw.y);

    std::vector<float> x(n);
    std::vector<float> y(n);
    std::vector<float> value(n);

    random_grid(x, y, value, this->seed, {0.f, 1.f, 0.f, 1.f});
    expand_grid(x, y, value, {0.f, 1.f, 0.f, 1.f});
    this->interp = _2D::LinearDelaunayTriangleInterpolator<float>();
    this->interp.setData(x, y, value);
  }

private:
  /**
   * @brief Interpolation function object.
   */
  _2D::AnyInterpolator<
      float,
      void(std::vector<float>, std::vector<float>, std::vector<float>)>
      interp;
};

/**
 * @brief ValueLinearNoiseFunction (x, y) function class.
 */
class ValueLinearNoiseFunction : public NoiseFunction
{
public:
  /**
   * @brief Construct a new ValueLinearNoiseFunction object.
   *
   * @param kw Noise wavenumber, with respect to
   * a unit domain.
   * @param seed Random seed number.
   */
  ValueLinearNoiseFunction(Vec2<float> kw, uint seed);

  /**
   * @brief Set the wavenumber attribute.
   *
   * @param new_kw New kw.
   */
  void set_kw(Vec2<float> new_kw)
  {
    this->kw = new_kw;
    this->update_interpolation_function();
  }

  /**
   * @brief Set the seed attribute.
   *
   * @param new_seed New seed.
   */
  void set_seed(uint new_seed)
  {
    this->seed = new_seed;
    this->update_interpolation_function();
  }

  /**
   * @brief Update base interpolation.
   */
  void update_interpolation_function()
  {
    // generate random values on a regular coarse grid (adjust extent
    // according to the input noise in order to avoid "holes" in the
    // data for large noise displacement)
    Vec4<float> bbox = {-1.f, 2.f, -1.f, 2.f}; // bounding box

    float lx = bbox.b - bbox.a;
    float ly = bbox.d - bbox.c;

    Vec2<int> shape_base = Vec2<int>((int)(kw.x * lx) + 1,
                                     (int)(kw.y * ly) + 1);

    std::vector<float> value;
    value.reserve(shape_base.x * shape_base.y);

    {
      std::mt19937                          gen(seed);
      std::uniform_real_distribution<float> dis(0.f, 1.f);
      for (int k = 0; k < shape_base.x * shape_base.y; k++)
        value.push_back(dis(gen));
    }

    // corresponding grids
    Array xv = Array(shape_base);
    Array yv = Array(shape_base);

    for (int i = 0; i < shape_base.x; i++)
      for (int j = 0; j < shape_base.y; j++)
      {
        xv(i, j) = bbox.a + lx * (float)i / (float)(shape_base.x - 1);
        yv(i, j) = bbox.c + ly * (float)j / (float)(shape_base.y - 1);
      }

    this->interp = _2D::BilinearInterpolator<float>();
    this->interp.setData(xv.vector, yv.vector, value);
  }

private:
  /**
   * @brief Interpolation function object.
   */
  _2D::AnyInterpolator<
      float,
      void(std::vector<float>, std::vector<float>, std::vector<float>)>
      interp;
};

/**
 * @brief ValueThinplateNoise (x, y) function class.
 */
class ValueThinplateNoiseFunction : public NoiseFunction
{
public:
  /**
   * @brief Construct a new ValueThinplateNoiseFunction object.
   *
   * @param kw Noise wavenumber, with respect to
   * a unit domain.
   * @param seed Random seed number.
   */
  ValueThinplateNoiseFunction(Vec2<float> kw, uint seed);

  /**
   * @brief Set the wavenumber attribute.
   *
   * @param new_kw New kw.
   */
  void set_kw(Vec2<float> new_kw)
  {
    this->kw = new_kw;
    this->update_interpolation_function();
  }

  /**
   * @brief Set the seed attribute.
   *
   * @param new_seed New seed.
   */
  void set_seed(uint new_seed)
  {
    this->seed = new_seed;
    this->update_interpolation_function();
  }

  /**
   * @brief Update base interpolation.
   */
  void update_interpolation_function()
  {
    // generate 'n' random grid points
    int n = (int)(this->kw.x * this->kw.y);

    std::vector<float> x(n);
    std::vector<float> y(n);
    std::vector<float> value(n);

    random_grid(x, y, value, this->seed, {0.f, 1.f, 0.f, 1.f});
    expand_grid(x, y, value, {0.f, 1.f, 0.f, 1.f});
    this->interp = _2D::ThinPlateSplineInterpolator<float>();
    this->interp.setData(x, y, value);
  }

private:
  /**
   * @brief Interpolation function object.
   */
  _2D::AnyInterpolator<
      float,
      void(std::vector<float>, std::vector<float>, std::vector<float>)>
      interp;
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
    this->set_seed(this->p_base->get_seed());
    this->set_kw(this->p_base->get_kw());
    this->update_amp0();
  }

  void set_kw(Vec2<float> new_kw)
  {
    this->kw = new_kw;
    this->p_base->set_kw(new_kw);
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
    this->p_base->set_seed(new_seed);
  }

  void scale_amp0(float scale)
  {
    this->amp0 *= scale;
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

/**
 * @brief Fbm layering function
 */
class FbmFunction : public GenericFractalFunction
{
public:
  /**
   * @brief Construct a new Fbm Function object.
   *
   * @param octaves Number of octaves.
   * @param weigth Octave weighting.
   * @param persistence Octave persistence.
   * @param lacunarity Defines the wavenumber ratio between each octaves.
   */
  FbmFunction(NoiseFunction *p_base,
              int            octaves,
              float          weight,
              float          persistence,
              float          lacunarity);
};

/**
 * @brief Iq layering function
 */
class FbmIqFunction : public GenericFractalFunction
{
public:
  /**
   * @brief Construct a new Fbm Iq Function object
   *
   * @param octaves Number of octaves.
   * @param weigth Octave weighting.
   * @param persistence Octave persistence.
   * @param lacunarity Defines the wavenumber ratio between each octaves.
   * @param gradient_scale Gradient scale influence.
   */
  FbmIqFunction(NoiseFunction *p_base,
                int            octaves,
                float          weight,
                float          persistence,
                float          lacunarity,
                float          gradient_scale);

protected:
  float gradient_scale;
};

/**
 * @brief Jordan layering function
 */
class FbmJordanFunction : public GenericFractalFunction
{
public:
  /**
   * @brief Construct a new Fbm Jordan Function object
   *
   * @param octaves Number of octaves.
   * @param weigth Octave weighting.
   * @param persistence Octave persistence.
   * @param lacunarity Defines the wavenumber ratio between each octaves.
   * @param warp0 Initial warp.
   * @param damp0 Initial damp.
   * @param warp_scale Warp scale.
   * @param damp_scale Damp scale.
   */
  FbmJordanFunction(NoiseFunction *p_base,
                    int            octaves,
                    float          weight,
                    float          persistence,
                    float          lacunarity,
                    float          warp0,
                    float          damp0,
                    float          warp_scale,
                    float          damp_scale);

protected:
  float warp0;
  float damp0;
  float warp_scale;
  float damp_scale;
};

/**
 * @brief Pingpong layering function
 */
class FbmPingpongFunction : public GenericFractalFunction
{
public:
  /**
   * @brief Construct a new Fbm Pingpong Function object.
   *
   * @param octaves Number of octaves.
   * @param weigth Octave weighting.
   * @param persistence Octave persistence.
   * @param lacunarity Defines the wavenumber ratio between each octaves.
   */
  FbmPingpongFunction(NoiseFunction *p_base,
                      int            octaves,
                      float          weight,
                      float          persistence,
                      float          lacunarity);

protected:
  float k_smoothing;
};

/**
 * @brief Ridged layering function
 */
class FbmRidgedFunction : public GenericFractalFunction
{
public:
  /**
   * @brief Construct a new Fbm Ridged Function object.
   *
   * @param octaves Number of octaves.
   * @param weigth Octave weighting.
   * @param persistence Octave persistence.
   * @param lacunarity Defines the wavenumber ratio between each octaves.
   * @param k_smoothing Smoothing parameter.
   */
  FbmRidgedFunction(NoiseFunction *p_base,
                    int            octaves,
                    float          weight,
                    float          persistence,
                    float          lacunarity,
                    float          k_smoothing);

protected:
  float k_smoothing;
};

/**
 * @brief Swiss layering function
 */
class FbmSwissFunction : public GenericFractalFunction
{
public:
  /**
   * @brief Construct a new Fbm Swiss Function object
   *
   * @param octaves Number of octaves.
   * @param weigth Octave weighting.
   * @param persistence Octave persistence.
   * @param lacunarity Defines the wavenumber ratio between each octaves.
   * @param warp_scale Warping scale.
   */
  FbmSwissFunction(NoiseFunction *p_base,
                   int            octaves,
                   float          weight,
                   float          persistence,
                   float          lacunarity,
                   float          warp_scale);

  /**
   * @brief Set the warp scale object
   *
   * @param new_warp_scale New warp scale.
   */
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
