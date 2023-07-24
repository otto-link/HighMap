#include <cmath>
#include <stdint.h>
#include <vector>

// #define STB_IMAGE_WRITE_IMPLEMENTATION
#include "macrologger.h"
#include "stb_image_write.h"

#include "highmap/array.hpp"
#include "highmap/colormaps.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

void apply_hillshade(std::vector<uint8_t> &img, const hmap::Array &array)
{
  Array hs = constant(array.shape, 1.f);
  hs =
      hillshade(array, 180.f, 45.f, 10.f * array.ptp() / (float)array.shape[1]);
  remap(hs);

  int k = 0;
  for (int j = array.shape[1] - 1; j > -1; j--)
    for (int i = 0; i < array.shape[0]; i++)
    {
      img[k] = (uint8_t)((float)img[k] * hs(i, j));
      img[k + 1] = (uint8_t)((float)img[k + 1] * hs(i, j));
      img[k + 2] = (uint8_t)((float)img[k + 2] * hs(i, j));
      k += 3;
    }
}

std::vector<uint8_t> colorize(const hmap::Array &array,
                              float              vmin,
                              float              vmax,
                              int                cmap,
                              bool               hillshading)
{
  // define colormap
  std::vector<uint32_t> colors_data = {};

  switch (cmap)
  {
  case cmap::blues:
  case -cmap::blues:
  {
    colors_data = CMAP_BLUES;
  }
  break;

  case cmap::bone:
  case -cmap::bone:
  {
    colors_data = CMAP_BONE;
  }
  break;

  case cmap::gray:
  case -cmap::gray:
  {
    colors_data = CMAP_GRAY;
  }
  break;

  case cmap::hot:
  case -cmap::hot:
  {
    colors_data = CMAP_HOT;
  }
  break;

  case cmap::inferno:
  case -cmap::inferno:
  {
    colors_data = CMAP_INFERNO;
  }
  break;

  case cmap::jet:
  case -cmap::jet:
  {
    colors_data = CMAP_JET;
  }
  break;

  case cmap::magma:
  case -cmap::magma:
  {
    colors_data = CMAP_MAGMA;
  }
  break;

  case cmap::nipy_spectral:
  case -cmap::nipy_spectral:
  {
    colors_data = CMAP_NIPY_SPECTRAL;
  }
  break;

  case cmap::seismic:
  case -cmap::seismic:
  {
    colors_data = CMAP_SEISMIC;
  }
  break;

  case cmap::terrain:
  case -cmap::terrain:
  {
    colors_data = CMAP_TERRAIN;
  }
  break;

  case cmap::viridis:
  case -cmap::viridis:
  {
    colors_data = CMAP_VIRIDIS;
  }
  break;
  }

  Clut1D clut = Clut1D({CMAP_SIZE}, colors_data);

  // create image
  std::vector<uint8_t> img(IMG_CHANNELS * array.shape[0] * array.shape[1]);

  // normalization factors
  float a = 0.f;
  float b = 0.f;
  if (vmin != vmax)
  {
    if (cmap > 0)
    {
      a = 1.f / (vmax - vmin);
      b = -vmin / (vmax - vmin);
    }
    else
    {
      a = 1.f / (vmin - vmax);
      b = -vmax / (vmin - vmax);
    }
  }

  // reorganize things to get an image with (i, j) used as (x, y)
  // coordinates, i.e. with (0, 0) at the bottom left
  int k = 0;

  for (int j = array.shape[1] - 1; j > -1; j--)
    for (int i = 0; i < array.shape[0]; i++)
    {
      float v = std::clamp(a * array(i, j) + b, 0.f, 1.f);

      // linear interpolation
      float u;
      int   p = clut.get_index(v, 0, u);

      UNPACK_COLOR(clut.value_at(p), uint8_t r0, uint8_t g0, uint8_t b0);

      if (p < clut.shape[0] - 1)
      {
        UNPACK_COLOR(clut.value_at(p + 1), uint8_t r1, uint8_t g1, uint8_t b1);

        img[k++] = (1.f - u) * r0 + u * r1;
        img[k++] = (1.f - u) * g0 + u * g1;
        img[k++] = (1.f - u) * b0 + u * b1;
      }
      else
      {
        img[k++] = r0;
        img[k++] = g0;
        img[k++] = b0;
      }
    }

  // add hillshading
  if (hillshading)
    apply_hillshade(img, array);

  return img;
}

