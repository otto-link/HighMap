#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {512, 512};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  auto z = hmap::noise_fbm(hmap::NoiseType::n_perlin, shape, kw, seed);

  float           ratio = 0.25f;
  hmap::Vec2<int> patch_shape = {(int)(ratio * shape.x),
                                 (int)(ratio * shape.y)};
  hmap::Vec2<int> tiling = {4, 4};
  float           overlap = 0.9f;

  // base function
  hmap::Array zq = hmap::quilting(z, patch_shape, tiling, overlap, ++seed);

  zq.to_png("ex_quilting0.png", hmap::cmap::magma);

  // wrapper / shuffle
  hmap::Array zs0 = hmap::quilting_shuffle(z, patch_shape, overlap, ++seed);
  hmap::Array zs1 = hmap::quilting_shuffle(z, patch_shape, overlap, ++seed);

  hmap::export_banner_png("ex_quilting1.png",
                          {z, zs0, zs1},
                          hmap::cmap::terrain,
                          true);

  // wrapper / expand

  // output array is 2-times larger in this case
  float expansion_ratio = 2.f;

  hmap::Array ze0 = hmap::quilting_expand(z,
                                          expansion_ratio,
                                          patch_shape,
                                          overlap,
                                          ++seed);

  ze0.to_png("ex_quilting2.png", hmap::cmap::terrain, true);

  // keep input shape for the output ('true' parameter)
  bool keep_input_shape = true;

  hmap::Array ze1 = hmap::quilting_expand(z,
                                          expansion_ratio,
                                          patch_shape,
                                          overlap,
                                          ++seed,
                                          keep_input_shape);

  hmap::Array ze2 = hmap::quilting_expand(z,
                                          2.f * expansion_ratio,
                                          patch_shape,
                                          overlap,
                                          ++seed,
                                          keep_input_shape);

  hmap::export_banner_png("ex_quilting3.png",
                          {z, ze1, ze2},
                          hmap::cmap::terrain,
                          true);
}
