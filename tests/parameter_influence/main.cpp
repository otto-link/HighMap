#include <functional>
#include <iostream>
#include <string>

#include "highmap.hpp"
#include "highmap/dbg.hpp"

template <typename T>
void show_influence(std::string                           label,
                    hmap::Array                          &x,
                    std::vector<T>                        params,
                    std::function<void(hmap::Array &, T)> unary_op)
{
  hmap::Array x_bckp = x; // bckp

  std::cout << "label: " << label.c_str() << "\n";
  for (auto &v : params)
  {
    std::cout << "- parameter value: " << v << "\n";
    hmap::Array x_tmp = x_bckp;
    unary_op(x_tmp, v);
    x_tmp.to_png(label + "-" + std::to_string(v) + ".png", hmap::cmap::magma);
  }
}

int main(void)
{
  hmap::Timer timer = hmap::Timer();

  const hmap::Vec2<int>   shape = {512, 512};
  const hmap::Vec2<float> res = {4.f, 4.f};
  int                     seed = 1;

  hmap::Array z = hmap::noise(hmap::NoiseType::n_perlin, shape, res, seed);
  hmap::remap(z);

  show_influence<float>("gain-gain",
                        z,
                        {1.f, 2.f, 4.f, 8.f, 16.f},
                        [](hmap::Array &x, float gain)
                        { return hmap::gain(x, gain); });

  show_influence<float>("gamma_correction-gamma",
                        z,
                        {0.1f, 0.5f, 1.f, 2.f, 4.f},
                        [](hmap::Array &x, float gamma)
                        { return hmap::gamma_correction(x, gamma); });

  show_influence<float>(
      "gamma_correction_local-gamma",
      z,
      {0.1f, 0.5f, 1.f, 2.f, 4.f},
      [](hmap::Array &x, float gamma)
      { return hmap::gamma_correction_local(x, gamma, 16, 0.f); });

  show_influence<int>("gamma_correction_local-ir",
                      z,
                      {4, 8, 16, 32, 64},
                      [](hmap::Array &x, int ir) {
                        return hmap::gamma_correction_local(x, 2.f, ir, 0.f);
                      });

  show_influence<float>("gamma_correction_local-k",
                        z,
                        {0.f, 0.1f, 0.2f, 0.4f, 0.8f},
                        [](hmap::Array &x, float k) {
                          return hmap::gamma_correction_local(x, 2.f, 16, k);
                        });

  show_influence<int>("expand-ir",
                      z,
                      {8, 16, 32, 64, 128},
                      [](hmap::Array &x, int ir)
                      { return hmap::expand(x, ir); });
}
