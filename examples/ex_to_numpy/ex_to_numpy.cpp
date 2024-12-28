#include <iostream>

#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {512, 256};
  hmap::Vec2<float> kw = {4.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);

  std::cout << z(10, 12) << std::endl;

  z.to_numpy("out.npy");
  z.to_png("ex_to_numpy.png", hmap::Cmap::JET);

  // --- python script to check this provided below in the comments

  // import matplotlib.pyplot as plt
  // import numpy as np
  // z = np.load('out.npy')
  // print(z.shape)
  // print(z[10, 12])
  // plt.imshow(z, cmap='jet')
  // plt.show()
}
