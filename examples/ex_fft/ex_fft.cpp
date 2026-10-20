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

  // ========================================================================================================
  // Benchmark 1: 2D Convolution (FFT convolve2d_fft vs GPU separable
  // smooth_cpulse)
  // ========================================================================================================

  std::cout << "==============================================================="
               "=========================================\n";
  std::cout << " Benchmark 1: FFT 2D Convolution (convolve2d_fft) vs GPU "
               "Separable Filter (smooth_cpulse)\n";
  std::cout << "==============================================================="
               "=========================================\n\n";

  std::cout << std::left << std::setw(12) << "Mesh Size" << std::setw(10)
            << "Radius" << std::setw(14) << "Kernel Size" << std::setw(18)
            << "FFT Conv (ms)" << std::setw(18) << "GPU Separable (ms)"
            << std::setw(16) << "Ratio (FFT/GPU)"
            << "\n";
  std::cout << std::string(88, '-') << "\n";

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

      float ratio = (dt_gpu_ms > 0.f) ? (dt_fft_ms / dt_gpu_ms) : 0.f;

      std::string mesh_str = std::to_string(n) + "x" + std::to_string(n);
      std::string kernel_str = std::to_string(shape_kernel.x) + "x" +
                               std::to_string(shape_kernel.y);

      std::cout << std::left << std::setw(12) << mesh_str << std::setw(10) << ir
                << std::setw(14) << kernel_str << std::fixed
                << std::setprecision(2) << std::setw(18) << dt_fft_ms
                << std::setw(18) << dt_gpu_ms << std::fixed
                << std::setprecision(2) << std::setw(16) << ratio << "\n";

      hmap::export_banner_png("ex_fft_conv.png", {z0, zf, zr}, hmap::Cmap::JET);
    }

    std::cout << std::string(88, '-') << "\n";
  }

  std::cout << "\n\n";

  // ========================================================================================================
  // Benchmark 2: Morphological Expand (GPU expand vs Soft-Max Expand FFT)
  // ========================================================================================================

  std::cout << "==============================================================="
               "=========================================\n";
  std::cout << " Benchmark 2: GPU Morphological Expand (gpu::expand) vs "
               "Soft-Max Expand FFT (expand_soft_fft)\n";
  std::cout << "==============================================================="
               "=========================================\n\n";

  std::cout << std::left << std::setw(12) << "Mesh Size" << std::setw(10)
            << "Radius" << std::setw(14) << "Kernel Size" << std::setw(18)
            << "GPU Expand (ms)" << std::setw(18) << "SoftExp FFT (ms)"
            << std::setw(16) << "Ratio (GPU/FFT)"
            << "\n";
  std::cout << std::string(88, '-') << "\n";

  for (int n : mesh_sizes)
  {
    glm::ivec2  shape = {n, n};
    hmap::Array z0 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);

    // Warm-up GPU once per mesh size
    hmap::Array z_warm = z0;
    hmap::gpu::expand(z_warm, 8);

    for (int ir : kernel_irs)
    {
      glm::ivec2  shape_kernel = {2 * ir + 1, 2 * ir + 1};
      hmap::Array kernel = hmap::get_kernel(shape_kernel,
                                            hmap::KernelType::CUBIC_PULSE);

      // --- Benchmark GPU standard expand with kernel
      auto z_exp = z0;
      auto t0_gpu = std::chrono::high_resolution_clock::now();
      hmap::gpu::expand(z_exp, kernel);
      auto  t1_gpu = std::chrono::high_resolution_clock::now();
      float dt_gpu_ms =
          std::chrono::duration<float, std::milli>(t1_gpu - t0_gpu).count();

      // --- Benchmark Soft-Max Expand via FFT with the exact same kernel
      auto z_soft = z0;
      auto t0_exp = std::chrono::high_resolution_clock::now();
      hmap::expand_soft_fft(z_soft, kernel, 10.f);
      auto  t1_exp = std::chrono::high_resolution_clock::now();
      float dt_exp_ms =
          std::chrono::duration<float, std::milli>(t1_exp - t0_exp).count();

      float ratio = (dt_exp_ms > 0.f) ? (dt_gpu_ms / dt_exp_ms) : 0.f;

      std::string mesh_str = std::to_string(n) + "x" + std::to_string(n);
      std::string kernel_str = std::to_string(shape_kernel.x) + "x" +
                               std::to_string(shape_kernel.y);

      std::cout << std::left << std::setw(12) << mesh_str << std::setw(10) << ir
                << std::setw(14) << kernel_str << std::fixed
                << std::setprecision(2) << std::setw(18) << dt_gpu_ms
                << std::setw(18) << dt_exp_ms << std::fixed
                << std::setprecision(2) << std::setw(16) << ratio << "\n";

      hmap::export_banner_png("ex_fft_expand.png",
                              {z0, z_exp, z_soft},
                              hmap::Cmap::JET);
    }

    std::cout << std::string(88, '-') << "\n";
  }

  return 0;
}
