#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 2;

  hmap::Vec4<float> bbox = {-1.f, 2.f, 0.f, 5.f};

  // generate a path using a random set of points
  int        npoints = 8;
  hmap::Path path = hmap::Path(npoints, seed, bbox);
  path.reorder_nns(); // reorder points to get a better look

  hmap::Array z1 = hmap::Array(shape);
  path.to_array(z1, bbox);

  // control function (supposed to be in [0, 1])
  hmap::Array z_control = slope(shape, 0.f, -1.f);
  hmap::remap(z_control);

  int   iterations = 6;
  float sigma = 0.3f;
  path.resample_uniform(); // to ensure a "uniform" output

  // fractalize, with and without control function
  hmap::Path pn = path;
  hmap::Path pc = path;

  // pn.closed = true;
  // pn.resample_uniform();

  pn.fractalize(iterations, seed, sigma);
  hmap::Array z3 = hmap::Array(shape);
  pn.to_array(z3, bbox);

  int   orientation = 0;
  float persistence = 1.f;

  // pc.closed = true;
  pc.fractalize(iterations,
                seed,
                sigma,
                orientation,
                persistence,
                &z_control,
                bbox);

  hmap::Array z4 = hmap::Array(shape);
  pc.to_array(z4, bbox);

  hmap::export_banner_png("ex_path_fractalize.png",
                          {z1, z_control, z3, z4},
                          hmap::Cmap::INFERNO);
}
