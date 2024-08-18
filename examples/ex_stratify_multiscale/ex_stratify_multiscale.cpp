#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  auto z1 = z;

  std::vector<int>   n_strata = {2, 3, 6};
  std::vector<float> strata_noise = {0.5f, 0.5f, 0.5f};
  std::vector<float> gamma_list = {2.f, 0.5f, 0.5f};
  std::vector<float> gamma_noise = {0.2f, 0.2f, 0.2f};

  hmap::stratify_multiscale(z1,
                            z1.min(),
                            z1.max(),
                            n_strata,
                            strata_noise,
                            gamma_list,
                            gamma_noise,
                            seed);

  hmap::export_banner_png("ex_stratify_multiscale.png",
                          {z, z1},
                          hmap::Cmap::TERRAIN,
                          true);
}
