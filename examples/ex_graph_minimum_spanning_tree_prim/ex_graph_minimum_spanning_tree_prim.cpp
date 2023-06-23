#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/io.hpp"

int main(void)
{
  const std::vector<int> shape = {256, 256};
  int                    seed = 1;

  std::vector<float> bbox = {-1.f, 2.f, 0.f, 5.f};

  // create a cloud of points and convert it to a graph using Delaunay
  // triangulation
  int         npoints = 10;
  hmap::Cloud cloud = hmap::Cloud(npoints, seed, bbox);
  hmap::Graph graph_delaunay = cloud.to_graph_delaunay();

  graph_delaunay.update_adjacency_matrix();
  hmap::Graph graph_mst = graph_delaunay.minimum_spanning_tree_prim();

  graph_delaunay.to_png("ex_graph_minimum_spanning_tree_prim0.png");
  graph_mst.to_png("ex_graph_minimum_spanning_tree_prim1.png");
}
