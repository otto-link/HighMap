/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>
#include <future>
#include <thread>

#include "macrologger.h"

#include "highmap/colorize.hpp"
#include "highmap/colormaps.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/math.hpp"
#include "highmap/operator.hpp"
#include "highmap/tensor.hpp"

namespace hmap
{

HeightmapRGB::HeightmapRGB()
{
  this->rgb.resize(3);
}

HeightmapRGB::HeightmapRGB(Heightmap r, Heightmap g, Heightmap b)
    : rgb({r, g, b}), shape(r.shape)
{
}

void HeightmapRGB::set_sto(Vec2<int> new_shape,
                           Vec2<int> new_tiling,
                           float     new_overlap)
{
  this->shape = new_shape;
  this->rgb.resize(3);
  for (auto &channel : rgb)
    channel.set_sto(new_shape, new_tiling, new_overlap);
}

void HeightmapRGB::colorize(Heightmap                      &h,
                            float                           vmin,
                            float                           vmax,
                            std::vector<std::vector<float>> colormap_colors,
                            bool                            reverse)
{
  if (reverse) std::swap(vmin, vmax);

  // write colorize function for each tile
  auto lambda =
      [&vmin, &vmax, &colormap_colors](Array &in, Array &out, int channel)
  {
    int         nc = (int)colormap_colors.size();
    Vec2<float> a = in.normalization_coeff(vmin, vmax);
    a.x *= (nc - 1);
    a.y *= (nc - 1);

    for (int j = 0; j < in.shape.y; j++)
      for (int i = 0; i < in.shape.x; i++)
      {
        float v = std::clamp(a.x * in(i, j) + a.y, 0.f, (float)nc - 1.f);
        int   k = (int)v;
        float t = v - k;

        if (k < nc - 1)
          out(i, j) = (1.f - t) * colormap_colors[k][channel] +
                      t * colormap_colors[k + 1][channel];
        else
          out(i, j) = colormap_colors[k][channel];
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

void HeightmapRGB::colorize(Heightmap &h,
                            float      vmin,
                            float      vmax,
                            int        cmap,
                            bool       reverse)
{
  std::vector<std::vector<float>> colors = get_colormap_data(cmap);
  this->colorize(h, vmin, vmax, colors, reverse);
}

void HeightmapRGB::normalize()
{
  float min = std::min(std::min(this->rgb[0].min(), this->rgb[1].min()),
                       this->rgb[2].min());
  float max = std::max(std::max(this->rgb[0].max(), this->rgb[1].max()),
                       this->rgb[2].max());

  for (auto &channel : this->rgb)
    channel.remap(0.f, 1.f, min, max);
}

std::vector<uint8_t> HeightmapRGB::to_img_8bit(Vec2<int> shape_img)
{
  if (shape_img.x * shape_img.y == 0) shape_img = this->shape;

  std::vector<uint8_t> img(shape_img.x * shape_img.y * 3);

  Array r_array = this->rgb[0].to_array(shape_img);
  Array g_array = this->rgb[1].to_array(shape_img);
  Array b_array = this->rgb[2].to_array(shape_img);

  int k = 0;
  for (int j = shape_img.y - 1; j > -1; j -= 1)
    for (int i = 0; i < shape_img.x; i++)
    {
      img[k++] = (uint8_t)(255.f * r_array(i, j));
      img[k++] = (uint8_t)(255.f * g_array(i, j));
      img[k++] = (uint8_t)(255.f * b_array(i, j));
    }

  return img;
}

void HeightmapRGB::to_png(const std::string &fname, int depth)
{
  Tensor col3 = Tensor(this->shape, 3);
  for (int ch = 0; ch < col3.shape.z; ch++)
    col3.set_slice(ch, this->rgb[ch].to_array());
  col3.to_png(fname, depth);
}

// FRIEND

HeightmapRGB mix_heightmap_rgb(HeightmapRGB &rgb1,
                               HeightmapRGB &rgb2,
                               Heightmap    &t)
{
  HeightmapRGB rgb_out;
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

HeightmapRGB mix_heightmap_rgb(HeightmapRGB &rgb1, HeightmapRGB &rgb2, float t)
{
  HeightmapRGB rgb_out;
  rgb_out.set_sto(rgb1.rgb[0].shape, rgb1.rgb[0].tiling, rgb1.rgb[0].overlap);

  // mixing function
  auto lambda = [&t](Array &out, Array &in1, Array &in2)
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
                              std::ref(rgb2.rgb[kc].tiles[i]));

    for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
      futures[i].get();
  }

  return rgb_out;
}

HeightmapRGB mix_heightmap_rgb_sqrt(HeightmapRGB &rgb1,
                                    HeightmapRGB &rgb2,
                                    Heightmap    &t)
{
  HeightmapRGB rgb_out;
  rgb_out.set_sto(rgb1.rgb[0].shape, rgb1.rgb[0].tiling, rgb1.rgb[0].overlap);

  // mixing function
  auto lambda = [](Array &out, Array &in1, Array &in2, Array &t)
  { out = pow((1.f - t) * in1 * in1 + t * in2 * in2, 0.5f); };

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

HeightmapRGB mix_heightmap_rgb_sqrt(HeightmapRGB &rgb1,
                                    HeightmapRGB &rgb2,
                                    float         t)
{
  HeightmapRGB rgb_out;
  rgb_out.set_sto(rgb1.rgb[0].shape, rgb1.rgb[0].tiling, rgb1.rgb[0].overlap);

  // mixing function
  auto lambda = [&t](Array &out, Array &in1, Array &in2)
  { out = pow((1.f - t) * in1 * in1 + t * in2 * in2, 0.5f); };

  // apply to the each rgb heightmaps
  for (size_t kc = 0; kc < rgb1.rgb.size(); kc++)
  {
    size_t                         nthreads = rgb1.rgb[kc].get_ntiles();
    std::vector<std::future<void>> futures(nthreads);

    for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
      futures[i] = std::async(lambda,
                              std::ref(rgb_out.rgb[kc].tiles[i]),
                              std::ref(rgb1.rgb[kc].tiles[i]),
                              std::ref(rgb2.rgb[kc].tiles[i]));

    for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
      futures[i].get();
  }

  return rgb_out;
}

} // namespace hmap
