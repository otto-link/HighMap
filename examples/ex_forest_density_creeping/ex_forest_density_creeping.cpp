#include <iostream>

#include "highmap.hpp"

int main()
{
  glm::ivec2 shape = {512, 512};
  glm::vec2  kw = {4.f, 4.f};
  int        seed = 42;

  // --- Terrain Elevation

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::SIMPLEX2, shape, kw, seed);
  z = hmap::bulkify(z, hmap::PrimitiveType::PRIM_CUBIC_PULSE, 2.f);
  hmap::remap(z);

  // --- Base Environmental Suitability Density (without creeping)

  hmap::Array density_base = hmap::build_tree_density(z,
                                                      0.1f, // min_elev
                                                      0.7f, // max_elev
                                                      0.05f,
                                                      2.f / shape.x, // min_talus
                                                      4.f / shape.x, // max_talus
                                                      45.f,          // angle
                                                      80.f,          // angle_width
                                                      1.0f,          // weight_elev
                                                      1.0f,          // weight_talus
                                                      0.7f,          // weight_angle
                                                      0.5f);         // weight_twi

  std::cout << "Base density sum: " << density_base.sum() << std::endl;

  // --- Gradual Vegetation Creeping

  float creeping_distance = 0.04f; // 4% of domain width (~20 pixels)
  float creeping_intensity = 0.7f;

  hmap::Array density_crept = hmap::creep_forest_density(density_base,
                                                         creeping_distance,
                                                         creeping_intensity);

  std::cout << "Crept density sum: " << density_crept.sum() << std::endl;

  // --- Seed Forest Using Crept Density

  size_t species_count = 3;
  size_t tree_count = 2000;

  hmap::ForestSeedingOptions options;
  options.seed = static_cast<uint32_t>(seed);
  options.species = {
      hmap::Species(0, 0.003f, 1.0f),
      hmap::Species(1, 0.002f, 0.8f),
      hmap::Species(2, 0.001f, 0.5f),
  };

  hmap::Array empty_exclusion;

  hmap::Forest forest = hmap::seed_forest_clusters(species_count,
                                                   tree_count,
                                                   density_crept,
                                                   empty_exclusion,
                                                   0.04f,
                                                   8,
                                                   options);

  forest.set_elevation_from_terrain(z);

  std::cout << forest.to_string() << std::endl;

  // export images
  density_base.to_png_grayscale("ex_forest_density_base.png");
  density_crept.to_png_grayscale("ex_forest_density_crept.png");
  forest.to_png("ex_forest_crept_trees.png", shape, z);

  return 0;
}
