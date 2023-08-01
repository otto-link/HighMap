#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  float             talus = 0.1f / shape.x;
  float             yaw_angle = 15.f;
  hmap::Vec3<float> xyz_center = {0.f, 0.f, 0.f};

  hmap::Array z = hmap::plane(shape, talus, yaw_angle, xyz_center);
  z.to_png("ex_plane.png", hmap::cmap::viridis);
}
