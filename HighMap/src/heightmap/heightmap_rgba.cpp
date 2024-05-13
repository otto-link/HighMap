/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>
#include <future>
#include <thread>

#include "libInterpolate/Interpolate.hpp"
#include "macrologger.h"

#include "highmap/colormaps.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/io.hpp"
#include "highmap/math.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

HeightMapRGBA::HeightMapRGBA()
{
  this->rgba.resize(4);
}

HeightMapRGBA::HeightMapRGBA(HeightMap r, HeightMap g, HeightMap b, HeightMap a)
    : rgba({r, g, b, a}), shape(r.shape)
{
}

void HeightMapRGBA::set_alpha(HeightMap new_alpha)
{
  this->rgba[3] = new_alpha;
}

void HeightMapRGBA::set_alpha(float new_alpha)
{
  this->rgba[3] = HeightMap(this->rgba[3].shape,
                            this->rgba[3].tiling,
                            this->rgba[3].overlap,
                            new_alpha);
}

void HeightMapRGBA::set_sto(Vec2<int> new_shape,
                            Vec2<int> new_tiling,
                            float     new_overlap)
{
  this->shape = new_shape;
  this->rgba.resize(4);
  for (auto &channel : rgba)
    channel.set_sto(new_shape, new_tiling, new_overlap);
}

void HeightMapRGBA::colorize(HeightMap                      &color_level,
                             float                           vmin,
                             float                           vmax,
                             std::vector<std::vector<float>> colormap_colors,
                             HeightMap                      *p_alpha,
                             bool                            reverse)
{
  if (reverse)
    std::swap(vmin, vmax);

  // write colorize function for each tile
  auto lambda =
      [&vmin, &vmax, &colormap_colors](Array &in, Array &out, int channel)
  {
    int         nc = (int)colormap_colors.size();
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
          out(i, j) = (1.f - t) * colormap_colors[k][channel] +
                      t * colormap_colors[k + 1][channel];
        else
          out(i, j) = colormap_colors[k][channel];
      }
  };

  // apply to the each rgb heightmaps (but not the alpha channel)
  for (size_t kc = 0; kc < 3; kc++)
  {
    size_t                         nthreads = this->rgba[kc].get_ntiles();
    std::vector<std::future<void>> futures(nthreads);

    for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
      futures[i] = std::async(lambda,
                              std::ref(color_level.tiles[i]),
                              std::ref(this->rgba[kc].tiles[i]),
                              kc);

    for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
      futures[i].get();
  }

  // alpha channel
  if (p_alpha)
    this->rgba[3] = *p_alpha;
  else
    transform(this->rgba[3], [](Array &x) { x = 1.f; });
}

void HeightMapRGBA::colorize(HeightMap &color_level,
                             float      vmin,
                             float      vmax,
                             int        cmap,
                             HeightMap *p_alpha,
                             bool       reverse)
{
  std::vector<std::vector<float>> colors = get_colormap_data(cmap);
  this->colorize(color_level, vmin, vmax, colors, p_alpha, reverse);
}

HeightMapRGBA mix_heightmap_rgba(HeightMapRGBA &rgba1,
                                 HeightMapRGBA &rgba2,
                                 bool           use_sqrt_avg)
{
  HeightMapRGBA rgba_out;
  rgba_out.set_sto(rgba1.rgba[0].shape,
                   rgba1.rgba[0].tiling,
                   rgba1.rgba[0].overlap);

  // mixing function
  std::function<void(Array &, Array &, Array &, Array &)> lambda;

  if (use_sqrt_avg)
    lambda = [](Array &out, Array &in1, Array &in2, Array &t)
    { out = pow((1.f - t) * in1 * in1 + t * in2 * in2, 0.5f); };
  else
    lambda = [](Array &out, Array &in1, Array &in2, Array &t)
    { out = lerp(in1, in2, t); };

  // apply to the each rgb heightmaps, using alpha as a mixing weight
  // between the layers

  // using "over" algo
  HeightMap t;
  t.set_sto(rgba1.rgba[0].shape, rgba1.rgba[0].tiling, rgba1.rgba[0].overlap);

  transform(t,
            rgba1.rgba[3],
            rgba2.rgba[3],
            [](Array &out, Array &a1, Array &a2)
            { out = a2 / (a2 + a1 * (1.f - a2)); });

  // apply mixing
  for (size_t kc = 0; kc < 3; kc++)
  {
    size_t                         nthreads = rgba1.rgba[kc].get_ntiles();
    std::vector<std::future<void>> futures(nthreads);

    for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
      futures[i] = std::async(lambda,
                              std::ref(rgba_out.rgba[kc].tiles[i]),
                              std::ref(rgba1.rgba[kc].tiles[i]),
                              std::ref(rgba2.rgba[kc].tiles[i]),
                              std::ref(t.tiles[i]));

    for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
      futures[i].get();
  }

  // alpha channel
  transform(t,
            rgba1.rgba[3],
            rgba2.rgba[3],
            [](Array &out, Array &a1, Array &a2)
            { out = a1 + a2 * (1.f - a1); });

  rgba_out.rgba[3] = t;

  return rgba_out;
}

