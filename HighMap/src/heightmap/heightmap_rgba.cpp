/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>
#include <future>
#include <thread>

#include "macrologger.h"

#include "highmap/colormaps.hpp"
#include "highmap/export.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/math.hpp"
#include "highmap/operator.hpp"
#include "highmap/primitives.hpp"
#include "highmap/tensor.hpp"

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

HeightMapRGBA::HeightMapRGBA(Vec2<int> shape,
                             Vec2<int> tiling,
                             float     overlap,
                             Array     array_r,
                             Array     array_g,
                             Array     array_b,
                             Array     array_a)
{
  this->set_sto(shape, tiling, overlap);
  this->rgba[0].from_array_interp_nearest(array_r);
  this->rgba[1].from_array_interp_nearest(array_g);
  this->rgba[2].from_array_interp_nearest(array_b);
  this->rgba[3].from_array_interp_nearest(array_a);
}

HeightMapRGBA::HeightMapRGBA(Vec2<int> shape, Vec2<int> tiling, float overlap)
{
  this->set_sto(shape, tiling, overlap);
}

HeightMap HeightMapRGBA::luminance()
{
  // https://stackoverflow.com/questions/596216
  HeightMap out = HeightMap(this->rgba[0].shape,
                            this->rgba[0].tiling,
                            this->rgba[0].overlap);

  transform(out,
            this->rgba[0],
            this->rgba[1],
            this->rgba[2],
            [](Array &y, Array &r, Array &g, Array &b)
            { y = 0.299f * r + 0.587f * g + 0.114f * b; });

  return out;
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
                             bool                            reverse,
                             HeightMap                      *p_noise)
{
  if (reverse) std::swap(vmin, vmax);

  // write colorize function for each tile
  auto lambda = [&vmin, &vmax, &colormap_colors](Array &in,
                                                 Array &out,
                                                 Array *p_noise_array,
                                                 int    channel)
  {
    int         nc = (int)colormap_colors.size();
    Vec2<float> a = in.normalization_coeff(vmin, vmax);
    a.x *= (nc - 1);
    a.y *= (nc - 1);

    if (p_noise_array)
    {
      for (int i = 0; i < in.shape.x; i++)
        for (int j = 0; j < in.shape.y; j++)
        {
          float v = std::clamp(a.x * (in(i, j) + (*p_noise_array)(i, j)) + a.y,
                               0.f,
                               (float)nc - 1.f);
          int   k = (int)v;
          float t = v - k;

          if (k < nc - 1)
            out(i, j) = (1.f - t) * colormap_colors[k][channel] +
                        t * colormap_colors[k + 1][channel];
          else
            out(i, j) = colormap_colors[k][channel];
        }
    }
    else
    {
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
    }
  };

  // apply to the each rgb heightmaps (but not the alpha channel)
  for (size_t kc = 0; kc < 3; kc++)
  {
    size_t                         nthreads = this->rgba[kc].get_ntiles();
    std::vector<std::future<void>> futures(nthreads);

    for (decltype(futures)::size_type i = 0; i < nthreads; ++i)
    {
      Array *p_n = (p_noise == nullptr) ? nullptr : &p_noise->tiles[i];

      futures[i] = std::async(lambda,
                              std::ref(color_level.tiles[i]),
                              std::ref(this->rgba[kc].tiles[i]),
                              p_n,
                              kc);
    }

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
                             bool       reverse,
                             HeightMap *p_noise)
{
  std::vector<std::vector<float>> colors = get_colormap_data(cmap);
  this->colorize(color_level, vmin, vmax, colors, p_alpha, reverse, p_noise);
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

HeightMapRGBA mix_normal_map_rgba(HeightMapRGBA          &nmap_base,
                                  HeightMapRGBA          &nmap_detail,
                                  float                   detail_scaling,
                                  NormalMapBlendingMethod blending_method)
{
  // mix two RGBA arrays assuming they represent normal maps

  // https://blog.selfshadow.com/publications/blending-in-detail/

  // https://j3l7h.de/talks/2008-02-18_Care_and_Feeding_of_Normal_Vectors.pdf

  // output, also used to store first normal map
  HeightMapRGBA nmap_out = nmap_base;

  // mix and then re-normalize values assuming a RGB channels
  // represent a normal vector
  auto lambda = [&detail_scaling, &blending_method](Array &r1,
                                                    Array &g1,
                                                    Array &b1,
                                                    Array &r2,
                                                    Array &g2,
                                                    Array &b2)
  {
    std::function<Vec3<float>(Vec3<float> &, Vec3<float> &)> blending_fct;

    switch (blending_method)
    {
    case NormalMapBlendingMethod::NMAP_LINEAR:
    {
      blending_fct = [](Vec3<float> &n1, Vec3<float> &n2) { return n1 + n2; };
    }
    break;
    //
    case NormalMapBlendingMethod::NMAP_DERIVATIVE:
    {
      blending_fct = [](Vec3<float> &n1, Vec3<float> &n2)
      {
        Vec3<float> vn = Vec3<float>(n1.x * n2.z + n2.x * n1.z,
                                     n1.y * n2.z + n2.y * n1.z,
                                     n1.z * n2.z);
        return vn;
      };
    }
    break;
    //
    case NormalMapBlendingMethod::NMAP_UDN:
    {
      blending_fct = [](Vec3<float> &n1, Vec3<float> &n2)
      {
        Vec3<float> vn = Vec3<float>(n1.x + n2.x, n1.y + n2.y, n1.z);
        return vn;
      };
    }
    break;
      //
    case NormalMapBlendingMethod::NMAP_UNITY:
    {
      blending_fct = [](Vec3<float> &n1, Vec3<float> &n2)
      {
        Vec3<float> m0 = Vec3<float>(n1.z, n1.x, -n1.x);
        Vec3<float> m1 = Vec3<float>(n1.x, n1.z, -n1.y);
        Vec3<float> m2 = Vec3<float>(n1.x, n1.y, n1.z);

        Vec3<float> vn = Vec3<float>(n2.x * m0.x + n2.y * m1.x + n2.z * m2.x,
                                     n2.x * m0.y + n2.y * m1.y + n2.z * m2.y,
                                     n2.x * m0.z + n2.y * m1.z + n2.z * m2.z);
        return vn;
      };
    }
    break;
    //
    case NormalMapBlendingMethod::NMAP_WHITEOUT:
    default:
    {
      blending_fct = [](Vec3<float> &n1, Vec3<float> &n2)
      {
        Vec3<float> vn = Vec3<float>(n1.x + n2.x, n1.y + n2.y, n1.z * n2.z);
        return vn;
      };
    }
    }

    for (int i = 0; i < r1.shape.x; i++)
      for (int j = 0; j < r1.shape.y; j++)
      {
        // do some rescaling because RGBA texture expected in [0, 1]
        // but normal vector expected in [-1, 1]

        Vec3<float> v111 = Vec3<float>(1.f, 1.f, 1.f);
        Vec3<float> n1 = 2.f * Vec3<float>(r1(i, j), g1(i, j), b1(i, j)) - v111;
        Vec3<float> n2 = 2.f * Vec3<float>(r2(i, j), g2(i, j), b2(i, j)) - v111;

        n2.x *= detail_scaling;
        n2.y *= detail_scaling;
        n2.z *= detail_scaling;

        Vec3<float> vn = blending_fct(n1, n2);
        vn.normalize();

        r1(i, j) = 0.5f * vn.x + 0.5f;
        g1(i, j) = 0.5f * vn.y + 0.5f;
        b1(i, j) = 0.5f * vn.z + 0.5f;
      }
  };

  // apply...
  transform(nmap_out.rgba[0],
            nmap_out.rgba[1],
            nmap_out.rgba[2],
            nmap_detail.rgba[0],
            nmap_detail.rgba[1],
            nmap_detail.rgba[2],
            lambda);

  return nmap_out;
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

void HeightMapRGBA::to_png(const std::string &fname, int depth)
{
  Tensor col3 = Tensor(this->shape, 4);
  for (int ch = 0; ch < col3.shape.z; ch++)
    col3.set_slice(ch, this->rgba[ch].to_array());
  col3.to_png(fname, depth);
}

std::vector<uint8_t> HeightMapRGBA::to_img_8bit(Vec2<int> shape_img)
{
  if (shape_img.x == 0 || shape_img.y == 0) shape_img = this->shape;

  std::vector<uint8_t> img(shape_img.x * shape_img.y * 4);
  Array                r_array = this->rgba[0].to_array(shape_img);
  Array                g_array = this->rgba[1].to_array(shape_img);
  Array                b_array = this->rgba[2].to_array(shape_img);
  Array                a_array = this->rgba[3].to_array(shape_img);

  int index = 0;
  for (int j = shape_img.y - 1; j >= 0; j--)
    for (int i = 0; i < shape_img.x; i++)
    {
      img[index++] = static_cast<uint8_t>(r_array(i, j) * 255.f);
      img[index++] = static_cast<uint8_t>(g_array(i, j) * 255.f);
      img[index++] = static_cast<uint8_t>(b_array(i, j) * 255.f);
      img[index++] = static_cast<uint8_t>(a_array(i, j) * 255.f);
    }

  return img;
}

} // namespace hmap
