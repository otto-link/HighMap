#include "highmap/array.hpp"
#include "highmap/carving.hpp"
#include "highmap/geometry/path.hpp"

#include <benchmark/benchmark.h>

using namespace hmap;

static Path make_test_path(int num_points)
{
  std::vector<glm::vec3> pts;
  pts.reserve(num_points);

  for (int i = 0; i < num_points; ++i)
  {
    float t = float(i) / float(num_points - 1);
    float x = 0.1f + 0.8f * t;
    float y = 0.5f + 0.25f * std::sin(t * 6.2831853f * 2.f);
    float z = 0.5f;
    pts.emplace_back(x, y, z);
  }

  return Path(pts);
}

static void BM_trench_bvh(benchmark::State &state)
{
  const int n = state.range(0);
  const int num_pts = state.range(1);

  Array z(glm::ivec2(n, n), 0.5f);
  Path  path = make_test_path(num_pts);

  for (auto _ : state)
  {
    Array out = z;
    trench(out, path, 0.05f, 0.1f, 0.f);
    benchmark::DoNotOptimize(out);
  }

  state.SetItemsProcessed(int64_t(state.iterations()) * n * n);
}

static void trench_args(benchmark::internal::Benchmark *b)
{
  std::vector<int> sizes = {512, 1024, 2048};
  std::vector<int> points = {50, 200};

  for (int n : sizes)
    for (int p : points)
      b->Args({n, p});
}

BENCHMARK(BM_trench_bvh)->Apply(trench_args)->Unit(benchmark::kMillisecond);
