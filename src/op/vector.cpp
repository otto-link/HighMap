#include <random>
#include <vector>

namespace hmap
{

std::vector<float> linspace(float start, float stop, int num)
{
  std::vector<float> v(num);

  for (int i = 0; i < num; i++)
  {
    float dv = (stop - start) / (float)(num - 1);
    v[i] = start + (float)i * dv;
  }

  return v;
}

std::vector<float> linspace_jitted(float start,
                                   float stop,
                                   int   num,
                                   float ratio,
                                   int   seed)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(-0.5f, 0.5f);

  std::vector<float> v(num);

  {
    float dv = (stop - start) / (float)num;

    for (int i = 0; i < num; i++)
      v[i] = start + (float)i * dv;

    // add noise
    for (int i = 1; i < num - 1; i++)
      v[i] += ratio * dis(gen) * dv;
  }
  return v;
}

std::vector<float> random_vector(float min, float max, int num, int seed)
{
  std::vector<float>                    v(num);
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(min, max);

  for (auto &v : v)
    v = dis(gen);
  return v;
}

} // namespace hmap
