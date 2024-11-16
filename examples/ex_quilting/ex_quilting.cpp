#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {512, 512};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  auto z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);
  auto zw = hmap::noise_fbm(hmap::NoiseType::WORLEY, shape, kw, ++seed);

  hmap::remap(z);
  hmap::remap(zw);

  float           ratio = 0.25f;
  hmap::Vec2<int> patch_shape = {(int)(ratio * shape.x),
                                 (int)(ratio * shape.y)};
  hmap::Vec2<int> tiling = {4, 4};
  float           overlap = 0.9f;

  // --- base function, a list of array can be provided as a source of
  // --- patches. Secondary arrays can be provided, quilting will
  // --- apply to them based on the input arrays.
  std::vector<hmap::Array *> input_p_arrays = {&z};

  auto zm = 1.f - z;
  auto dz = hmap::gradient_norm(z);
  hmap::remap(dz);
  std::vector<hmap::Array *> sec_arrays = {&zm, &dz}; // will be
                                                      // overriden

  hmap::Array zq = hmap::quilting({&z},
                                  patch_shape,
                                  tiling,
                                  overlap,
                                  ++seed,
                                  sec_arrays);

  hmap::export_banner_png("ex_quilting0.png",
                          {zq, zm, dz},
                          hmap::Cmap::MAGMA,
                          true);

  // --- wrapper / shuffle
  hmap::Array zs0 = hmap::quilting_shuffle(z, patch_shape, overlap, ++seed);

  // with secondary arrays
  dz = hmap::gradient_norm(z);
  hmap::remap(dz);
  sec_arrays = {&dz};

  hmap::Array zs1 = hmap::quilting_shuffle(z,
                                           patch_shape,
                                           overlap,
                                           ++seed,
                                           sec_arrays);

  hmap::export_banner_png("ex_quilting1.png",
                          {z, zs0, zs1, dz},
                          hmap::Cmap::TERRAIN,
                          true);

  // --- wrapper / expand

  // output array is 2-times larger in this case
  float expansion_ratio = 2.f;

  // with secondary arrays
  dz = hmap::gradient_norm(z);
  hmap::remap(dz);
  sec_arrays = {&dz};

  hmap::Array ze0 = hmap::quilting_expand(z,
                                          expansion_ratio,
                                          patch_shape,
                                          overlap,
                                          ++seed,
                                          sec_arrays,
                                          true);

  hmap::export_banner_png("ex_quilting2.png",
                          {ze0, dz},
                          hmap::Cmap::TERRAIN,
                          true);

  // keep input shape for the output ('true' parameter)
  bool keep_input_shape = true;

  hmap::Array ze1 = hmap::quilting_expand(z,
                                          expansion_ratio,
                                          patch_shape,
                                          overlap,
                                          ++seed,
                                          {}, // no secondary arrays
                                          keep_input_shape);

  hmap::Array ze2 = hmap::quilting_expand(z,
                                          2.f * expansion_ratio,
                                          patch_shape,
                                          overlap,
                                          ++seed,
                                          {},
                                          keep_input_shape);

  hmap::export_banner_png("ex_quilting3.png",
                          {z, ze1, ze2},
                          hmap::Cmap::TERRAIN,
                          true);

  // wrapper / blend
  hmap::Array zb = hmap::quilting_blend({&z, &zw},
                                        patch_shape,
                                        overlap,
                                        ++seed);

  hmap::export_banner_png("ex_quilting4.png",
                          {z, zw, zb},
                          hmap::Cmap::TERRAIN,
                          true);
}
