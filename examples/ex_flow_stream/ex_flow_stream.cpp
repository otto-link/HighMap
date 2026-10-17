#include "highmap.hpp"

int main(void)
{
  glm::ivec2 shape = {256, 256};
  // shape = {1024, 1024};
  glm::vec2 kw = {4.f, 4.f};
  int       seed = 1;

  // base terrain with Perlin noise
  hmap::Array z = 0.2f *
                  hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);
  z = hmap::bulkify(z, hmap::PrimitiveType::PRIM_SMOOTH_COSINE, 1.f);

  hmap::remap(z);

  // generate triangular terrain mesh from heightmap
  float                max_error = 1e-2f;
  hmap::TerrainTriMesh mesh = hmap::generate_terrain_tri_mesh_from_heightmap(
      z,
      max_error);

  // multiple starting points across the slopes of the valley head
  std::vector<glm::vec2> start_points = {
      {100.f / float(shape.x - 1), 200.f / float(shape.y - 1)},
      {155.f / float(shape.x - 1), 210.f / float(shape.y - 1)},
      {100.f / float(shape.x - 1), 80.f / float(shape.y - 1)},
      {80.f / float(shape.x - 1), 100.f / float(shape.y - 1)},
      {70.f / float(shape.x - 1), 124.f / float(shape.y - 1)},
      {70.f / float(shape.x - 1), 150.f / float(shape.y - 1)},
      {60.f / float(shape.x - 1), 170.f / float(shape.y - 1)},
      {175.f / float(shape.x - 1), 160.f / float(shape.y - 1)}};

  std::vector<hmap::Path> streams;

  for (const auto &start : start_points)
  {
    hmap::Path stream = hmap::flow_stream(mesh,
                                          start,
                                          0.95f,
                                          2.f,
                                          100.f,
                                          true,
                                          streams,
                                          0.02f,
                                          0.4f,
                                          true);

    if (!stream.empty()) streams.push_back(stream);
  }

  // convert merged streams into a connected Graph
  hmap::Graph stream_graph = hmap::hydrology_paths_to_graph(streams, 1e-3f);

  // visualize graph on an empty grid
  glm::vec4   bbox(0.f, 1.f, 0.f, 1.f);
  hmap::Array z_graph(shape, 0.f);
  stream_graph.to_array(z_graph, bbox, false);

  // carve consistent river network with trench evolving width downstream
  hmap::Array z_trenched = z;
  float       base_width = 0.02f;
  float       width_exponent = 1.f;

  hmap::trench(z_trenched,
               stream_graph,
               base_width,
               width_exponent,
               /* enable_width_depth_scaling */ true,
               hmap::RadialProfile::RP_SMOOTHSTEP_UPPER,
               2.f,
               hmap::ElevationLongitudinalProfile::ELP_DECREASING,
               -0.05f);

  hmap::export_banner_png("ex_flow_stream.png",
                          {z, z_graph, z_trenched},
                          hmap::Cmap::JET,
                          true);
}
