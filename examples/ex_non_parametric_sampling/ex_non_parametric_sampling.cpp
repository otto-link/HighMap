#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {64, 64};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 1;

  auto z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);
  hmap::remap(z);

  hmap::Vec2<int> patch_shape = {5, 5};

  // base function
  hmap::Array zq1 = hmap::non_parametric_sampling(z, patch_shape, ++seed, 0.5f);

  hmap::Array zq2 = hmap::non_parametric_sampling(z, patch_shape, ++seed, 0.5f);

  hmap::export_banner_png("ex_non_parametric_sampling.png",
                          {z, zq1, zq2},
                          hmap::Cmap::TERRAIN,
                          true);

  // // wrapper / shuffle
  // hmap::Array zs0 = hmap::quilting_shuffle(z, patch_shape, overlap, ++seed);
  // hmap::Array zs1 = hmap::quilting_shuffle(z, patch_shape, overlap, ++seed);

  // hmap::export_banner_png("ex_quilting1.png",
  //                         {z, zs0, zs1},
  //                         hmap::cmap::terrain,
  //                         true);

  // // wrapper / expand

  // // output array is 2-times larger in this case
  // float expansion_ratio = 2.f;

  // hmap::Array ze0 = hmap::quilting_expand(z,
  //                                         expansion_ratio,
  //                                         patch_shape,
  //                                         overlap,
  //                                         ++seed);

  // ze0.to_png("ex_quilting2.png", hmap::cmap::terrain, true);

  // // keep input shape for the output ('true' parameter)
  // bool keep_input_shape = true;

  // hmap::Array ze1 = hmap::quilting_expand(z,
  //                                         expansion_ratio,
  //                                         patch_shape,
  //                                         overlap,
  //                                         ++seed,
  //                                         keep_input_shape);

  // hmap::Array ze2 = hmap::quilting_expand(z,
  //                                         2.f * expansion_ratio,
  //                                         patch_shape,
  //                                         overlap,
  //                                         ++seed,
  //                                         keep_input_shape);

  // hmap::export_banner_png("ex_quilting3.png",
  //                         {z, ze1, ze2},
  //                         hmap::cmap::terrain,
  //                         true);
}
