#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};

  // "segments", coordinates are given by groups of three points in
  // order to define a quadratic bezier curve, start point, control
  // point and end point (xa_1, xa_2, xa_3, xb_1, xb_2, xb_3...)
  std::vector<float> x = {0.1f, 0.5f, 0.7f};
  std::vector<float> y = {0.6f, 0.2f, 0.8f};
  std::vector<float> v = {1.0f, 0.f, 0.5f}; // control pt (middle)
                                            // value not used

  float slope = 4.f;

  auto z1 = hmap::ridgelines_bezier(shape, x, y, v, slope);
  hmap::remap(z1);

  v = {-1.0f, 0.f, -0.5f};
  auto z2 = hmap::ridgelines_bezier(shape, x, y, v, -slope);
  hmap::remap(z2);

  hmap::export_banner_png("ex_ridgelines_bezier.png",
                          {z1, z2},
                          hmap::Cmap::INFERNO);
}
