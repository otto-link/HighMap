#include "highmap.hpp"

int main(void)
{
  hmap::Vec3<int> shape = {512, 256, 4};
  hmap::Array3    array3 = hmap::Array3(shape);

  array3.set_slice(1, hmap::Array({shape.x, shape.y}, 1.f));

  hmap::Array pulse = hmap::gaussian_pulse({shape.x, shape.y}, 0.1f);
  array3.set_slice(3, pulse);

  array3.to_png_8bit("ex_array3_0.png");
  array3.to_png_16bit("ex_array3_1.png");

  pulse.to_png("out.png", hmap::Cmap::TERRAIN, true);

  pulse.to_png_grayscale_8bit("out_gs8.png");
  pulse.to_png_grayscale_16bit("out_gs16.png");
  pulse.to_exr("out.exr");
  pulse.to_tiff("out.tiff");
}
