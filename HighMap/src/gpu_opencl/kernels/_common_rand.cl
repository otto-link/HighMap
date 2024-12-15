R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
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

void shuffle_array_i(int *v, int size, int seed)
{
  // use a simple linear congruential generator (LCG) for
  // pseudo-random numbers
  int rng = seed;

  // fisher-Yates shuffle
  for (int i = size - 1; i > 0; i--)
  {
    // generate a random index between 0 and i
    rng = (rng * 1103515245 + 12345) & 0x7FFFFFFF; // LCG
    int j = rng % (i + 1);

    int temp_v = v[i];
    v[i] = v[j];
    v[j] = temp_v;
  }
}

void shuffle_array_f(float *v, int size, int seed)
{
  // use a simple linear congruential generator (LCG) for
  // pseudo-random numbers
  int rng = seed;

  // fisher-Yates shuffle
  for (int i = size - 1; i > 0; i--)
  {
    // generate a random index between 0 and i
    rng = (rng * 1103515245 + 12345) & 0x7FFFFFFF; // LCG
    int j = rng % (i + 1);

    float temp_v = v[i];
    v[i] = v[j];
    v[j] = temp_v;
  }
}
)""
