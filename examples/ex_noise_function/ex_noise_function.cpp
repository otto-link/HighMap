#include "highmap.hpp"
#include "highmap/dbg.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  // shape = {2048, 2048};
  hmap::Vec2<float> kw = {4.f, 4.f};
  uint              seed = 1;

  hmap::Timer timer = hmap::Timer();

  // std::unique_ptr<hmap::NoiseFunction> pa =
  //     std::unique_ptr<hmap::NoiseFunction>(new hmap::PerlinFunction(kw,
  //     seed));

  // std::unique_ptr<hmap::NoiseFunction> pa =
  //     std::unique_ptr<hmap::NoiseFunction>(
  //         new hmap::PerlinMixFunction(kw, seed));

  // std::unique_ptr<hmap::NoiseFunction> pa =
  //     std::unique_ptr<hmap::NoiseFunction>(
  //         new hmap::PerlinHalfFunction(kw, seed, 1.f));

  // std::unique_ptr<hmap::NoiseFunction> pb =
  //     std::unique_ptr<hmap::NoiseFunction>(
  //         new hmap::WorleyDoubleFunction(kw, seed, 0.5f, 0.5f));

  hmap::PerlinFunction p = hmap::PerlinFunction(kw, seed);
  // hmap::Simplex2Function     p = hmap::Simplex2Function(kw, seed);
  hmap::WorleyDoubleFunction q = hmap::WorleyDoubleFunction(kw,
                                                            seed,
                                                            0.5f,
                                                            0.1f);

  hmap::WaveSineFunction s = hmap::WaveSineFunction(kw, 15.f, 0.f);
  s.set_angle(30.f);

  // hmap::FbmFunction f = hmap::FbmFunction(p.get_base_ref(), 8, 0.7f,
  // 0.5f, 2.f);

  hmap::FbmRidgedFunction f = hmap::FbmRidgedFunction(p.get_base_ref(),
                                                      8,
                                                      0.7f,
                                                      0.5f,
                                                      2.f);

  // hmap::FbmSwissFunction f = hmap::FbmSwissFunction(pa.get()->get_base_ref(),
  //                                                   8,
  //                                                   0.7f,
  //                                                   0.5f,
  //                                                   2.f,
  //                                                   0.1f);

  std::vector<float> x = hmap::linspace(0.f, 1.f, shape.x, false);
  std::vector<float> y = hmap::linspace(0.f, 1.f, shape.y, false);

  hmap::Array z = hmap::Array(shape);

  timer.start("NoiseFunction");
  fill_array_using_xy_function(z,
                               x,
                               y,
                               nullptr,
                               nullptr,
                               nullptr,
                               f.get_function());
  timer.stop("NoiseFunction");

  timer.start("ArrayOp");
  auto a = hmap::fbm_perlin(shape, kw, seed);
  timer.stop("ArrayOp");

  timer.start("ArrayNew");
  auto b = hmap::fbm(shape,
                     kw,
                     seed,
                     hmap::noise_type::noise_perlin,
                     hmap::fractal_type::fractal_fbm);
  timer.stop("ArrayNew");

  z.infos();
  z.to_png("out.png", hmap::cmap::terrain, true);
}
