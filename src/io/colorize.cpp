#include <cmath>
#include <stdint.h>
#include <vector>

#include "highmap/array.hpp"
#include "highmap/io.hpp"

// helper - convert value to color range [0, 255]
#define V2C(v, vmin, vmax)                                                     \
  (uint8_t) std::floor(255 * ((v - vmin) / (vmax - vmin)))

namespace hmap
{

std::vector<uint8_t> colorize(hmap::Array &array,
                              float        vmin,
                              float        vmax,
                              int          cmap)
{
  std::vector<uint8_t> data(IMG_CHANNELS * array.shape[0] * array.shape[1]);

  int k = -1;

  switch (cmap)
  {
  case cmap::gray:
    // reorganize things to get an image with (i, j) used as (x, y)
    // coordinates, i.e. with (0, 0) at the bottom left
    for (int j = array.shape[1] - 1; j > -1; j--)
    {
      for (int i = 0; i < array.shape[0]; i++)
      {
        uint8_t c = V2C(array(i, j), vmin, vmax);
        data[++k] = c;
        data[++k] = c;
        data[++k] = c;
      }
    }
    break;

  case cmap::jet: // https://stackoverflow.com/questions/7706339
    for (int j = array.shape[1] - 1; j > -1; j--)
    {
      for (int i = 0; i < array.shape[0]; i++)
      {
        float v = (array(i, j) - vmin) / (vmax - vmin);
        float r, g, b;

        if (v < 0.25)
        {
          r = 0.f;
          g = 4.f * v;
          b = 1.f;
        }
        else if (v < 0.5)
        {
          r = 0.f;
          g = 1.f;
          b = 1.f - 4.f * (v - 0.25f);
        }
        else if (v < 0.75)
        {
          r = 4.f * (v - 0.5f);
          g = 1.f;
          b = 0.f;
        }
        else
        {
          r = 1.f;
          g = 1.f - 4.f * (v - 0.75f);
          b = 0.f;
        }

        // uint8_t v = V2C(array(i, j), vmin, vmax);
        data[++k] = V2C(g, 0.f, 1.f);
        data[++k] = V2C(b, 0.f, 1.f);
        data[++k] = V2C(r, 0.f, 1.f);
      }
    }
    break;
  }

  return data;
}

} // namespace hmap
