#include <iostream>

#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int> shape = {256, 256};
  float                  talus = 0.1f / shape[0];
  float                  yaw_angle = 15.f;
  std::vector<float>     xyz_center = {0.f, 0.f, 0.f};

  hmap::Array z = hmap::plane(shape, talus, yaw_angle, xyz_center);
  z.to_png("ex_plane.png", hmap::cmap::gray);
}
