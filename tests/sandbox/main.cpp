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

#include "highmap/dbg.hpp"

int main(void)
{

  hmap::Timer timer = hmap::Timer();

  // const std::vector<int> shape = {2048, 2048};
  // const std::vector<int> shape = {1024, 1024};
  const std::vector<int> shape = {512, 512};
  // const std::vector<int> shape = {256, 256};
  // const std::vector<int> shape = {32, 32};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 2;

  // seed = (int)time(NULL);

  std::cout << "seed: " << seed << std::endl;

  timer.start("fbm");
  hmap::Array z = hmap::fbm_perlin(shape, res, seed, 8);
  timer.stop("fbm");

  // hmap::set_borders(z, z.min(), shape[0] / 2);
  hmap::smooth_fill(z, shape[0] / 4);

  // hmap::remap(z);
  // hmap::steepen_convective(z, 0.f, 20, 8);

  hmap::remap(z);
  auto z0 = z;

  // --- points
  if (true)
  {
    hmap::Point p1 = hmap::Point(0.f, 1.f);
    hmap::Point p2 = hmap::Point(1.f, 3.f, 2.f);
    hmap::Point p3 = hmap::Point(-0.5f, 4.f, 3.f);

    // hmap::Cloud cloud = hmap::Cloud({p1, p2});
    // cloud.add_point(p1);
    // cloud.add_point(p2);

    // hmap::Cloud cloud = hmap::Cloud({1.f, 2.f}, {0.f, 1.f}, {5.f, 4.f});

    std::vector<float> bbox = {-1.f, 2.f, 2.f, 5.f};

    hmap::Path path = hmap::Path(5, 1, bbox, true);
    // hmap::Path path = hmap::Path({p1, p2, p3}, true);

    path.reorder_nns();
    // path.divide();
    // path.uniform_resampling();

    // path.divide();
    // path.divide();
    // path.divide();

    // path.fractalize(2, seed, 0.2f, 0.f);

    hmap::Cloud cloud = hmap::Cloud(10, 1, bbox);
    hmap::Graph graph = cloud.to_graph_delaunay();

    graph.to_png("tmp.png");
    graph.to_csv("path.csv");

    // z = 0.f;
    // path.to_array(z, {-1.5f, 2.5f, 1.5f, 5.5f});

    z.infos();
  }

  z.to_png("out.png", hmap::cmap::terrain, false);
  z0.to_png("out0.png", hmap::cmap::terrain, true);
  z.to_file("out.bin");

  z.to_png("hmap.png", hmap::cmap::gray, false);
}
