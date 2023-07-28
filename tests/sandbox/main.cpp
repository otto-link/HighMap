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
  // tiling = {1, 1};
  hmap::HeightMap h = hmap::HeightMap(shape, tiling);
  hmap::HeightMap h2 = hmap::HeightMap(shape, tiling);

  auto noise = hmap::HeightMap(shape, tiling);
  hmap::fill(
      noise,
      [&noise, &res, &seed](std::vector<int> shape, std::vector<float> shift)
      {
        return hmap::fbm_perlin(shape,
                                res,
                                seed,
                                8,
                                0.7f,
                                0.5f,
                                2.f,
                                nullptr,
                                shift,
                                noise.tile_scale);
      });

  h.infos();

  timer.start("fill");
  hmap::fill(h,
             [&h, &res, &seed](std::vector<int> shape, std::vector<float> shift)
             {
               return hmap::ridged_perlin(shape,
                                          res,
                                          seed,
                                          8,
                                          0.7f,
                                          0.5f,
                                          2.f,
                                          nullptr,
                                          shift,
                                          h.tile_scale);
             });

  hmap::fill(h,
             noise,
             [&h, &res, &seed](std::vector<int>   shape,
                               std::vector<float> shift,
                               hmap::Array       *p_noise)
             {
               return hmap::ridged_perlin(shape,
                                          res,
                                          seed,
                                          8,
                                          0.7f,
                                          0.5f,
                                          2.f,
                                          p_noise,
                                          shift,
                                          h.tile_scale);
             });
  timer.stop("fill");

  // hmap::fill(
  //     h,
  //     [&h](std::vector<int> shape, std::vector<float> shift) {
  //       return hmap::gaussian_pulse(shape, 16, nullptr, shift, h.tile_scale);
  //     });

  // hmap::fill(
  //     h,
  //     noise,
  //     [&h](std::vector<int>   shape,
  //          std::vector<float> shift,
  //          hmap::Array       *p_noise) {
  //       return hmap::gaussian_pulse(shape, 64, p_noise, shift, h.tile_scale);
  //     });

  // peak
  if (false)
  {
    hmap::fill(h,
               noise,
               [&h](std::vector<int>   shape,
                    std::vector<float> shift,
                    hmap::Array       *p_noise) {
                 return hmap::peak(shape,
                                   48.f,
                                   p_noise,
                                   16.f,
                                   0.4f,
                                   shift,
                                   h.tile_scale);
               });
  }

  // crater
  if (false)
  {
    hmap::fill(h,
               noise,
               [&h](std::vector<int>   shape,
                    std::vector<float> shift,
                    hmap::Array       *p_noise)
               {
                 return hmap::crater(shape,
                                     48.f,
                                     0.5f,
                                     32.f,
                                     0.3f,
                                     p_noise,
                                     shift,
                                     h.tile_scale);
               });
  }

  // caldera
  if (false)
  {
    hmap::fill(h,
               noise,
               [&h](std::vector<int>   shape,
                    std::vector<float> shift,
                    hmap::Array       *p_noise)
               {
                 return hmap::caldera(shape,
                                      32.f,
                                      8.f,
                                      32.f,
                                      0.3f,
                                      p_noise,
                                      8.f,
                                      0.4f,
                                      shift,
                                      h.tile_scale);
               });
  }

  // Worley
  if (false)
  {
    hmap::fill(h,
               [&h](std::vector<int> shape, std::vector<float> shift)
               {
                 return hmap::value_noise_linear(shape,
                                                 {4.f, 4.f},
                                                 1,
                                                 nullptr,
                                                 shift,
                                                 h.tile_scale);
               });

    hmap::fill(h,
               noise,
               [&h](std::vector<int>   shape,
                    std::vector<float> shift,
                    hmap::Array       *p_noise)
               {
                 return hmap::value_noise_linear(shape,
                                                 {4.f, 4.f},
                                                 1,
                                                 p_noise,
                                                 shift,
                                                 h.tile_scale);
               });
  }

  // wave triangular
  if (false)
  {
    hmap::fill(h,
               [&h](std::vector<int> shape, std::vector<float> shift)
               {
                 return hmap::wave_triangular(shape,
                                              4.f,
                                              15.f,
                                              0.1f,
                                              nullptr,
                                              shift,
                                              h.tile_scale);
               });

    hmap::fill(h,
               noise,
               [&h](std::vector<int>   shape,
                    std::vector<float> shift,
                    hmap::Array       *p_noise)
               {
                 return hmap::wave_triangular(shape,
                                              4.f,
                                              15.f,
                                              0.1f,
                                              p_noise,
                                              shift,
                                              h.tile_scale);
               });
  }

  // step
  if (false)
  {
    hmap::fill(h,
               [&h](std::vector<int> shape, std::vector<float> shift) {
                 return hmap::step(shape,
                                   15.f,
                                   6.f / 512.f,
                                   nullptr,
                                   shift,
                                   h.tile_scale);
               });

    hmap::fill(h,
               noise,
               [&h](std::vector<int>   shape,
                    std::vector<float> shift,
                    hmap::Array       *p_noise) {
                 return hmap::step(shape,
                                   15.f,
                                   6.f / 512.f,
                                   p_noise,
                                   shift,
                                   h.tile_scale);
               });
  }

  // slope_x
  if (false)
  {
    hmap::fill(h,
               [&h](std::vector<int> shape, std::vector<float> shift) {
                 return hmap::slope_y(shape,
                                      6.f / 512.f,
                                      nullptr,
                                      shift,
                                      h.tile_scale);
               });

    hmap::fill(h,
               noise,
               [&h](std::vector<int>   shape,
                    std::vector<float> shift,
                    hmap::Array       *p_noise) {
                 return hmap::slope_y(shape,
                                      6.f / 512.f,
                                      p_noise,
                                      shift,
                                      h.tile_scale);
               });
  }

  // Perlin
  if (false)
  {
    hmap::fill(h,
               [&h](std::vector<int> shape, std::vector<float> shift)
               {
                 return hmap::perlin_mix(shape,
                                         {4.f, 4.f},
                                         1,
                                         nullptr,
                                         shift,
                                         h.tile_scale);
               });

    hmap::fill(h,
               noise,
               [&h](std::vector<int>   shape,
                    std::vector<float> shift,
                    hmap::Array       *p_noise)
               {
                 return hmap::perlin_mix(shape,
                                         {4.f, 4.f},
                                         1,
                                         p_noise,
                                         shift,
                                         h.tile_scale);
               });
  }

  h.infos();

  hmap::fill(h2,
             [](std::vector<int> shape) { return hmap::constant(shape, 0.f); });

  h.remap();

  // timer.start("transform");
  // hmap::transform(h, [](hmap::Array &x) { hmap::gamma_correction(x, 2.f); });
  // timer.stop("transform");

  // hmap::transform(h,
  //                 h2,
  //                 [](hmap::Array &x, hmap::Array &y)
  //                 { hmap::clamp_min(x, y); });

  // timer.start("transform seq");
  // hmap::gamma_correction(z, 2.f);
  // timer.stop("transform seq");

  // z.to_png("out.png", hmap::cmap::inferno);

  h.infos();

  timer.start("preview");
  h.to_array().to_png("out_h.png", hmap::cmap::magma);
  timer.stop("preview");
}