HeightMapRGBA mix_heightmap_rgba(std::vector<HeightMapRGBA *> rgba_plist,
                                 bool                         use_sqrt_avg)
{
  HeightMapRGBA rgba_out;

  // overlay the RGBA "2 by 2"
  if (rgba_plist.size() == 0)
    throw std::runtime_error("empty RGBA list");
  else if (rgba_plist.size() == 1)
    rgba_out = *rgba_plist[0];
  else
  {
    rgba_out = *rgba_plist[0];
    for (size_t k = 1; k < rgba_plist.size(); k++)
      rgba_out = mix_heightmap_rgba(rgba_out, *rgba_plist[k], use_sqrt_avg);
  }

  return rgba_out;
}

void HeightMapRGBA::normalize()
{
  float min = std::min(std::min(this->rgba[0].min(), this->rgba[1].min()),
                       this->rgba[2].min());
  float max = std::max(std::max(this->rgba[0].max(), this->rgba[1].max()),
                       this->rgba[2].max());

  for (size_t k = 0; k < 4; k++)
    if (k == 3)
      this->rgba[k].remap(0.f, 1.f); // alpha
    else
      this->rgba[k].remap(0.f, 1.f, min, max); // RGB
}

void HeightMapRGBA::to_png_8bit(std::string fname)
{
  std::vector<uint8_t> img(this->shape.x * this->shape.y * 4);

  Array r_array = this->rgba[0].to_array();
  Array g_array = this->rgba[1].to_array();
  Array b_array = this->rgba[2].to_array();
  Array a_array = this->rgba[3].to_array();

  int k = 0;
  for (int j = this->shape.y - 1; j > -1; j -= 1)
    for (int i = 0; i < this->shape.x; i++)
    {
      img[k++] = (uint8_t)(255.f * r_array(i, j));
      img[k++] = (uint8_t)(255.f * g_array(i, j));
      img[k++] = (uint8_t)(255.f * b_array(i, j));
      img[k++] = (uint8_t)(255.f * a_array(i, j));
    }
  write_png_rgba_8bit(fname, img, this->shape);
}

void HeightMapRGBA::to_png_16bit(std::string fname)
{
  std::vector<uint16_t> img(this->shape.x * this->shape.y * 4);

  Array r_array = this->rgba[0].to_array();
  Array g_array = this->rgba[1].to_array();
  Array b_array = this->rgba[2].to_array();
  Array a_array = this->rgba[3].to_array();

  int k = 0;
  for (int j = this->shape.y - 1; j > -1; j -= 1)
    for (int i = 0; i < this->shape.x; i++)
    {
      img[k++] = (uint16_t)(65535.f * r_array(i, j));
      img[k++] = (uint16_t)(65535.f * g_array(i, j));
      img[k++] = (uint16_t)(65535.f * b_array(i, j));
      img[k++] = (uint16_t)(65535.f * a_array(i, j));
    }
  write_png_rgba_16bit(fname, img, this->shape);
}

std::vector<uint8_t> HeightMapRGBA::to_img_8bit(Vec2<int> shape_img)
{
  if (shape_img.x * shape_img.y == 0)
    shape_img = this->shape;

  std::vector<uint8_t> img(shape_img.x * shape_img.y * 4);

  Array r_array = this->rgba[0].to_array(shape_img);
  Array g_array = this->rgba[1].to_array(shape_img);
  Array b_array = this->rgba[2].to_array(shape_img);
  Array a_array = this->rgba[3].to_array(shape_img);

  int k = 0;
  for (int j = shape_img.y - 1; j > -1; j -= 1)
    for (int i = 0; i < shape_img.x; i++)
    {
      img[k++] = (uint8_t)(255.f * r_array(i, j));
      img[k++] = (uint8_t)(255.f * g_array(i, j));
      img[k++] = (uint8_t)(255.f * b_array(i, j));
      img[k++] = (uint8_t)(255.f * a_array(i, j));
    }

  return img;
}

} // namespace hmap
