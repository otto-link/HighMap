#include <iostream>

#include "highmap.hpp"

int main(void)
{

  // --- heightmaps

  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise(hmap::NoiseType::SIMPLEX2, shape, res, seed);
  hmap::clamp_min_smooth(z, 0.f, 0.2f);
  hmap::remap(z);

  z.to_png("hmap.png", hmap::Cmap::TERRAIN);

  for (auto &[export_id, export_infos] : hmap::asset_export_format_as_string)
  {
    std::cout << "exporting format: " << export_infos[0] << "\n";

    float error_tolerance = 1e-2f; // for tri_optimized only

    hmap::export_asset("hmap.dummy_extension",
                       z,
                       hmap::MeshType::TRI_OPTIMIZED,
                       export_id,
                       0.2f,
                       "hmap.png",
                       "", // normal map
                       error_tolerance);
  }

  return 0;
}
