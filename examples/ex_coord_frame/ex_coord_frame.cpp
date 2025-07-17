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

  // --- tests

  hmap::Vec4<float> bbox1 = frame1.compute_bounding_box();
  hmap::Vec4<float> bbox2 = frame2.compute_bounding_box();

  hmap::Vec4<float> bboxi = intersect_bounding_boxes(bbox1, bbox2);

  LOG_DEBUG("%f %f %f %f", bbox1.a, bbox1.b, bbox1.c, bbox1.d);
  LOG_DEBUG("%f %f %f %f", bboxi.a, bboxi.b, bboxi.c, bboxi.d);

  hmap::Vec2<int> shape2 = {1000, 1000};

  std::vector<float> x = hmap::linspace(-200.f, 200.f, shape2.x);
  std::vector<float> y = hmap::linspace(-200.f, 200.f, shape2.y);

  hmap::Array array(shape2);

  for (int i = 0; i < shape2.x; i++)
    for (int j = 0; j < shape2.y; j++)
    {
      if (frame1.is_point_within(x[i], y[j]))
        array(i, j) = 1.f;
      else if (frame2.is_point_within(x[i], y[j]))
        array(i, j) = 0.75f;
      else if (hmap::is_point_within_bounding_box(x[i], y[j], bboxi))
        array(i, j) = 0.5f;
      else if (hmap::is_point_within_bounding_box(x[i], y[j], bbox1))
        array(i, j) = 0.1f;
      else if (hmap::is_point_within_bounding_box(x[i], y[j], bbox2))
        array(i, j) = 0.05f;
    }

  array.to_png("out.png", hmap::Cmap::JET);

  //
  hmap::Array array_itp(shape2);

  for (int i = 0; i < shape2.x; i++)
    for (int j = 0; j < shape2.y; j++)
    {
      array_itp(i, j) = frame1.get_heightmap_value_nearest(h1, x[i], y[j]);
    }

  LOG_DEBUG("ok");

  array_itp.to_png("out1.png", hmap::Cmap::INFERNO);

  //
  hmap::interpolate_heightmap(h1, h2, frame1, frame2);

  hmap::Array array_itp2(shape2);

  // p_h2->to_array().dump();
  // h.to_array().dump();

  for (int i = 0; i < shape2.x; i++)
    for (int j = 0; j < shape2.y; j++)
    {
      array_itp2(i, j) = frame2.get_heightmap_value_nearest(h2, x[i], y[j]);
    }

  array_itp2.to_png("out2.png", hmap::Cmap::INFERNO);
}