std::vector<uint8_t> colorize_grayscale(const Array     &array,
                                        std::vector<int> step)
{
  // create image
  std::vector<uint8_t> img(array.shape[0] * array.shape[1]);

  // normalization factors
  float a = 0.f;
  float b = 0.f;
  float vmin = array.min();
  float vmax = array.max();

  if (vmin != vmax)
  {
    a = 1.f / (vmax - vmin);
    b = -vmin / (vmax - vmin);
  }

  int k = 0;

  for (int j = array.shape[1] - 1; j > -1; j -= step[1])
    for (int i = 0; i < array.shape[0]; i += step[0])
    {
      float v = a * array(i, j) + b;
      img[k++] = (uint8_t)(v * 255.f);
    }

  return img;
}

std::vector<uint8_t> colorize_trivariate(const Array &c0,
                                         const Array &c1,
                                         const Array &c2,
                                         Clut3D      &clut,
                                         bool         hillshading)
{
  std::vector<uint8_t> img(IMG_CHANNELS * c0.shape[0] * c0.shape[1]);

  // TODO vlim and clamping

  // normalization coefficients for each array
  float cmin = c0.min();
  float cmax = c0.max();
  float ac0 = 1.f / (cmax - cmin);
  float bc0 = -cmin / (cmax - cmin);

  cmin = c1.min();
  cmax = c1.max();
  float ac1 = 1.f / (cmax - cmin);
  float bc1 = -cmin / (cmax - cmin);

  cmin = c2.min();
  cmax = c2.max();
  float ac2 = 1.f / (cmax - cmin);
  float bc2 = -cmin / (cmax - cmin);

  int k = 0;

  for (int j = c0.shape[1] - 1; j > -1; j--)
    for (int i = 0; i < c0.shape[0]; i++)
    {
      // linear interpolation for the first criterion (generally
      // dominant)
      float u;
      int   p1 = clut.get_index(ac0 * c0(i, j) + bc0, 0, u);
      int   p2 = clut.get_index(ac1 * c1(i, j) + bc1, 1);
      int   p3 = clut.get_index(ac2 * c2(i, j) + bc2, 2);

      UNPACK_COLOR(clut.value_at(p1, p2, p3),
                   uint8_t r0,
                   uint8_t g0,
                   uint8_t b0);

      if (p1 < clut.shape[0] - 1)
      {
        UNPACK_COLOR(clut.value_at(p1 + 1, p2, p3),
                     uint8_t r1,
                     uint8_t g1,
                     uint8_t b1);

        img[k++] = (1.f - u) * r0 + u * r1;
        img[k++] = (1.f - u) * g0 + u * g1;
        img[k++] = (1.f - u) * b0 + u * b1;
      }
      else
      {
        img[k++] = r0;
        img[k++] = g0;
        img[k++] = b0;
      }
    }

  // blur colors to smooth nearest interpolation
  {
    std::vector<uint8_t> delta(IMG_CHANNELS * c0.shape[0] * c0.shape[1]);

    for (int i = 1; i < c0.shape[0] - 1; i++)
      for (int j = 1; j < c0.shape[1] - 1; j++)
      {
        int k = j * IMG_CHANNELS + i * IMG_CHANNELS * c0.shape[1];
        int kn = (j + 1) * IMG_CHANNELS +
                 i * IMG_CHANNELS * c0.shape[1]; // north
        int ks = (j - 1) * IMG_CHANNELS +
                 i * IMG_CHANNELS * c0.shape[1]; // south
        int ke = j * IMG_CHANNELS +
                 (i - 1) * IMG_CHANNELS * c0.shape[1]; // east
        int kw = j * IMG_CHANNELS +
                 (i + 1) * IMG_CHANNELS * c0.shape[1]; // west

        for (int r = 0; r < IMG_CHANNELS; r++)
          delta[k + r] = (uint8_t)(0.25f * (img[kn + r] + img[ks + r] +
                                            img[ke + r] + img[kw + r])) -
                         img[k + r];
      }

    std::transform(img.begin(),
                   img.end(),
                   delta.begin(),
                   img.begin(),
                   [](uint8_t i, uint8_t d) { return i + d; });
  }

  // add hillshading
  if (hillshading)
    apply_hillshade(img, c0);

  return img;
}

void write_png_8bit(std::string           fname,
                    std::vector<uint8_t> &img,
                    std::vector<int>      shape)
{
  // row and column are permutted
  stbi_write_png(fname.c_str(),
                 shape[0],
                 shape[1],
                 IMG_CHANNELS,
                 img.data(),
                 IMG_CHANNELS * shape[0]);
}

} // namespace hmap
