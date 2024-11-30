#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>          shape = {256, 256};
  std::vector<hmap::Array> kernels = {};

  kernels.push_back(hmap::cone_smooth(shape));
  kernels.push_back(hmap::disk(shape));
  kernels.push_back(hmap::lorentzian(shape));
  kernels.push_back(hmap::lorentzian_compact(shape));
  kernels.push_back(hmap::smooth_cosine(shape));
  kernels.push_back(hmap::square(shape));
  kernels.push_back(hmap::tricube(shape));

  // windows
  kernels.push_back(hmap::blackman(shape));
  kernels.push_back(hmap::hann(shape));

  // specific kernel(s)
  float kw = 4.f;
  kernels.push_back(hmap::sinc_radial(shape, kw));
  kernels.push_back(hmap::sinc_separable(shape, kw));

  // generic call function
  kernels.push_back(hmap::get_kernel(shape, hmap::KernelType::TRICUBE));

  hmap::export_banner_png("ex_kernels.png", kernels, hmap::Cmap::JET);
}
