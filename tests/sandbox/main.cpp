#include "highmap.hpp"

int main(void)
{
  // hmap::Vec2<int> shape = {1024, 1024};
  hmap::Vec2<int>   shape = {512, 512};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, kw, seed, 9, 0.5f);
  hmap::remap(z);

  if (true)
  {
    auto z0 = z;

    hmap::hydraulic_particle(z, 100000, seed, 0, 10.f);

    auto z_deposition = z - z0;
    hmap::clamp_min(z_deposition, 0.f);

    auto z_erosion = z0 - z;
    hmap::clamp_min(z_erosion, 0.f);

    z0.to_png("out0.png", hmap::cmap::terrain, true);
    z.to_png("out1.png", hmap::cmap::terrain, true);

    z_deposition.to_png("out_d.png", hmap::cmap::inferno);
    z_erosion.to_png("out_e.png", hmap::cmap::inferno);
  }

  if (false)
  {

    hmap::Vec4<float> bbox = hmap::Vec4(0.f, 1.f, 0.f, 1.f);

    hmap::Path path = hmap::Path({0.05f, 0.85f}, {0.05f, 0.5f});
    path.set_values_from_array(z, bbox);

    path.print();

    path.dijkstra(z, bbox, 0, 0.5f, 2.f);
    path.subsample(40);
    path.fractalize(1, seed);
    path.bezier();
    path.meanderize(0.05f, 0.5f, 1);
    path.set_values_from_array(z, bbox);
    // path.print();
    path.to_png("out_p.png");

    hmap::Array zp = hmap::Array(shape);
    path.to_array(zp, bbox);
    hmap::make_binary(zp);
    // zp = hmap::maximum_local(zp, 1);

    // zp = hmap::maximum_local(zp, 1);

    hmap::fill_talus(zp, 20.f / shape.x, seed, 0.5f);

    // auto mask = zp;
    // // hmap::make_binary(mask);
    // hmap::distance_transform(mask);
    // mask.to_png("out_mask.png", hmap::cmap::gray);

    hmap::gamma_correction(zp, 4.f);

    zp *= 0.1f;

    // hmap::fill_talus_fast(zp, hmap::Vec2(128, 128), 1.f / shape.x, seed,
    // 0.5f);

    zp.to_png("out_f.png", hmap::cmap::magma);

    zp.infos();

    z -= zp;

    // hmap::smooth_fill_holes(z, 3, &mask);

    // hmap::Array z = hmap::Array(shape);
    // graph.to_array(z, bbox);

    // // hmap::fill_talus(z, 4.f / shape.x, seed, 0.f);
    // hmap::fill_talus_fast(z, hmap::Vec2(256, 256), 4.f / shape.x, seed,
    // 0.5f);

    z.to_png("out.png", hmap::cmap::terrain, true);
    z.to_file("out.bin");
  }

  if (false)
  {
    auto nx = 0.2f * hmap::fbm_perlin(shape, kw, seed++, 12, 0.f);
    auto ny = 0.2f * hmap::fbm_perlin(shape, kw, seed++, 12, 0.f);
    z = hmap::value_noise_delaunay(shape, 2.f, seed++, &nx, &ny);

    z = hmap::fbm_perlin(shape, {2.f, 2.f}, seed++, 8, 0.7f);

    hmap::remap(z);
    // z *= 1.f - hmap::gaussian_pulse(shape, 256.f, &nx);
    z *= hmap::gaussian_pulse(shape, 256.f, &nx);
    // hmap::gamma_correction(z, 2.f);

    hmap::hydraulic_ridge(z, 10.f / shape.x, 0.5f, 1.5f, 1.f, 0.1f, 8, seed);

    // hmap::Array z_bedrock = hmap::minimum_local(z, 8);
    // hmap::hydraulic_stream(z, z_bedrock, 0.005f, 20.f / shape.x);

    z.to_png("hmap.png", hmap::cmap::gray);
    z.to_png("hmap_h.png", hmap::cmap::terrain, true);
    z.to_file("out.bin");

    //
    hmap::Array  dzx = hmap::gradient_x(z);
    hmap::Array  dzy = hmap::gradient_y(z);
    hmap::Clut3D clut = hmap::Clut3D({32, 32, 32}, "greenland.bin");
    auto         img = hmap::colorize_trivariate(z, dzx, dzy, clut, true);
    hmap::write_png_8bit("hmap_c.png", img, shape);
  }

  if (false)
  {
    auto w = hmap::fbm_perlin(shape, kw, seed, 9, 0.7f);
    hmap::remap(w);

    auto mask = hmap::select_gt(w, 0.5f);
    mask = hmap::distance_transform(1.f - mask);
    hmap::remap(mask);
    // mask = hmap::pow(mask, 0.5f);

    mask.infos("mask w");

    // hmap::expand(w, 128, &mask);
    // hmap::smooth_fill_holes(z, 32, &mask);
    // hmap::gain(w, 8.f, &mask);
    // hmap::gamma_correction(w, 2.f, &mask);
    // hmap::gamma_correction_local(w, 0.1f, 8, &mask);
    // hmap::laplace(w, &mask);
    // hmap::laplace_edge_preserving(w, 10.f / shape.x, &mask);

    hmap::hydraulic_ridge(w, 16.f / shape.x, &mask);

    w.infos();

    // hmap::recast_canyon(w, 0.7f, &mask);
    // hmap::recast_peak(w, 32, &mask);
    // hmap::recast_rocky_slopes(w, 2.f / shape.x, 4, 0.2, seed, 16.f, 0.5f,
    // &mask);

    w.infos();

    mask.to_png("w_mask.png", hmap::cmap::gray);
    w.to_png("w.png", hmap::cmap::magma);
    w.to_file("out.bin");
  }

  if (false)
  {
    hmap::HeightMap h1 = hmap::HeightMap({512, 512}, {4, 2}, 0.25f);
    hmap::HeightMap h2 = hmap::HeightMap({512, 512}, {4, 2}, 0.25f);

    hmap::fill(h1,
               [](hmap::Vec2<int>   shape,
                  hmap::Vec2<float> shift,
                  hmap::Vec2<float> scale)
               {
                 return hmap::fbm_perlin(shape,
                                         {4.f, 4.f},
                                         1,
                                         8,
                                         0.7f,
                                         0.5f,
                                         2.f,
                                         nullptr,
                                         nullptr,
                                         shift,
                                         scale);
               });

    hmap::fill(h2,
               [](hmap::Vec2<int>   shape,
                  hmap::Vec2<float> shift,
                  hmap::Vec2<float> scale)
               {
                 return hmap::fbm_perlin(shape,
                                         {4.f, 4.f},
                                         2,
                                         8,
                                         0.7f,
                                         0.5f,
                                         2.f,
                                         nullptr,
                                         nullptr,
                                         shift,
                                         scale);
               });

    h1.to_array().to_png("h1.png", hmap::cmap::inferno);
    h2.to_array().to_png("h2.png", hmap::cmap::inferno);

    hmap::HeightMap hk = hmap::HeightMap({512, 512}, {4, 2}, 0.25f);

    // hmap::transform(hk,
    //                 h1,
    //                 h2,
    //                 [](hmap::Array a1, hmap::Array a2)
    //                 { return hmap::kmeans_clustering2(a1, a2, 5, 1); });

    hmap::transform(hk, h1, [](hmap::Array a) { return hmap::gradient_x(a); });

    hk.to_array().to_png("hk.png", hmap::cmap::inferno);
  }
}
