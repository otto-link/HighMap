#include "highmap.hpp"

int main(void)
{

  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::fbm_simplex(shape, res, seed);
  hmap::clamp_min_smooth(z, 0.f, 0.5f);
  hmap::remap(z);

  hmap::export_wavefront_obj("hmap.obj", z, hmap::mesh_type::tri_optimized);
  hmap::export_wavefront_obj("hmap_quad.obj", z, hmap::mesh_type::quad);
  hmap::export_wavefront_obj("hmap_tri.obj", z, hmap::mesh_type::tri);
}
