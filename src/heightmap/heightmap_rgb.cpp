/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>
#include <future>
#include <thread>

#include "Interpolate.hpp"
#include "macrologger.h"

#include "highmap/colormaps.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"

namespace hmap
{

HeightMapRGB::HeightMapRGB()
{
  this->rgb.resize(3);
}

HeightMapRGB::HeightMapRGB(HeightMap r, HeightMap g, HeightMap b)
    : rgb({r, g, b}), shape(r.shape)
{
}

void HeightMapRGB::set_sto(Vec2<int> new_shape,
                           Vec2<int> new_tiling,
                           float     new_overlap)
{
  this->shape = new_shape;
  this->rgb.resize(3);
  for (auto &channel : rgb)
    channel.set_sto(new_shape, new_tiling, new_overlap);
}

void HeightMapRGB::colorize(HeightMap &h,
                            float      vmin,
                            float      vmax,
                            int        cmap,
                            bool       reverse)
{
  std::vector<std::vector<float>> colors = get_colormap_data(cmap);

  if (reverse)
    std::swap(vmin, vmax);

  // write colorize function for each tile
  auto lambda = [&vmin, &vmax, &colors](Array &in, Array &out, int channel)
  {
    int         nc = (int)colors.size();
    Vec2<float> a = in.normalization_coeff(vmin, vmax);
    a.x *= (nc - 1);
    a.y *= (nc - 1);

    for (int i = 0; i < in.shape.x; i++)
      for (int j = 0; j < in.shape.y; j++)
      {
        float v = std::clamp(a.x * in(i, j) + a.y, 0.f, (float)nc - 1.f);
        int   k = (int)v;
        float t = v - k;

        if (k < nc - 1)
          out(i, j) = (1.f - t) * colors[k][channel] +
                      t * colors[k + 1][channel];
        else
          out(i, j) = colors[k][channel];
      }
  };

  // apply to the each rgb heightmaps
  for (size_t kc = 0; kc < this->rgb.size(); kc++)
  {
    size_t                         nthreads = this->rgb[kc].get_ntiles();
    std::vector<std::future<void>> futures(nthreads);

    for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
      futures[i] = std::async(lambda,
                              std::ref(h.tiles[i]),
                              std::ref(this->rgb[kc].tiles[i]),
                              kc);

    for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
      futures[i].get();
  }
}

void HeightMapRGB::normalize()
{
  float min = std::min(std::min(this->rgb[0].min(), this->rgb[1].min()),
                       this->rgb[2].min());
  float max = std::max(std::max(this->rgb[0].max(), this->rgb[1].max()),
                       this->rgb[2].max());

  for (auto &channel : rgb)
    channel.remap(0.f, 1.f, min, max);
}

void HeightMapRGB::to_png_16bit(std::string fname)
{
  std::vector<uint16_t> img(this->shape.x * this->shape.y * 3);

  this->normalize();

  Array r_array = this->rgb[0].to_array();
  Array g_array = this->rgb[1].to_array();
  Array b_array = this->rgb[2].to_array();

  int k = 0;
  for (int j = this->shape.y - 1; j > -1; j -= 1)
    for (int i = 0; i < this->shape.x; i++)
    {
      img[k++] = (uint16_t)(65535.f * r_array(i, j));
      img[k++] = (uint16_t)(65535.f * g_array(i, j));
      img[k++] = (uint16_t)(65535.f * b_array(i, j));
    }
  write_png_rgb_16bit(fname, img, this->shape);
}

// FRIEND

HeightMapRGB mix_heightmap_rgb(HeightMapRGB &rgb1,
                               HeightMapRGB &rgb2,
                               HeightMap    &t)
{
  HeightMapRGB rgb_out;
  rgb_out.set_sto(rgb1.rgb[0].shape, rgb1.rgb[0].tiling, rgb1.rgb[0].overlap);

  // mixing function
  auto lambda = [](Array &out, Array &in1, Array &in2, Array &t)
  { out = lerp(in1, in2, t); };

  // apply to the each rgb heightmaps
  for (size_t kc = 0; kc < rgb1.rgb.size(); kc++)
  {
    size_t                         nthreads = rgb1.rgb[kc].get_ntiles();
    std::vector<std::future<void>> futures(nthreads);

    for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
      futures[i] = std::async(lambda,
                              std::ref(rgb_out.rgb[kc].tiles[i]),
                              std::ref(rgb1.rgb[kc].tiles[i]),
                              std::ref(rgb2.rgb[kc].tiles[i]),
                              std::ref(t.tiles[i]));

    for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
      futures[i].get();
  }

  return rgb_out;
}

} // namespace hmap
