#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 1;

  hmap::Vec4<float> bbox = {-1.f, 0.f, 0.5f, 1.5f};

  hmap::Cloud cloud = hmap::Cloud(10, seed, bbox);

  cloud.print();

  hmap::Array z0 = hmap::Array(shape);
  cloud.to_array(z0, bbox);

  std::vector<int> idx = cloud.get_convex_hull_point_indices();

  // create a polyline to display the chull
  std::vector<float> x, y, v;

  for (int k : idx)
  {
    x.push_back(cloud.points[k].x);
    y.push_back(cloud.points[k].y);
    v.push_back(cloud.points[k].v);
  }
  hmap::Path path_chull = hmap::Path(x, y, v);
  path_chull.closed = true;

  hmap::Array z1 = z0;
  path_chull.to_array(z1, bbox);

  hmap::export_banner_png("ex_cloud_get_convex_hull.png",
                          {z0, z1},
                          hmap::Cmap::INFERNO);
}
