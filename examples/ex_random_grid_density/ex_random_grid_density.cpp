#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;
  int               n_grid_points = 500;

  hmap::Array density = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                        shape,
                                        res,
                                        seed);
  hmap::remap(density);
  density.to_png("ex_random_grid_density0.png", hmap::Cmap::MAGMA);

  std::vector<float> x(n_grid_points);
  std::vector<float> y(n_grid_points);
  std::vector<float> v(n_grid_points);

  {
    hmap::random_grid_density(x, y, density, seed);

    hmap::Cloud cloud = hmap::Cloud(x, y, 1.f);
    hmap::Array array = hmap::Array(shape);
    cloud.to_array(array);
    array.to_png("ex_random_grid_density1.png", hmap::Cmap::GRAY);
  }
}
