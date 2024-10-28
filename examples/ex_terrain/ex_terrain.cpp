#include "highmap.hpp"

#include "highmap/terrain/terrain.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<int>   tiling = {4, 2};
  float             overlap = 0.25;
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;
  hmap::HeightMap   h = hmap::HeightMap(shape, tiling, overlap);

  hmap::fill(h,
             [&kw, &seed](hmap::Vec2<int> shape, hmap::Vec4<float> bbox)
             {
               return hmap::noise(hmap::NoiseType::PERLIN,
                                  shape,
                                  kw,
                                  seed,
                                  nullptr,
                                  nullptr,
                                  nullptr,
                                  bbox);
             });

  h.remap();

  // ---

  hmap::Terrain terrain1 = hmap::Terrain(hmap::Vec2<float>(10.f, 20.f),
                                         hmap::Vec2<float>(50.f, 100.f),
                                         -1.f,
                                         2.f,
                                         30.f,
                                         0);

  std::string id1 = terrain1.add_heightmap(h);

  hmap::Terrain terrain2 = hmap::Terrain(hmap::Vec2<float>(-20.f, 50.f),
                                         hmap::Vec2<float>(100.f, 70.f),
                                         -1.f,
                                         2.f,
                                         -30.f,
                                         1);

  hmap::HeightMap h2 = hmap::HeightMap({512, 256}, {2, 4}, 0.5f);
  std::string     id2 = terrain2.add_heightmap(h2);

  hmap::Vec4<float> bbox1 = terrain1.compute_bounding_box();
  hmap::Vec4<float> bbox2 = terrain2.compute_bounding_box();

  hmap::Vec4<float> bboxi = intersect_bounding_boxes(bbox1, bbox2);

  LOG_DEBUG("%f %f %f %f", bboxi.a, bboxi.b, bboxi.c, bboxi.d);

  hmap::Vec2<int> shape2 = {1000, 1000};

  std::vector<float> x = hmap::linspace(-200.f, 200.f, shape2.x);
  std::vector<float> y = hmap::linspace(-200.f, 200.f, shape2.y);

  hmap::Array array(shape2);

  for (int i = 0; i < shape2.x; i++)
    for (int j = 0; j < shape2.y; j++)
    {
      if (terrain1.is_point_within(x[i], y[j]))
        array(i, j) = 1.f;
      else if (terrain2.is_point_within(x[i], y[j]))
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

  hmap::HeightMap *p_h = terrain1.get_heightmap_ref(id1);

  for (int i = 0; i < shape2.x; i++)
    for (int j = 0; j < shape2.y; j++)
    {
      array_itp(i, j) = terrain1.get_heightmap_value_nearest(*p_h, x[i], y[j]);
    }

  array_itp.to_png("out1.png", hmap::Cmap::INFERNO);

  //
  hmap::interpolate_terrain_heightmap(terrain1, id1, terrain2, id2);

  hmap::Array      array_itp2(shape2);
  hmap::HeightMap *p_h2 = terrain2.get_heightmap_ref(id2);

  for (int i = 0; i < shape2.x; i++)
    for (int j = 0; j < shape2.y; j++)
    {
      array_itp2(i,
                 j) = terrain2.get_heightmap_value_nearest(*p_h2, x[i], y[j]);
    }

  array_itp2.to_png("out2.png", hmap::Cmap::INFERNO);
}
