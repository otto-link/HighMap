#include "highmap.hpp"
#include "highmap/dbg.hpp"

int main(void)
{
  hmap::Timer timer = hmap::Timer();

  {
    const hmap::Vec2<int>   shape = {512, 512};
    const hmap::Vec2<float> res = {4.f, 4.f};
    int                     seed = 2;

    timer.start("fbm_perlin");
    hmap::Array z = hmap::fbm_perlin(shape, res, seed);
    timer.stop("fbm_perlin");

    auto facc = hmap::flow_accumulation_dinf(z, 0.1f);
    hmap::clamp(facc, 0.f, 40.f);

    facc.to_png("out.png", hmap::cmap::inferno);
  }

  hmap::Vec2<int>   shape = {512, 512};
  hmap::Vec2<int>   tiling = {4, 2};
  float             overlap = 0.25;
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 2;

  hmap::HeightMap h = hmap::HeightMap(shape, tiling, overlap);

  hmap::fill(h,
             [&kw, &seed](hmap::Vec2<int>   shape,
                          hmap::Vec2<float> shift,
                          hmap::Vec2<float> scale)
             {
               return hmap::fbm_perlin(shape,
                                       kw,
                                       seed,
                                       8,
                                       0.7f,
                                       0.5f,
                                       2.f,
                                       nullptr,
                                       nullptr,
                                       shift,
                                       scale);
             });

  hmap::HeightMap facc = hmap::HeightMap(shape, tiling, overlap);

  hmap::transform(facc,
                  h,
                  [](hmap::Array &f, hmap::Array &z)
                  { return f = hmap::flow_accumulation_dinf(z, 0.1f); });

  hmap::transform(facc, [](hmap::Array &z) { hmap::clamp(z, 0., 40.f); });

  facc.smooth_overlap_buffers();

  h.to_array().to_png("out0.png", hmap::cmap::terrain);
  facc.to_array().to_png("out1.png", hmap::cmap::inferno);
}
