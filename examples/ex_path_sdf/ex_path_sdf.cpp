#include <iostream>

#include "highmap.hpp"

// helper
void fill_array(hmap::Array                       &array,
                hmap::Vec4<float>                  bbox,
                std::function<float(float, float)> distance_fct)
{
  std::vector<float> x = hmap::linspace(bbox.a, bbox.b, array.shape.x);
  std::vector<float> y = hmap::linspace(bbox.c, bbox.d, array.shape.y);

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
      array(i, j) = distance_fct(x[i], y[j]);
}

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  uint            seed = 1;

  auto noise = hmap::fbm_perlin(shape, {2, 2}, seed);
  hmap::remap(noise, 0.f, 0.1f);

  hmap::Vec4<float> bbox = {0.2f, 0.8f, 0.2f, 0.8f};
  hmap::Path        path = hmap::Path(5, seed, bbox);
  path.reorder_nns();

  hmap::Vec4<float> bbox_array = {0.f, 1.f, 0.f, 1.f};

  // --- distance

  hmap::Array zo = hmap::Array(shape);
  {
    auto sdf = [&path](float x, float y) { return path.sdf_open(x, y); };
    fill_array(zo, bbox_array, sdf);
  }

  hmap::Array zc = hmap::Array(shape);
  {
    auto sdf = [&path](float x, float y) { return path.sdf_closed(x, y); };
    fill_array(zc, bbox_array, sdf);
  }

  // --- angle

  hmap::Array za_o = hmap::Array(shape);
  {
    auto sdf = [&path](float x, float y) { return path.sdf_angle_open(x, y); };
    fill_array(za_o, bbox_array, sdf);
  }

  hmap::Array zc_o = hmap::Array(shape);
  {
    auto sdf = [&path](float x, float y)
    { return path.sdf_angle_closed(x, y); };
    fill_array(zc_o, bbox_array, sdf);
  }

  hmap::export_banner_png("ex_path_sdf0.png", {zo, zc}, hmap::cmap::inferno);
  hmap::export_banner_png("ex_path_sdf1.png", {za_o, zc_o}, hmap::cmap::jet);
}
