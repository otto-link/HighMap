#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::remap(z);

  auto z1 = z;
  {
    int   ir = 16;
    float gamma = 2.f;
    hmap::recast_peak(z1, ir, gamma);
  }

  auto z2 = z;
  {
    hmap::Array noise = hmap::fbm_perlin(shape, res, seed++);
    hmap::remap(noise, -0.1f, 0.1f);

    float zcut = 0.5f;
    float gamma = 4.f;
    hmap::recast_canyon(z2, zcut, gamma, &noise);
  }

  auto z3 = z;
  {
    float talus = 2.f / shape.x;
    int   ir = 4;
    float amplitude = 0.2f;
    float kw = 16.f;
    float gamma = 0.5f;
    hmap::recast_rocky_slopes(z3, talus, ir, amplitude, seed, kw, gamma);
  }

  hmap::export_banner_png("ex_recast.png",
                          {z, z1, z2, z3},
                          hmap::cmap::terrain,
                          true);
}
