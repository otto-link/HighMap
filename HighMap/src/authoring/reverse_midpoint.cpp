/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <random>

#include "macrologger.h"

#include "highmap/array.hpp"

namespace hmap
{

// Modeling landscapes with ridges and rivers: bottom up approach
// https://dl.acm.org/doi/abs/10.1145/1101389.1101479

void square_fill_md(Array                                &array,
                    Mat<int>                             &is_done,
                    int                                   i1,
                    int                                   i2,
                    int                                   j1,
                    int                                   j2,
                    float                                 noise_scale,
                    std::mt19937                          gen,
                    std::uniform_real_distribution<float> dis)
{
  int step = i2 - i1;

  while (step > 1)
  {
    float dx = noise_scale * (float)step / (float)array.shape.x;

    std::vector<int> di = {step / 2, -step / 2, step / 2, -step / 2};
    std::vector<int> dj = {step / 2, step / 2, -step / 2, -step / 2};

    std::vector<int> di_s = {0, 0, step / 2, -step / 2};
    std::vector<int> dj_s = {step / 2, -step / 2, 0, 0};

    // diamond phase
    for (int i = i1 + step / 2; i <= i2 - step / 2; i += step)
      for (int j = j1 + step / 2; j <= j2 - step / 2; j += step)
      {

        if (is_done(i, j) == 0)
        {
          is_done(i, j) = 1;

          // average four corners
          float avg = 0.f;
          for (int k = 0; k < 4; k++)
            avg += array(i + di[k], j + dj[k]);

          avg *= 0.25f;

          array(i, j) = avg + dx * dis(gen);
        }
      }

    // square

    for (int i = i1; i <= i2 - step; i += step)
      for (int j = j1; j <= j2; j += step)
      {
        int ic = i + step / 2;
        int jc = j;

        if (is_done(ic, jc) == 0)
        {
          is_done(ic, jc) = 1;

          float avg = 0.f;
          int   navg = 0;

          if (ic - step / 2 >= i1)
          {
            avg += array(ic - step / 2, jc);
            navg += 1;
          }
          if (ic + step / 2 <= i2)
          {
            avg += array(ic + step / 2, jc);
            navg += 1;
          }
          if (jc - step / 2 >= j1)
          {
            avg += array(ic, jc - step / 2);
            navg += 1;
          }
          if (jc + step / 2 <= j2)
          {
            avg += array(ic, jc + step / 2);
            navg += 1;
          }

          avg /= (float)navg;

          array(ic, jc) = avg + dx * dis(gen);
        }
      }

    for (int i = i1; i <= i2; i += step)
      for (int j = j1; j <= j2 - step / 2; j += step)
      {
        int ic = i;
        int jc = j + step / 2;

        if (is_done(ic, jc) == 0)
        {
          is_done(ic, jc) = 1;

          float avg = 0.f;
          int   navg = 0;

          if (ic - step / 2 >= i1)
          {
            avg += array(ic - step / 2, jc);
            navg += 1;
          }
          if (ic + step / 2 <= i2)
          {
            avg += array(ic + step / 2, jc);
            navg += 1;
          }
          if (jc - step / 2 >= j1)
          {
            avg += array(ic, jc - step / 2);
            navg += 1;
          }
          if (jc + step / 2 <= j2)
          {
            avg += array(ic, jc + step / 2);
            navg += 1;
          }

          avg /= (float)navg;

          array(ic, jc) = avg + dx * dis(gen);
        }
      }

    step /= 2;
  }
}

void square_md(Array                                &array,
               Mat<int>                             &is_done,
               int                                   step,
               float                                 noise_scale,
               std::mt19937                          gen,
               std::uniform_real_distribution<float> dis)
{
  Array    array_work = array;
  Mat<int> is_done_work = is_done;

  float dx = noise_scale * (float)step / (float)array.shape.x;

  for (int i = 0; i < array.shape.x - step; i += step)
    for (int j = 0; j < array.shape.y - step; j += step)
    {
      // among the four corners of the current square, find number
      // of corners with defined values
      int nn_s = is_done(i, j) + is_done(i + step, j) +
                 is_done(i + step, j + step) + is_done(i, j + step);

      if (nn_s > 0 && nn_s < 4)
      {
        float avg = is_done(i, j) * array(i, j) +
                    is_done(i + step, j) * array(i + step, j) +
                    is_done(i + step, j + step) * array(i + step, j + step) +
                    is_done(i, j + step) * array(i, j + step);
        avg /= nn_s;

        // update corners
        if (is_done(i, j) == 0)
        {
          is_done_work(i, j) = 1;
          array_work(i, j) = avg + dx * dis(gen);
        }

        if (is_done(i + step, j) == 0)
        {
          is_done_work(i + step, j) = 1;
          array_work(i + step, j) = avg + dx * dis(gen);
        }

        if (is_done(i + step, j + step) == 0)
        {
          is_done_work(i + step, j + step) = 1;
          array_work(i + step, j + step) = avg + dx * dis(gen);
        }

        if (is_done(i, j + step) == 0)
        {
          is_done_work(i, j + step) = 1;
          array_work(i, j + step) = avg + dx * dis(gen);
        }
      }
    }

  array = array_work;
  is_done = is_done_work;

  if (step > 1)
  {
    for (int i = 0; i < array.shape.x - step; i += step)
      for (int j = 0; j < array.shape.y - step; j += step)
      {
        int nn_s = is_done(i, j) + is_done(i + step, j) +
                   is_done(i + step, j + step) + is_done(i, j + step);

        if (nn_s == 4)
          square_fill_md(array,
                         is_done,
                         i,
                         i + step,
                         j,
                         j + step,
                         noise_scale,
                         gen,
                         dis);
      }
  }

  // next scale
  step *= 2;

  if (step < array.shape.x)
    square_md(array, is_done, step, noise_scale, gen, dis);
}

Array reverse_midpoint(const Array &array,
                       uint         seed,
                       float        noise_scale,
                       float        threshold)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(-1.f, 1.f);

  // work on a 2^n + 1 cells array to simplify things
  Array    array_w = Array(Vec2<int>(array.shape.x + 1, array.shape.y + 1));
  Mat<int> is_done = Mat<int>(array_w.shape);

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      array_w(i, j) = array(i, j);
      if (array(i, j) > threshold)
        is_done(i, j) = 1;
      else
        is_done(i, j) = 0;
    }

  // perform reverse midpoint
  int step = 1;
  square_md(array_w, is_done, step, noise_scale, gen, dis);

  // fall back to original array size
  Array array_out = array_w.extract_slice(
      Vec4<int>(0, array.shape.x, 0, array.shape.y));

  return array_out;
}

} // namespace hmap
