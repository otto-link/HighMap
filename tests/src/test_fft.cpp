#include "highmap/array.hpp"
#include "highmap/dbg/assert.hpp"
#include "highmap/kernels.hpp"
#include "highmap/math/convolve.hpp"
#include "highmap/math/fft.hpp"
#include "highmap/primitives.hpp"

#include <gtest/gtest.h>

using namespace hmap;

// ------------------------------------------------------------
// FFT Roundtrip Tests
// ------------------------------------------------------------

TEST(FFTTest, RoundtripImpulse)
{
  glm::ivec2 shape(16, 16);
  Array      a(shape, 0.0f);
  a(3, 5) = 1.0f;
  a(7, 2) = 2.5f;

  std::vector<std::complex<float>> spec = fft2d_r2c(a);
  Array                            reconstructed = fft2d_c2r(spec, shape, true);

  ASSERT_EQ(reconstructed.shape.x, shape.x);
  ASSERT_EQ(reconstructed.shape.y, shape.y);

  for (int j = 0; j < shape.y; ++j)
  {
    for (int i = 0; i < shape.x; ++i)
    {
      EXPECT_NEAR(reconstructed(i, j), a(i, j), 1e-5f);
    }
  }
}

TEST(FFTTest, GoodSize)
{
  EXPECT_EQ(fft_good_size(1), 1);
  EXPECT_GE(fft_good_size(17), 17);
  EXPECT_GE(fft_good_size(63), 63);
  EXPECT_GE(fft_good_size(100), 100);
}

// ------------------------------------------------------------
// FFT Convolution Tests
// ------------------------------------------------------------

TEST(FFTTest, Convolve2D_FFT_DeltaKernel)
{
  glm::ivec2 shape(32, 32);
  Array      input = white(glm::vec2(shape.x, shape.y), 0.0f, 1.0f, 1234);

  // 1x1 Dirac delta kernel
  Array delta_kernel(glm::ivec2(1, 1), 1.0f);
  Array out = convolve2d_fft(input, delta_kernel);

  ASSERT_EQ(out.shape.x, shape.x);
  ASSERT_EQ(out.shape.y, shape.y);

  for (int j = 0; j < shape.y; ++j)
  {
    for (int i = 0; i < shape.x; ++i)
    {
      EXPECT_NEAR(out(i, j), input(i, j), 1e-4f);
    }
  }
}

TEST(FFTTest, Convolve2D_FFT_MatchesConvolve2D)
{
  glm::ivec2 shape(48, 48);
  Array      input = white(glm::vec2(shape.x, shape.y), 0.0f, 1.0f, 42);

  // 9x9 kernel
  Array kernel = cubic_pulse(glm::ivec2(9, 9));

  Array out_spatial = convolve2d(input, kernel);
  Array out_fft = convolve2d_fft(input, kernel);

  ASSERT_EQ(out_fft.shape.x, out_spatial.shape.x);
  ASSERT_EQ(out_fft.shape.y, out_spatial.shape.y);

  for (int j = 0; j < shape.y; ++j)
  {
    for (int i = 0; i < shape.x; ++i)
    {
      EXPECT_NEAR(out_fft(i, j), out_spatial(i, j), 1e-3f);
    }
  }
}

TEST(FFTTest, Convolve2D_FFT_LargeKernel)
{
  glm::ivec2 shape(64, 64);
  Array      input = white(glm::vec2(shape.x, shape.y), 0.0f, 1.0f, 999);

  // Large 31x31 kernel
  Array kernel = cubic_pulse(glm::ivec2(31, 31));

  Array out_spatial = convolve2d(input, kernel);
  Array out_fft = convolve2d_fft(input, kernel);

  ASSERT_EQ(out_fft.shape.x, shape.x);
  ASSERT_EQ(out_fft.shape.y, shape.y);

  for (int j = 0; j < shape.y; ++j)
  {
    for (int i = 0; i < shape.x; ++i)
    {
      EXPECT_NEAR(out_fft(i, j), out_spatial(i, j), 1e-3f);
    }
  }
}
