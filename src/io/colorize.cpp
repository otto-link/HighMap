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
    std::vector<float>              v = {0.f, 1.f};
    std::vector<std::vector<float>> c = {{0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}};
    color_bounds = v;
    colors = c;
  }
  break;

  case cmap::jet: // https://stackoverflow.com/questions/7706339
  case -cmap::jet:
  {
    std::vector<float>              v = {0.f, 0.25f, 0.5f, 0.75f, 1.f};
    std::vector<std::vector<float>> c = {{0.f, 0.f, 1.f},
                                         {0.f, 1.f, 1.f},
                                         {0.f, 1.f, 0.f},
                                         {1.f, 1.f, 0.f},
                                         {1.f, 0.f, 0.f}};
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
