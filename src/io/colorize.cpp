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

std::vector<uint8_t> colorize(hmap::Array &array,
                              float        vmin,
                              float        vmax,
                              int          cmap,
                              bool         hillshading)
{
  std::vector<uint8_t> data(IMG_CHANNELS * array.shape[0] * array.shape[1]);

  std::vector<float>              color_bounds;
  std::vector<std::vector<float>> colors;

  // -- define colormap

  switch (cmap)
  {

  case cmap::bone:
  case -cmap::bone:
  {
    std::vector<std::vector<float>> c = {
        {0.000f, 0.000f, 0.000f},
        {0.080f, 0.080f, 0.111f},
        {0.159f, 0.159f, 0.221f},
        {0.239f, 0.239f, 0.332f},
        {0.318f, 0.318f, 0.443f},
        {0.398f, 0.427f, 0.523f},
        {0.477f, 0.536f, 0.602f},
        {0.557f, 0.646f, 0.682f},
        {0.636f, 0.755f, 0.761f},
        {0.751f, 0.841f, 0.841f},
        {0.876f, 0.920f, 0.920f},
        {1.000f, 1.000f, 1.000f},
    };
    colors = c;
  }
  break;

  case cmap::gray:
  case -cmap::gray:
  {
    std::vector<std::vector<float>> c = {
        {0.000f, 0.000f, 0.000f},
        {0.091f, 0.091f, 0.091f},
        {0.182f, 0.182f, 0.182f},
        {0.273f, 0.273f, 0.273f},
        {0.364f, 0.364f, 0.364f},
        {0.455f, 0.455f, 0.455f},
        {0.545f, 0.545f, 0.545f},
        {0.636f, 0.636f, 0.636f},
        {0.727f, 0.727f, 0.727f},
        {0.818f, 0.818f, 0.818f},
        {0.909f, 0.909f, 0.909f},
        {1.000f, 1.000f, 1.000f},
    };
    colors = c;
  }
  break;

  case cmap::hot:
  case -cmap::hot:
  {
    std::vector<std::vector<float>> c = {
        {0.042f, 0.000f, 0.000f},
        {0.280f, 0.000f, 0.000f},
        {0.519f, 0.000f, 0.000f},
        {0.758f, 0.000f, 0.000f},
        {0.996f, 0.000f, 0.000f},
        {1.000f, 0.235f, 0.000f},
        {1.000f, 0.473f, 0.000f},
        {1.000f, 0.712f, 0.000f},
        {1.000f, 0.951f, 0.000f},
        {1.000f, 1.000f, 0.284f},
        {1.000f, 1.000f, 0.642f},
        {1.000f, 1.000f, 1.000f},
    };
    colors = c;
  }
  break;

  case cmap::inferno:
  case -cmap::inferno:
  {
    std::vector<std::vector<float>> c = {
        {0.001f, 0.000f, 0.014f},
        {0.077f, 0.042f, 0.206f},
        {0.225f, 0.036f, 0.388f},
        {0.373f, 0.074f, 0.432f},
        {0.522f, 0.128f, 0.420f},
        {0.665f, 0.182f, 0.370f},
        {0.797f, 0.255f, 0.287f},
        {0.902f, 0.364f, 0.184f},
        {0.969f, 0.516f, 0.063f},
        {0.988f, 0.683f, 0.072f},
        {0.961f, 0.859f, 0.298f},
        {0.988f, 0.998f, 0.645f},
    };
    colors = c;
  }
  break;

  case cmap::jet:
  case -cmap::jet:
  {
    std::vector<std::vector<float>> c = {
        {0.000f, 0.000f, 0.500f},
        {0.000f, 0.000f, 0.913f},
        {0.000f, 0.227f, 1.000f},
        {0.000f, 0.591f, 1.000f},
        {0.044f, 0.955f, 0.924f},
        {0.337f, 1.000f, 0.630f},
        {0.630f, 1.000f, 0.337f},
        {0.924f, 1.000f, 0.044f},
        {1.000f, 0.677f, 0.000f},
        {1.000f, 0.340f, 0.000f},
        {0.913f, 0.003f, 0.000f},
        {0.500f, 0.000f, 0.000f},
    };
    colors = c;
  }
  break;

  case cmap::nipy_spectral:
  case -cmap::nipy_spectral:
  {
    std::vector<std::vector<float>> c = {
        {0.000f, 0.000f, 0.000f},
        {0.521f, 0.000f, 0.588f},
        {0.000f, 0.000f, 0.794f},
        {0.000f, 0.527f, 0.867f},
        {0.000f, 0.667f, 0.630f},
        {0.000f, 0.612f, 0.000f},
        {0.000f, 0.855f, 0.000f},
        {0.533f, 1.000f, 0.000f},
        {0.970f, 0.861f, 0.000f},
        {1.000f, 0.382f, 0.000f},
        {0.855f, 0.000f, 0.000f},
        {0.800f, 0.800f, 0.800f},
    };
    colors = c;
  }
  break;

  case cmap::seismic:
  case -cmap::seismic:
  {
    std::vector<std::vector<float>> c = {
        {0.000f, 0.000f, 0.300f},
        {0.000f, 0.000f, 0.555f},
        {0.000f, 0.000f, 0.809f},
        {0.091f, 0.091f, 1.000f},
        {0.455f, 0.455f, 1.000f},
        {0.818f, 0.818f, 1.000f},
        {1.000f, 0.818f, 0.818f},
        {1.000f, 0.455f, 0.455f},
        {1.000f, 0.091f, 0.091f},
        {0.864f, 0.000f, 0.000f},
        {0.682f, 0.000f, 0.000f},
        {0.500f, 0.000f, 0.000f},
    };
    colors = c;
  }
  break;

  case cmap::terrain:
  case -cmap::terrain:
  {
    std::vector<std::vector<float>> c = {
        {0.200f, 0.200f, 0.600f},
        {0.079f, 0.442f, 0.842f},
        {0.000f, 0.664f, 0.809f},
        {0.091f, 0.818f, 0.418f},
        {0.455f, 0.891f, 0.491f},
        {0.818f, 0.964f, 0.564f},
        {0.909f, 0.884f, 0.551f},
        {0.727f, 0.651f, 0.453f},
        {0.545f, 0.418f, 0.355f},
        {0.636f, 0.535f, 0.513f},
        {0.818f, 0.767f, 0.756f},
        {1.000f, 1.000f, 1.000f},
    };
    colors = c;
  }
  break;

  case cmap::viridis:
  case -cmap::viridis:
  {
    std::vector<std::vector<float>> c = {
        {0.267f, 0.005f, 0.329f},
        {0.283f, 0.131f, 0.449f},
        {0.262f, 0.242f, 0.521f},
        {0.220f, 0.343f, 0.549f},
        {0.177f, 0.438f, 0.558f},
        {0.143f, 0.523f, 0.556f},
        {0.120f, 0.607f, 0.540f},
        {0.166f, 0.691f, 0.497f},
        {0.320f, 0.771f, 0.411f},
        {0.526f, 0.833f, 0.288f},
        {0.762f, 0.876f, 0.137f},
        {0.993f, 0.906f, 0.144f},
    };
    colors = c;
  }
  break;
  }

  color_bounds = linspace(0.f, 1.f, (int)colors.size());

  // reverse colormap if requested
  if (cmap < 0)
    std::reverse(std::begin(colors), std::end(colors));

  // -- add hillshading (if requested)
  Array hs = constant(array.shape, 1.f);
  if (hillshading)
  {
    hs = hillshade(array,
                   180.f,
                   45.f,
                   10.f * array.ptp() / (float)array.shape[1]);
    remap(hs);
  }

  // -- define data

  const int nc = (int)color_bounds.size();
  int       k = 0;

  float a = 0.f;
  float b = 0.f;
  if (vmin != vmax)
  {
    a = 1.f / (vmax - vmin);
    b = -vmin / (vmax - vmin);
  }

  // reorganize things to get an image with (i, j) used as (x, y)
  // coordinates, i.e. with (0, 0) at the bottom left
  for (int j = array.shape[1] - 1; j > -1; j--)
    for (int i = 0; i < array.shape[0]; i++)
    {
      float              v = a * array(i, j) + b;
      std::vector<float> rgb(3);
      int                ic = (int)(v * (float)(nc - 1));

      if (ic == nc - 1)
      {
        rgb = colors[nc - 1];
      }
      else
      {
        float t = (v - color_bounds[ic]) /
                  (color_bounds[ic + 1] - color_bounds[ic]);
        for (int p = 0; p < 3; p++)
        {
          rgb[p] = (1.f - t) * colors[ic][p] + t * colors[ic + 1][p];
          rgb[p] *= hs(i, j); // hillshading
        }
      }

      data[k++] = (uint8_t)std::floor(255 * rgb[0]);
      data[k++] = (uint8_t)std::floor(255 * rgb[1]);
      data[k++] = (uint8_t)std::floor(255 * rgb[2]);
    }

  return data;
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
  if (true)
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

  // add hillshading (if requested)
  if (hillshading)
  {
    Array hs = constant(c0.shape, 1.f);
    hs = hillshade(c0, 180.f, 45.f, 10.f * c0.ptp() / (float)c0.shape[1]);
    remap(hs);

    int k = 0;
    for (int j = c0.shape[1] - 1; j > -1; j--)
      for (int i = 0; i < c0.shape[0]; i++)
      {
        img[k] = (uint8_t)((float)img[k] * hs(i, j));
        img[k + 1] = (uint8_t)((float)img[k + 1] * hs(i, j));
        img[k + 2] = (uint8_t)((float)img[k + 2] * hs(i, j));
        k += 3;
      }
  }

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
