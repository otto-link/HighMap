#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);

  int                        nlevels = 4;
  hmap::PyramidDecomposition pyr = hmap::PyramidDecomposition(z, nlevels);

  pyr.decompose();

  {
    // some intermediate outputs for checking
    for (int n = 0; n < pyr.nlevels; n++)
      pyr.components[n].to_png("components_" + std::to_string(n) + ".png",
                               hmap::cmap::jet);

    pyr.residual.to_png("components_res.png", hmap::cmap::jet);

    z.to_png("components_initial.png", hmap::cmap::jet);
  }

  hmap::Array zr = pyr.reconstruct();
  zr.to_png("components_rebuild.png", hmap::cmap::jet, false);

  pyr.to_png("ex_pyramid_decomposition0.png", hmap::cmap::magma);

  hmap::export_banner_png("ex_pyramid_decomposition1.png",
                          {z, zr},
                          hmap::cmap::inferno);
}
