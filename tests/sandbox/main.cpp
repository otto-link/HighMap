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
    std::vector<float> bbox = {-1.f, 2.f, 2.f, 5.f};

    hmap::Cloud cloud = hmap::Cloud(15, 1, bbox);
    hmap::Graph graph = cloud.to_graph_delaunay();

    // graph.print();

    graph.to_png("tmp.png");
    graph.to_csv("path.csv");

    graph.update_adjacency_matrix();
    graph.update_connectivity();
    std::vector<int> path = graph.dijkstra(0, 4);

    std::cout << "Path:\n";
    for (auto &p : path)
      std::cout << p << "\n";

    graph = graph.minimum_spanning_tree_prim();

    graph.to_png("graph.png");

    z.infos();
  }

  z.to_png("out.png", hmap::cmap::terrain, false);
  z0.to_png("out0.png", hmap::cmap::terrain, true);
  z.to_file("out.bin");

  z.to_png("hmap.png", hmap::cmap::gray, false);
}
