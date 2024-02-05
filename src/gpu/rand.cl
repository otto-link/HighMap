R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

// https://github.com/fvalle1/latentrees/blob/main/independentrees/opencl/random.cl
uint wang_hash(uint seed)
{
  seed = (seed ^ 61) ^ (seed >> 16);
  seed *= 9;
  seed = seed ^ (seed >> 4);
  seed *= 0x27d4eb2d;
  seed = seed ^ (seed >> 15);
  return seed;
}

void rand_xorshift(uint *p_rng_state)
{
  // Xorshift algorithm from George Marsaglia's paper
  *p_rng_state ^= (*p_rng_state << 13);
  *p_rng_state ^= (*p_rng_state >> 17);
  *p_rng_state ^= (*p_rng_state << 5);
}

float rand(uint *p_rng_state)
{
  rand_xorshift(p_rng_state);
  return *p_rng_state * (1.f / 4294967296.f);
}
)""
