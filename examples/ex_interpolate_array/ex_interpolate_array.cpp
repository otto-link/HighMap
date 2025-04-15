#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  // const hmap::Vec2<int>   shape = {4096, 4096};
  const hmap::Vec2<int>   shape = {13, 7};
  const hmap::Vec2<float> res = {1.f, 2.f};
  int                     seed = 0;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  z.to_png("ex_interpolate_array0.png", hmap::Cmap::JET);

  // --- nearest

  hmap::Array zn0(hmap::Vec2<int>(256, 256));
  hmap::Array zn1(hmap::Vec2<int>(1024, 256));

  hmap::interpolate_array_nearest(z, zn0);
  hmap::interpolate_array_nearest(z, zn1);

  hmap::Array zn0_gpu(hmap::Vec2<int>(256, 256));
  hmap::Array zn1_gpu(hmap::Vec2<int>(1024, 256));

  hmap::gpu::interpolate_array_nearest(z, zn0_gpu);
  hmap::gpu::interpolate_array_nearest(z, zn1_gpu);

  zn0.to_png("ex_interpolate_array1.png", hmap::Cmap::JET);
  zn1.to_png("ex_interpolate_array2.png", hmap::Cmap::JET);

  zn0_gpu.to_png("ex_interpolate_array3.png", hmap::Cmap::JET);
  zn1_gpu.to_png("ex_interpolate_array4.png", hmap::Cmap::JET);

  // --- bilinear

  hmap::Array zb0(hmap::Vec2<int>(256, 256));
  hmap::Array zb1(hmap::Vec2<int>(1024, 256));

  hmap::interpolate_array_bilinear(z, zb0);
  hmap::interpolate_array_bilinear(z, zb1);

  hmap::Array zb0_gpu(hmap::Vec2<int>(256, 256));
  hmap::Array zb1_gpu(hmap::Vec2<int>(1024, 256));

  hmap::gpu::interpolate_array_bilinear(z, zb0_gpu);
  hmap::gpu::interpolate_array_bilinear(z, zb1_gpu);

  zb0.to_png("ex_interpolate_array5.png", hmap::Cmap::JET);
  zb1.to_png("ex_interpolate_array6.png", hmap::Cmap::JET);

  zb0_gpu.to_png("ex_interpolate_array7.png", hmap::Cmap::JET);
  zb1_gpu.to_png("ex_interpolate_array8.png", hmap::Cmap::JET);

  // --- bicubic

  hmap::Array zc0(hmap::Vec2<int>(256, 256));
  hmap::Array zc1(hmap::Vec2<int>(1024, 256));

  hmap::interpolate_array_bicubic(z, zc0);
  hmap::interpolate_array_bicubic(z, zc1);

  hmap::Array zc0_gpu(hmap::Vec2<int>(256, 256));
  hmap::Array zc1_gpu(hmap::Vec2<int>(1024, 256));

  hmap::gpu::interpolate_array_bicubic(z, zc0_gpu);
  hmap::gpu::interpolate_array_bicubic(z, zc1_gpu);

  zc0.to_png("ex_interpolate_array9.png", hmap::Cmap::JET);
  zc1.to_png("ex_interpolate_array10.png", hmap::Cmap::JET);

  zc0_gpu.to_png("ex_interpolate_array11.png", hmap::Cmap::JET);
  zc1_gpu.to_png("ex_interpolate_array12.png", hmap::Cmap::JET);

  // --- Lagrangian

  hmap::Array zl0_gpu(hmap::Vec2<int>(256, 256));
  hmap::Array zl1_gpu(hmap::Vec2<int>(1024, 256));

  int order = 6;

  hmap::gpu::interpolate_array_lagrange(z, zl0_gpu, order);
  hmap::gpu::interpolate_array_lagrange(z, zl1_gpu, order);

  zl0_gpu.to_png("ex_interpolate_array13.png", hmap::Cmap::JET);
  zl1_gpu.to_png("ex_interpolate_array14.png", hmap::Cmap::JET);

  // --- w/ bounding box

  hmap::Vec4<float> bbox_s(1.f, 2.f, -1.f, 0.f);
  hmap::Vec4<float> bbox_t(1.25f, 1.75f, -0.75f, 0.25f);
  // hmap::Vec4<float> bbox_t(1.25f, 1.75f, -0.75f, -0.25f);

  // nearest
  hmap::Array z_bbox_n(hmap::Vec2<int>(1024, 256));
  hmap::interpolate_array_nearest(z, z_bbox_n, bbox_s, bbox_t);

  hmap::Array z_bbox_n_gpu(hmap::Vec2<int>(1024, 256));
  hmap::gpu::interpolate_array_nearest(z, z_bbox_n_gpu, bbox_s, bbox_t);

  z_bbox_n.to_png("ex_interpolate_array15.png", hmap::Cmap::JET);
  z_bbox_n_gpu.to_png("ex_interpolate_array16.png", hmap::Cmap::JET);

  // linear
  hmap::Array z_bbox_b(hmap::Vec2<int>(1024, 256));
  hmap::interpolate_array_bilinear(z, z_bbox_b, bbox_s, bbox_t);

  hmap::Array z_bbox_b_gpu(hmap::Vec2<int>(1024, 256));
  hmap::gpu::interpolate_array_bilinear(z, z_bbox_b_gpu, bbox_s, bbox_t);

  z_bbox_b.to_png("ex_interpolate_array17.png", hmap::Cmap::JET);
  z_bbox_b_gpu.to_png("ex_interpolate_array18.png", hmap::Cmap::JET);

  // cubic
  hmap::Array z_bbox_c(hmap::Vec2<int>(1024, 256));
  hmap::interpolate_array_bicubic(z, z_bbox_c, bbox_s, bbox_t);

  hmap::Array z_bbox_c_gpu(hmap::Vec2<int>(1024, 256));
  hmap::gpu::interpolate_array_bicubic(z, z_bbox_c_gpu, bbox_s, bbox_t);

  z_bbox_c.to_png("ex_interpolate_array19.png", hmap::Cmap::JET);
  z_bbox_c_gpu.to_png("ex_interpolate_array20.png", hmap::Cmap::JET);
}
