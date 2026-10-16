#include <iostream>
#include <vector>

#include "highmap.hpp"

int main(void)
{
  glm::ivec2    shape = {512, 512};
  std::uint32_t seed = 42;

  // Synthesize terrain with Perlin noise
  hmap::Array terrain = hmap::noise(hmap::NoiseType::PERLIN,
                                    shape,
                                    glm::vec2(3.f, 3.f),
                                    seed);
  hmap::remap(terrain, 0.f, 1.f);

  // Configuration options for POI extraction
  hmap::PoiGenerationOptions opt;
  opt.scale_radius = 16;
  opt.prominence = 0.01f;
  opt.threshold = 0.5f;
  opt.min_distance = 0.06f;
  opt.max_points = 30;

  // 1. Detect peaks (Apexes)
  auto apex_pois = hmap::generate_points_of_interest(terrain,
                                                     hmap::PoiType::APEX,
                                                     opt);

  // 2. Detect depressions (Sinks)
  auto sink_pois = hmap::generate_points_of_interest(terrain,
                                                     hmap::PoiType::SINK,
                                                     opt);

  std::cout << "Detected " << apex_pois.size() << " peaks (apexes):\n";
  for (size_t i = 0; i < apex_pois.size(); ++i)
  {
    std::cout << "  Peak #" << i << ": pos=(" << apex_pois[i].position.x << ", "
              << apex_pois[i].position.y << "), elev=" << apex_pois[i].elevation
              << ", score=" << apex_pois[i].score
              << ", aspect=" << apex_pois[i].aspect << "\n";
  }

  std::cout << "\nDetected " << sink_pois.size() << " sinks (depressions):\n";
  for (size_t i = 0; i < sink_pois.size(); ++i)
  {
    std::cout << "  Sink #" << i << ": pos=(" << sink_pois[i].position.x << ", "
              << sink_pois[i].position.y << "), elev=" << sink_pois[i].elevation
              << ", score=" << sink_pois[i].score
              << ", aspect=" << sink_pois[i].aspect << "\n";
  }

  // Visualization: render suitability field and rasterize markers
  hmap::Array apex_suitability = hmap::compute_poi_suitability_field(
      terrain,
      hmap::PoiType::APEX,
      opt);

  hmap::Cloud apex_cloud = hmap::generate_points_of_interest_cloud(
      terrain,
      hmap::PoiType::APEX,
      opt);

  hmap::Array marker_raster(shape);
  apex_cloud.to_array(marker_raster);

  std::vector<hmap::Array> zs = {terrain, apex_suitability, marker_raster};
  hmap::export_banner_png("ex_poi.png", zs, hmap::Cmap::BONE);

  std::cout << "\nSaved visualization to ex_poi.png\n";
  return 0;
}
