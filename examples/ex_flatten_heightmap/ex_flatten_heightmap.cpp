#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {64, 32};
  shape = {512, 256};
  hmap::Vec2<int>   tiling = {4, 2};
  float             overlap = 0.25f;
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  // --- 1st frame

  hmap::CoordFrame frame1 = hmap::CoordFrame(hmap::Vec2<float>(10.f, 20.f),
                                             hmap::Vec2<float>(50.f, 100.f),
                                             30.f);

  hmap::Heightmap h1 = hmap::Heightmap(shape, tiling, overlap);

  hmap::transform(
      {&h1},
      [kw, seed](std::vector<hmap::Array *> p_arrays,
                 hmap::Vec2<int>            shape,
                 hmap::Vec4<float>          bbox)
      {
        hmap::Array *pa_out = p_arrays[0];

        *pa_out = hmap::noise(hmap::NoiseType::PERLIN,
                              shape,
                              kw,
                              seed,
                              nullptr,
                              nullptr,
                              nullptr,
                              bbox);
      },
      hmap::TransformMode::DISTRIBUTED);

  h1.remap();

  // --- second frame

  hmap::CoordFrame frame2 = hmap::CoordFrame(hmap::Vec2<float>(-20.f, 50.f),
                                             hmap::Vec2<float>(100.f, 70.f),
                                             -30.f);

  hmap::Heightmap h2 = hmap::Heightmap({512, 256}, {2, 4}, 0.5f);

  hmap::interpolate_heightmap(h1, h2, frame1, frame2);

  hmap::transform(
      {&h2},
      [](std::vector<hmap::Array *> p_arrays)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::gain(*pa_out, 8.f);
      },
      hmap::TransformMode::DISTRIBUTED);

  // --- third frame

  hmap::CoordFrame frame3 = hmap::CoordFrame(hmap::Vec2<float>(10.f, 20.f),
                                             hmap::Vec2<float>(50.f, 50.f),
                                             45.f);

  hmap::Heightmap h3 = hmap::Heightmap({512, 256}, {2, 4}, 0.5f);

  hmap::interpolate_heightmap(h1, h3, frame1, frame3);

  hmap::transform(
      {&h3},
      [](std::vector<hmap::Array *> p_arrays)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::plateau(*pa_out, 32, 4.f);
      },
      hmap::TransformMode::DISTRIBUTED);

  h3.smooth_overlap_buffers();

  // --- flatten

  hmap::Heightmap hf = hmap::Heightmap(hmap::Vec2<int>(1024, 512),
                                       hmap::Vec2<int>(4, 4),
                                       0.5f);

  hmap::flatten_heightmap(h1, h2, hf, frame1, frame2, frame1);

  hmap::Heightmap hg = hmap::Heightmap(hmap::Vec2<int>(1024, 512),
                                       hmap::Vec2<int>(4, 4),
                                       0.5f);

  hmap::flatten_heightmap({&h1, &h2, &h3},
                          hg,
                          {&frame1, &frame2, &frame3},
                          frame1);

  h1.to_array().to_png("out_h1.png", hmap::Cmap::JET);
  h2.to_array().to_png("out_h2.png", hmap::Cmap::JET);
  h3.to_array().to_png("out_h3.png", hmap::Cmap::JET);

  hf.to_array().to_png("out_hf.png", hmap::Cmap::JET);
  hg.to_array().to_png("out_hg.png", hmap::Cmap::JET);
}
