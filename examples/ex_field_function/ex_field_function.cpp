#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  uint              seed = 0;
  hmap::Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f};

  float             gain = 1.f;
  hmap::Vec2<float> center = {0.f, 0.f};

  std::unique_ptr<hmap::Function> p = std::unique_ptr<hmap::Function>(
      new hmap::BumpFunction(gain, center));

  hmap::Cloud cloud = hmap::Cloud(15, seed, bbox);
  cloud.remap_values(1.f, 4.f);

  hmap::FieldFunction field_fct = hmap::FieldFunction(std::move(p),
                                                      cloud.get_x(),
                                                      cloud.get_y(),
                                                      cloud.get_values());

  //

  hmap::Array z = hmap::Array(shape);
  fill_array_using_xy_function(z,
                               hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f),
                               nullptr,
                               nullptr,
                               nullptr,
                               nullptr,
                               field_fct.get_delegate());

  z.to_png("ex_field_function.png", hmap::cmap::jet, true);
}
