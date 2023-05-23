#define _USE_MATH_DEFINES

#include <algorithm>
#include <cmath>

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

#include "macrologger.h"

// neighbor pattern search based on Moore pattern and define diagonal
// weight coefficients ('c' corresponds to a weight coefficient
// applied to take into account the longer distance for diagonal
// comparison between cells)

// 6 2 8
// 1 . 4
// 5 3 7
// clang-format off
#define DI {-1, 0, 0, 1, -1, -1, 1, 1}
#define DJ {0, 1, -1, 0, -1, 1, -1, 1}
#define C  {1.f, 1.f, 1.f, 1.f, M_SQRT2, M_SQRT2, M_SQRT2, M_SQRT2}
// clang-format on

namespace hmap
{

//----------------------------------------------------------------------
// Main operator
//----------------------------------------------------------------------

void thermal(Array &z,
             Array &talus,
             Array &bedrock,
             int    iterations,
             float  ct,
             bool   post_filtering)
{
  std::vector<int>   di = DI;
  std::vector<int>   dj = DJ;
  std::vector<float> c = C;
  const uint         nb = di.size();

  // main loop
  for (int it = 0; it < iterations; it++)
  {
    // modify neighbor search at each iterations to limit numerical
    // artifacts
    std::rotate(di.begin(), di.begin() + 1, di.end());
    std::rotate(dj.begin(), dj.begin() + 1, dj.end());
    std::rotate(c.begin(), c.begin() + 1, c.end());

    for (int i = 1; i < z.shape[0] - 1; i++)
    {
      for (int j = 1; j < z.shape[1] - 1; j++)
      {
        if (z(i, j) > bedrock(i, j))
        {
          float              dmax = 0.f;
          float              dsum = 0.f;
          std::vector<float> dz(nb);

          for (uint k = 0; k < nb; k++)
          {
            dz[k] = z(i, j) - z(i + di[k], j + dj[k]);
            if (dz[k] > talus(i, j) * c[k])
            {
              dsum += dz[k];
              dmax = std::max(dmax, dz[k]);
            }
          }

          if (dmax > 0.f)
          {
            for (uint k = 0; k < nb; k++)
            {
              int   ia = i + di[k];
              int   ja = j + dj[k];
              float amount =
                  std::min(ct * (dmax - talus(i, j) * c[k]) * dz[k] / dsum,
                           z(i, j) - bedrock(i, j));
              z(ia, ja) += amount;
            }
          }
        }
      }
    }
  }

  // clean-up: fix boundaries, remove spurious oscillations and make
  // sure final elevation is not lower than the bedrock
  extrapolate_borders(z);
  laplace(z);
  z = maximum(z, bedrock);
}

//----------------------------------------------------------------------
// Overloading
//----------------------------------------------------------------------

// no bedrock
void thermal(Array &z,
             Array &talus,
             int    iterations,
             float  ct,
             bool   post_filtering)
{
  Array bedrock = constant(z.shape, z.min() - z.ptp());
  thermal(z, talus, bedrock, iterations, ct, post_filtering);
}

// uniform talus limit, no bedrock
void thermal(Array &z,
             float  talus,
             int    iterations,
             float  ct,
             bool   post_filtering)
{
  Array talus_map = constant(z.shape, talus);
  Array bedrock = constant(z.shape, z.min() - z.ptp());
  thermal(z, talus_map, bedrock, iterations, ct, post_filtering);
}

//----------------------------------------------------------------------
// Macros
//----------------------------------------------------------------------

void thermal_auto_bedrock(Array &z,
                          Array &talus,
                          int    iterations,
                          float  ct,
                          bool   post_filtering)
{
  Array z_init = z; // backup initial map
  Array bedrock = constant(z.shape, z.min() - z.ptp());

  int ncycle = 10;
  for (int ic = 0; ic < ncycle; ic++) // thermal weathering cycles
  {
    thermal(z, talus, bedrock, iterations / ncycle, ct, post_filtering);

    // only keep what's above the initial ground level
    z = maximum(z_init, z);

    // define the bedrock elevation: it is equal to the initial
    // elevation if the elevation after thermal erosion is lower. If
    // not, the bedrock elevation is set to a value smaller than the
    // actual elevation (to allow more erosion).
    std::transform(z_init.vector.begin(),
                   z_init.vector.end(),
                   z.vector.begin(),
                   bedrock.vector.begin(),
                   [](float a, float b) { return a > b ? a : 2.f * a - b; });
  }
}

void thermal_auto_bedrock(Array &z,
                          float  talus,
                          int    iterations,
                          float  ct,
                          bool   post_filtering)
{
  Array talus_map = constant(z.shape, talus);
  thermal_auto_bedrock(z, talus_map, iterations, ct, post_filtering);
}

} // namespace hmap
