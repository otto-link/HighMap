#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {1024, 1024};
  const std::vector<float> res = {3.f, 3.f};
  int                      seed = 3;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  remap(z, 0.1f, 1.f);
  z = z * hmap::biweight(shape);
  remap(z);

  hmap::Array z_bedrock = hmap::Array(shape);

  for (int k = 0; k < 4; k++)
  {
    z_bedrock = hmap::minimum_local(z, 11);
    hmap::hydraulic_stream(z, z_bedrock, 0.005f, 10.f / shape[0]);

    float nparticles = (int)(0.1f * z.shape[0] * z.shape[1]);
    hmap::hydraulic_particle(z, z, nparticles, seed, 0, 20., 0.1, 0.5, 0.01);
  }

  z.to_png("ug_combined_erosions.png", hmap::cmap::terrain, true);
}
