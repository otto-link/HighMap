#include <iostream>
#include <time.h>

// #include <xsimd.hpp>

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/geometry.hpp"
#include "highmap/hydrology.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"
#include "highmap/roads.hpp"

#include "highmap/dbg.hpp"

int main(void)
{

  hmap::Timer timer = hmap::Timer();

  // const std::vector<int> shape = {2048, 2048};
  // const std::vector<int> shape = {1024, 1024};
  const std::vector<int> shape = {512, 512};
  // const std::vector<int> shape = {256, 256};
  // const std::vector<int> shape = {32, 32};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 2;

  // seed = (int)time(NULL);

  std::cout << "seed: " << seed << std::endl;

  timer.start("fbm");
  hmap::Array z = hmap::fbm_perlin(shape, res, seed, 8);
  timer.stop("fbm");

  // for (int i = 0; i < z.shape[0]; i++)
  //   for (int j = 0; j < z.shape[1]; j++)
  //   {
  //     float x = (float)i / (z.shape[0] - 1);
  //     float y = (float)j / (z.shape[1] - 1);
  //     float r2 = (x - 0.5f) * (x - 0.5f) + (y - 0.5f) * (y - 0.5f);
  //     z(i, j) = std::exp(-r2 / 2 / 0.1f / 0.1f);
  //   }

  z.infos();

  // hmap::set_borders(z, z.min(), shape[0] / 2);
  // hmap::smooth_fill(z, shape[0] / 4);

  // hmap::remap(z);
  // hmap::steepen_convective(z, 0.f, 20, 8);

  hmap::remap(z);
  auto z0 = z;

  if (false)
  {
    std::vector<float> bbox = {1.f, 2.f, -0.5f, 0.5f};

    // hmap::Cloud cloud = hmap::Cloud(10, seed, {1.1f, 1.9f, -0.4, 0.4f});

    hmap::Path path = hmap::Path(10, ++seed, {1.1f, 1.9f, -0.4, 0.4f});
    path.reorder_nns();
    path.closed = true;

    path.to_png("path0.png");

    path.bezier(0.5f, 20);

    path.to_png("path.png");

    z = 0.f;
    path.to_array(z, bbox);
  }

  z.to_png("out.png", hmap::cmap::gray, false);

  z0.to_png("out0.png", hmap::cmap::terrain, true);
  z.to_file("out.bin");

  z.to_png("hmap.png", hmap::cmap::gray, false);
}
