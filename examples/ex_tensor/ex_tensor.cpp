#include "highmap.hpp"

int main(void)
{
  hmap::Vec3<int> shape = {512, 256, 4};
  hmap::Tensor    array3 = hmap::Tensor(shape);

  array3.set_slice(1, hmap::Array({shape.x, shape.y}, 1.f));

  hmap::Array pulse = hmap::gaussian_pulse({shape.x, shape.y}, 0.1f);
  array3.set_slice(3, pulse);

  array3.to_png("ex_array3_0.png");         // 8bit png
  array3.to_png("ex_array3_1.png", CV_16U); // 16bit png

  pulse.to_png("out.png", hmap::Cmap::TERRAIN, true);

  pulse.to_png_grayscale("out_gs8.png", CV_8U);
  pulse.to_png_grayscale("out_gs16.png", CV_16U);
  pulse.to_exr("out.exr");
  pulse.to_tiff("out.tiff");
}
