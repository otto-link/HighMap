#include "highmap/array.hpp"
<<<<<<< HEAD
#include "highmap/colormaps.hpp"
#include "highmap/erosion.hpp"
#include "highmap/geometry.hpp"
#include "highmap/hydrology.hpp"
=======
>>>>>>> 8d392f8 (Clean sandbox example for the main branch)
#include "highmap/io.hpp"
#include "highmap/primitives.hpp"

int main(void)
{

  const std::vector<int>   shape = {512, 512};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 2;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed, 8);

<<<<<<< HEAD
  // for (int i = 0; i < z.shape[0]; i++)
  //   for (int j = 0; j < z.shape[1]; j++)
  //   {
  //     float x = (float)i / (z.shape[0] - 1);
  //     float y = (float)j / (z.shape[1] - 1);
  //     float r2 = (x - 0.5f) * (x - 0.5f) + (y - 0.5f) * (y - 0.5f);
  //     z(i, j) = std::exp(-r2 / 2 / 0.1f / 0.1f);
  //   }

  z += 0.5f * hmap::cubic_pulse(shape);

  z.infos();

  hmap::set_borders(z, z.min(), shape[0] / 2);
  hmap::smooth_fill(z, shape[0] / 4);

  // hmap::remap(z);
  // hmap::steepen_convective(z, 0.f, 20, 8);

  hmap::remap(z);
  // hmap::hydraulic_ridge(z, 10.f / shape[0], 0.15f);

  // hmap::Array z_bedrock = hmap::minimum_local(z, 8);
  // hmap::hydraulic_stream(z, z_bedrock, 0.05f, 0.001f / shape[0]);

  // hmap::remap(z);
  // hmap::thermal_scree(z, 3.f / shape[0], seed, 0.5f, -1.f, 0.3f);

  hmap::hydraulic_particle(z, 100000, 1);

  auto z0 = z;

  if (false)
  {
    z = 0.f;
    std::vector<float> bbox = {1.f, 2.f, -0.5f, 0.5f};

    // hmap::Cloud cloud = hmap::Cloud(10, seed, {1.1f, 1.9f, -0.4, 0.4f});

    hmap::Path path = hmap::Path(4, ++seed, {1.3f, 1.7f, -0.2, 0.2f});
    path.reorder_nns();
    path.closed = true;
    path.closed = false;

    path.to_array(z, bbox);

    path.to_png("path0.png");

    path.bezier(0.5f, 40);
    path.to_png("path1.png");

    path.to_array(z, bbox);

    path.meanderize(0.2f, 0.2f, 1);

    path.to_png("path2.png");

    path.to_array(z, bbox);
  }

  //   std::vector<uint32_t> data = {
  // #include "tmp"
  //   };

  // hmap::Clut3D clut = hmap::Clut3D({32, 32, 32}, data);
  hmap::Clut3D clut = hmap::Clut3D({32, 32, 32}, "sahara.bin");

  timer.start("colorize");
  auto img = colorize_trivariate(z,
                                 hmap::gradient_x(z),
                                 hmap::gradient_y(z),
                                 clut,
                                 true);
  timer.stop("colorize");

  hmap::write_png_8bit("clut.png", img, shape);

  z.to_png("out.png", hmap::cmap::gray, false);

  z0.to_png("out0.png", hmap::cmap::terrain, true);
  z.to_file("out.bin");
=======
  z.to_png("out.png", hmap::cmap::gray);
>>>>>>> 8d392f8 (Clean sandbox example for the main branch)
}
