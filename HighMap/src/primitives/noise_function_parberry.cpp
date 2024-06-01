/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "highmap/math.hpp"
#include "highmap/noise_function.hpp"

namespace hmap
{

// helpers

void normalize2(float v[2])
{
  float s = sqrt(v[0] * v[0] + v[1] * v[1]);
  v[0] /= s;
  v[1] /= s;
}

float randomflt()
{
  return (2.0f * rand()) / RAND_MAX - 1.0f;
}

// methods

ParberryFunction::ParberryFunction(Vec2<float> kw, uint seed, float mu)
    : NoiseFunction(kw, seed), mu(mu)
{
  this->p.resize(this->perlin_b);
  this->g2.resize(this->perlin_b, std::vector<float>(2, 0.f));
  this->m.resize(this->perlin_b);

  this->initialize();
}

void ParberryFunction::initialize()
{
  // --- initialize the permutation, gradient, and magnitude tables

  srand(this->seed);

  // random gradient vectors
  for (int i = 0; i < this->perlin_b; i++)
  {
    g2[i][0] = randomflt();
    g2[i][1] = randomflt();
    normalize2(g2[i].data());
  }

  // random permutation
  for (int i = 0; i < this->perlin_b; i++)
    p[i] = i;

  for (int i = this->perlin_b - 1; i > 0; i--)
    std::swap(p[i], p[rand() % (i + 1)]);

  // gradient magnitude array
  float s = 1.f;

  for (int i = 0; i < this->perlin_b; i++)
  {
    m[i] = s;
    s /= this->mu;
  }

  // --- noise function

  this->function = [this](float x, float y, float)
  {
    int   bx0, bx1, by0, by1;
    float rx0, rx1, ry0, ry1;

    {
      float t = this->kw.x * x + this->perlin_n;
      bx0 = ((int)t) & this->perlin_bm;
      bx1 = (bx0 + 1) & this->perlin_bm;
      rx0 = t - (int)t;
      rx1 = rx0 - 1.0f;
    }

    {
      float t = this->kw.y * y + this->perlin_n;
      by0 = ((int)t) & this->perlin_bm;
      by1 = (by0 + 1) & this->perlin_bm;
      ry0 = t - (int)t;
      ry1 = ry0 - 1.0f;
    }

    int b00 = this->p[(this->p[bx0] + by0) & this->perlin_bm];
    int b10 = this->p[(this->p[bx1] + by0) & this->perlin_bm];
    int b01 = this->p[(this->p[bx0] + by1) & this->perlin_bm];
    int b11 = this->p[(this->p[bx1] + by1) & this->perlin_bm];

    float sx = smoothstep5(rx0);

    float u, v;
    u = this->m[b00] * (rx0 * this->g2[b00][0] + ry0 * this->g2[b00][1]);
    v = this->m[b10] * (rx1 * this->g2[b10][0] + ry0 * this->g2[b10][1]);
    float a = lerp(u, v, sx);

    u = this->m[b01] * (rx0 * this->g2[b01][0] + ry1 * this->g2[b01][1]);
    v = this->m[b11] * (rx1 * this->g2[b11][0] + ry1 * this->g2[b11][1]);
    float b = lerp(u, v, sx);

    float sy = smoothstep5(ry0);

    return lerp(a, b, sy);
  };
}

} // namespace hmap
