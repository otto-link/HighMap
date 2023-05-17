#include <cmath>
#include <stdint.h>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/io.hpp"

// helper - convert value to color range [0, 255]
#define V2C_8BIT(v, vmin, vmax)                                                \
  (uint8_t) std::floor(255 * ((v - vmin) / (vmax - vmin)))

namespace hmap
{

std::vector<uint8_t> colorize(hmap::Array &array,
                              float        vmin,
                              float        vmax,
                              int          cmap)
{
  std::vector<uint8_t> data(IMG_CHANNELS * array.shape[0] * array.shape[1]);

  std::vector<float>              color_bounds;
  std::vector<std::vector<float>> colors;

  // -- define colormap

  switch (cmap)
  {

  case cmap::gray:
  case -cmap::gray:
  {
    std::vector<float>              v = {0.000f,
                                         0.091f,
                                         0.182f,
                                         0.273f,
                                         0.364f,
                                         0.455f,
                                         0.545f,
                                         0.636f,
                                         0.727f,
                                         0.818f,
                                         0.909f,
                                         1.000f};
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
    color_bounds = v;
    colors = c;
  }
  break;

  case cmap::hot:
  case -cmap::hot:
  {
    std::vector<float>              v = {0.000f,
                                         0.091f,
                                         0.182f,
                                         0.273f,
                                         0.364f,
                                         0.455f,
                                         0.545f,
                                         0.636f,
                                         0.727f,
                                         0.818f,
                                         0.909f,
                                         1.000f};
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
    color_bounds = v;
    colors = c;
  }
  break;

  case cmap::jet:
  case -cmap::jet:
  {
    std::vector<float>              v = {0.000f,
                                         0.091f,
                                         0.182f,
                                         0.273f,
                                         0.364f,
                                         0.455f,
                                         0.545f,
                                         0.636f,
                                         0.727f,
                                         0.818f,
                                         0.909f,
                                         1.000f};
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
    color_bounds = v;
    colors = c;
  }
  break;

  case cmap::nipy_spectral:
  case -cmap::nipy_spectral:
  {
    std::vector<float>              v = {0.000f,
                                         0.091f,
                                         0.182f,
                                         0.273f,
                                         0.364f,
                                         0.455f,
                                         0.545f,
                                         0.636f,
                                         0.727f,
                                         0.818f,
                                         0.909f,
                                         1.000f};
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
    color_bounds = v;
    colors = c;
  }
  break;

  case cmap::terrain:
  case -cmap::terrain:
  {
    std::vector<float>              v = {0.000f,
                                         0.091f,
                                         0.182f,
                                         0.273f,
                                         0.364f,
                                         0.455f,
                                         0.545f,
                                         0.636f,
                                         0.727f,
                                         0.818f,
                                         0.909f,
                                         1.000f};
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
    color_bounds = v;
    colors = c;
  }
  break;

  case cmap::viridis:
  case -cmap::viridis:
  {
    std::vector<float>              v = {0.000f,
                                         0.091f,
                                         0.182f,
                                         0.273f,
                                         0.364f,
                                         0.455f,
                                         0.545f,
                                         0.636f,
                                         0.727f,
                                         0.818f,
                                         0.909f,
                                         1.000f};
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
    color_bounds = v;
    colors = c;
  }
  break;
  }

  // -- define data

  const int nc = (int)color_bounds.size();
  int       k = -1;

  // reorganize things to get an image with (i, j) used as (x, y)
  // coordinates, i.e. with (0, 0) at the bottom left
  for (int j = array.shape[1] - 1; j > -1; j--)
  {

    for (int i = 0; i < array.shape[0]; i++)
    {
      float              v = (array(i, j) - vmin) / (vmax - vmin);
      std::vector<float> rgb(3);

      // reverse colormap if requested
      if (cmap < 0)
        v = 1.f - v;

      int ic = (int)(v * (float)(nc - 1));

      if (ic == nc - 1)
      {
        rgb = colors[nc - 1];
      }
      else
      {
        float t =
            (v - color_bounds[ic]) / (color_bounds[ic + 1] - color_bounds[ic]);
        for (int p = 0; p < 3; p++)
          rgb[p] = (1.f - t) * colors[ic][p] + t * colors[ic + 1][p];
      }

      data[++k] = V2C_8BIT(rgb[0], 0.f, 1.f);
      data[++k] = V2C_8BIT(rgb[1], 0.f, 1.f);
      data[++k] = V2C_8BIT(rgb[2], 0.f, 1.f);
    }
  }

  return data;
}

} // namespace hmap
