#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  const std::vector<int> mesh_sizes = {128, 256, 512}; // , 1024, 2048, 4096};
  const std::vector<int> kernel_irs = {8, 16, 32, 64, 128};

  const glm::vec2 kw = {4.f, 4.f};
  const int       seed = 1;

  std::cout << "==============================================================="
               "=========\n";
  std::cout << " Benchmark: FFT 2D Convolution vs GPU Separable Filter "
               "(smooth_cpulse)\n";
  std::cout << "==============================================================="
               "=========\n\n";

  std::cout << std::left << std::setw(12) << "Mesh Size" << std::setw(10)
            << "Radius" << std::setw(16) << "Kernel Size" << std::setw(16)
            << "FFT (ms)" << std::setw(16) << "GPU (ms)" << std::setw(12)
            << "Speedup"
            << "\n";
  std::cout << std::string(82, '-') << "\n";

  for (int n : mesh_sizes)
  {
    glm::ivec2  shape = {n, n};
    hmap::Array z0 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);

    // Warm-up GPU once per mesh size
    hmap::Array z_warm = z0;
    hmap::gpu::smooth_cpulse(z_warm, 8);

    for (int ir : kernel_irs)
    {
      glm::ivec2  shape_kernel = {2 * ir + 1, 2 * ir + 1};
      hmap::Array kernel = hmap::get_kernel(shape_kernel,
                                            hmap::KernelType::CUBIC_PULSE);
      kernel.normalize();

      // --- Benchmark FFT 2D convolution
      auto  t0_fft = std::chrono::high_resolution_clock::now();
      auto  zf = hmap::convolve2d_fft(z0, kernel);
      auto  t1_fft = std::chrono::high_resolution_clock::now();
      float dt_fft_ms =
          std::chrono::duration<float, std::milli>(t1_fft - t0_fft).count();

      // --- Benchmark GPU separable filter
      auto zr = z0;
      auto t0_gpu = std::chrono::high_resolution_clock::now();
      hmap::gpu::smooth_cpulse(zr, ir);
      auto  t1_gpu = std::chrono::high_resolution_clock::now();
      float dt_gpu_ms =
          std::chrono::duration<float, std::milli>(t1_gpu - t0_gpu).count();

      float speedup = (dt_gpu_ms > 0.f) ? (dt_fft_ms / dt_gpu_ms) : 0.f;

      std::string mesh_str = std::to_string(n) + "x" + std::to_string(n);
      std::string kernel_str = std::to_string(shape_kernel.x) + "x" +
                               std::to_string(shape_kernel.y);

      std::cout << std::left << std::setw(12) << mesh_str << std::setw(10) << ir
                << std::setw(16) << kernel_str << std::fixed
                << std::setprecision(2) << std::setw(16) << dt_fft_ms
                << std::setw(16) << dt_gpu_ms << std::fixed
                << std::setprecision(2) << std::setw(12) << speedup << "\n";

      hmap::export_banner_png("ex_fft.png", {z0, zf, zr}, hmap::Cmap::INFERNO);
    }

    std::cout << std::string(82, '-') << "\n";
  }

  return 0;
}
