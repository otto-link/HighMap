#include <cmath>
#include <cstdlib>
#include <thread>

#include "macrologger.h"

#include "highmap/array.hpp"

namespace hmap
{

void fct(Array &array, float gamma, int k1, int k2)
{
  LOG_DEBUG("in core function, CPU #%d", sched_getcpu());
  auto lambda = [&gamma](float x) { return std::pow(x, gamma); };
  std::transform(array.vector.begin() + k1,
                 array.vector.begin() + k2,
                 array.vector.begin() + k1,
                 lambda);
}

void gamma_correction_thread(Array &array, float gamma)
{
  int                      nthreads = std::thread::hardware_concurrency();
  std::vector<std::thread> threads(nthreads);
  int                      nbuffer = (int)(array.size() / nthreads);

  for (int i = 0; i < nthreads; i++)
  {
    int k1 = i * nbuffer;
    int k2 = 0;
    if (i < nthreads - 1)
      k2 = k1 + nbuffer;
    else
      k2 = array.size();

    threads[i] = std::thread(fct, std::ref(array), gamma, k1, k2);
  }

  for (int i = 0; i < nthreads; i++)
    threads[i].join();
}

} // namespace hmap
