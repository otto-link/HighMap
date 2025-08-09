#include "highmap.hpp"

int main(void)
{
  // for render only
  hmap::Vec2<int>          shape = {256, 256};
  hmap::Array              raster(shape);
  std::vector<hmap::Array> zs = {};
  uint                     seed = 0;

  // density field
  hmap::Vec2<float> kw = {2.f, 2.f};
  hmap::Array density = hmap::noise(hmap::NoiseType::PERLIN, shape, kw, seed);
  hmap::remap(density); // /!\ NEEDS TO BE IN [0, 1]

  // base param
  size_t count = 1000;

  // --- random

  for (auto &type : {hmap::PointSamplingMethod::RND_RANDOM,
                     hmap::PointSamplingMethod::RND_HALTON,
                     hmap::PointSamplingMethod::RND_HAMMERSLEY,
                     hmap::PointSamplingMethod::RND_LHS})
  {
    // auto xy = hmap::random_points(count, seed, type);
    // hmap::Cloud cloud(xy[0], xy[1], 1.f /* value */);

    hmap::Cloud cloud = hmap::random_cloud(count, seed, type);

    cloud.to_array(raster);
    zs.push_back(raster);
  }

  hmap::export_banner_png("ex_point_sampling0.png", zs, hmap::Cmap::BONE);

  // --- density

  {
    zs.clear();
    zs.push_back(density);
    raster = 0.f;

    // auto        xy = hmap::random_points_density(count, density, seed);
    // hmap::Cloud cloud(xy[0], xy[1], 1.f /* value */);

    hmap::Cloud cloud = hmap::random_cloud_density(count, density, seed);

    cloud.to_array(raster);

    zs.push_back(raster);
    hmap::export_banner_png("ex_point_sampling1.png", zs, hmap::Cmap::BONE);
  }

  // --- distance based

  {
    zs.clear();
    zs.push_back(density);

    float min_dist = 0.02f;
    float max_dist = 0.08f;

    // auto        xy = hmap::random_points_distance(min_dist, seed);
    // hmap::Cloud cloud(xy[0], xy[1], 1.f /* value */);

    hmap::Cloud cloud = hmap::random_cloud_distance(min_dist, seed);

    raster = 0.f;
    cloud.to_array(raster);
    zs.push_back(raster);

    // auto        xy = hmap::random_points_distance(min_dist, max_dist,
    // density, seed); cloud = hmap::Cloud(xy[0], xy[1], 1.f /* value */);

    cloud = hmap::random_cloud_distance(min_dist, max_dist, density, seed);

    raster = 0.f;
    cloud.to_array(raster);
    zs.push_back(raster);

    hmap::export_banner_png("ex_point_sampling2.png", zs, hmap::Cmap::BONE);
  }

  // --- grid jittered

  {
    zs.clear();

    hmap::Vec2<float> jitter_amount = {0.3f, 0.3f};
    hmap::Vec2<float> stagger_ratio = {0.5f, 0.f};

    // auto        xy = hmap::random_points_jittered(count,
    //                                        jitter_amount,
    //                                        stagger_ratio,
    //                                        seed);
    // hmap::Cloud cloud(xy[0], xy[1], 1.f /* value */);

    hmap::Cloud cloud = hmap::random_cloud_jittered(count,
                                                    jitter_amount,
                                                    stagger_ratio,
                                                    seed);
    cloud.set_values_from_min_distance();

    raster = 0.f;
    cloud.to_array(raster);
    zs.push_back(raster);

    hmap::export_banner_png("ex_point_sampling3.png", zs, hmap::Cmap::BONE);
  }
}
