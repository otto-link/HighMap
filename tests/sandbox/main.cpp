#include "highmap.hpp"

#include "highmap/dbg.hpp"

int main(void)
{
  const std::vector<int> shape = {512, 512};
  // const std::vector<int>   shape = {4096 * 2, 4096 * 2};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  hmap::Timer timer = hmap::Timer();

  hmap::Array z = hmap::perlin(shape, res, seed);

  std::vector<int> tiling = {4, 2};
  hmap::HeightMap  h = hmap::HeightMap(shape, tiling);
  hmap::HeightMap  h2 = hmap::HeightMap(shape, tiling);

  h.infos();

  timer.start("fill");
  hmap::fill(h,
             [&h, &res, &seed](std::vector<int> shape, std::vector<float> shift)
             {
               return hmap::fbm_perlin(shape,
                                       h.rescale_kw(res),
                                       seed,
                                       8,
                                       0.7f,
                                       0.5f,
                                       2.f,
                                       shift);
             });
  timer.stop("fill");

  hmap::fill(h2,
             [](std::vector<int> shape) { return hmap::constant(shape, 0.f); });

  h.remap();

  timer.start("transform");
  hmap::transform(h, [](hmap::Array &x) { hmap::gamma_correction(x, 2.f); });
  timer.stop("transform");

  hmap::transform(h,
                  h2,
                  [](hmap::Array &x, hmap::Array &y)
                  { hmap::clamp_min(x, y); });

  timer.start("transform seq");
  hmap::gamma_correction(z, 2.f);
  timer.stop("transform seq");

  // z.to_png("out.png", hmap::cmap::inferno);

  timer.start("preview");
  h.to_array().to_png("out_h.png", hmap::cmap::jet);
  timer.stop("preview");
}
