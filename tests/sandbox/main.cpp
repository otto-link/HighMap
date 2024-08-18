#include "highmap.hpp"
#include "highmap/dbg/timer.hpp"

int main(void)
{
  const hmap::Vec2<int>   shape = {512, 512};
  const hmap::Vec2<float> res = {4.f, 4.f};
  int                     seed = 2;

  hmap::Timer::Start("fbm_perlin");
  hmap::Array z = hmap::noise(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::Timer::Stop("fbm_perlin");

  z.to_png("out.png", hmap::Cmap::INFERNO);
}
