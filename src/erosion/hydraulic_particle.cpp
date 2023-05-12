#include <random>

#include "highmap/array.hpp"
#include "highmap/math.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

#define DT 1.f // 1.f
#define VOLUME_INIT 1.f
#define VOLUME_MIN 0.01f
#define SPAWN_MOISTURE_LOW_LIMIT 0.1f
#define VELOCITY_INIT 0.00f
#define VELOCITY_MIN 0.001f
#define GRADIENT_MIN 0.0001f
#define LAPLACE_PERIOD 10000
#define LAPLACE_SIGMA 0.05f
#define LAPLACE_ITERATIONS 1

namespace hmap
{

//----------------------------------------------------------------------
// Main operator(s)
//----------------------------------------------------------------------

void hydraulic_particle(Array &z,
                        Array &moisture_map,
                        int    nparticles,
                        int    seed,
                        int    c_radius,
                        float  c_capacity,
                        float  c_erosion,
                        float  c_deposition,
                        float  drag_rate,
                        float  evap_rate)
{
  const int ni = z.shape[0];
  const int nj = z.shape[1];
  float     dt = DT;

  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0, 1);

  // --- initialization

  // define erosion kernel
  const int ir = c_radius;
  const int nk = 2 * ir + 1;
  Array     kernel = cone({nk, nk});

  // normalize
  kernel = kernel / kernel.sum();

  // --- main loop
  for (int ip = 0; ip < nparticles; ip++)
  {
    float x = 0.f;
    float y = 0.f;
    float vx = VELOCITY_INIT * (2.f * dis(gen) - 1.f);
    float vy = VELOCITY_INIT * (2.f * dis(gen) - 1.f);
    float s = 0.f; // sediment

    // keep spawning new particle in [1, shape[...] - 2] until the
    // initial volume is large enough (to avoid using particle from
    // dry regions)
    do
    {
      x = dis(gen) * (float)(ni - 3) + 1;
      y = dis(gen) * (float)(nj - 3) + 1;
    } while (moisture_map((int)x, (int)y) < SPAWN_MOISTURE_LOW_LIMIT);

    float volume = VOLUME_INIT * moisture_map((int)x, (int)y);

    int ix = (int)x;
    int jy = (int)y;

    while (volume > VOLUME_MIN)
    {
      int i = ix;
      int j = jy;

      // surface normal \nabla z = (-dz/dx, -dz/dy, 1)
      float nx = -z.get_gradient_x_at(i, j);
      float ny = -z.get_gradient_y_at(i, j);

      if (approx_hypot(nx, ny) < GRADIENT_MIN)
        break;

      // classical mechanics (with gravity = 1)
      vx += dt * nx;
      vy += dt * ny;

      // drag
      vx *= (1.f - dt * drag_rate);
      vy *= (1.f - dt * drag_rate);

      float vnorm = approx_hypot(vx, vy);

      if (vnorm < VELOCITY_MIN)
        break;

      // move particle
      x += dt * vx;
      y += dt * vy;

      // perform erosion / deposition
      ix = (int)x;
      jy = (int)y;

      if ((i != ix) or (j != jy))
      {
        if ((ix < 1) or (ix > ni - 2) or (jy < 1) or (jy > nj - 2))
          break;

        // particle sediment capacity
        float dz = z(i, j) - z(ix, jy);
        float sc = std::max(0.f, c_capacity * volume * vnorm * dz);
        float delta_sc = dt * (sc - s);

        if (delta_sc > 0.f) // - EROSION -
        {
          // clamp erosion amount to avoid instabilities
          float amount = c_erosion * delta_sc;
          s += amount;

          if (ir == 0)
          {
            // pixel-based erosion
            z(i, j) -= amount;
          }
          else if ((i > ir) and (i < ni - ir - 1) and (j > ir) and
                   (j < nj - ir - 1))
          {
            // kernel-based erosion
            z.depose_amount_kernel(i, j, kernel, -amount);
          }
        }
        else // - DEPOSITION -
        {
          float amount = c_deposition * delta_sc;
          s += amount;
          z(i, j) -= amount;
        }
      }

      volume *= (1 - dt * evap_rate);
    }

    // apply low-pass filtering to smooth spurious spatial
    // oscillations
    if (ip % LAPLACE_PERIOD == 0)
    {
      fill_borders(z); // more stable
      laplace(z, LAPLACE_SIGMA, LAPLACE_ITERATIONS);
    }
  }

  extrapolate_borders(z); // look nicer
  laplace(z, LAPLACE_SIGMA, LAPLACE_ITERATIONS);
}

//----------------------------------------------------------------------
// Overloading
//----------------------------------------------------------------------

// uniform moisture map
void hydraulic_particle(Array &z,
                        int    nparticles,
                        int    seed,
                        int    c_radius,
                        float  c_capacity,
                        float  c_erosion,
                        float  c_deposition,
                        float  drag_rate,
                        float  evap_rate)
{
  Array mmap = constant(z.shape, 1.f);
  hydraulic_particle(z,
                     mmap,
                     nparticles,
                     seed,
                     c_radius,
                     c_capacity,
                     c_erosion,
                     c_deposition,
                     drag_rate,
                     evap_rate);
}

//----------------------------------------------------------------------
// Macros
//----------------------------------------------------------------------

// void hydraulic_particle_multiscale(Array &z)
// {
// }

// void hydraulic_thermal(Array &z)
// {
// }

} // namespace hmap