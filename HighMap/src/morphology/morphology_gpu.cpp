/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/boundary.hpp"
#include "highmap/filters.hpp"
#include "highmap/morphology.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

Array border(const Array &array, int ir)
{
  return array - gpu::erosion(array, ir);
}

Array closing(const Array &array, int ir)
{
  return gpu::erosion(gpu::dilation(array, ir), ir);
}

Array dilation(const Array &array, int ir)
{
  return gpu::maximum_local(array, ir);
}

Array erosion(const Array &array, int ir)
{
  return gpu::minimum_local(array, ir);
}

Array morphological_black_hat(const Array &array, int ir)
{
  return gpu::closing(array, ir) - array;
}

Array morphological_gradient(const Array &array, int ir)
{
  return gpu::dilation(array, ir) - gpu::erosion(array, ir);
}

Array morphological_top_hat(const Array &array, int ir)
{
  return array - gpu::opening(array, ir);
}

Array opening(const Array &array, int ir)
{
  return gpu::dilation(gpu::erosion(array, ir), ir);
}

Array relative_distance_from_skeleton(const Array &array,
                                      int          ir_search,
                                      bool         zero_at_borders,
                                      int          ir_erosion)
{
  Array border = array - gpu::erosion(array, ir_erosion);
  Array sk = gpu::skeleton(array, zero_at_borders);
  Array rdist(array.shape);

  auto run = clwrapper::Run("relative_distance_from_skeleton");

  run.bind_imagef("array",
                  const_cast<std::vector<float> &>(array.vector),
                  array.shape.x,
                  array.shape.y);
  run.bind_imagef("sk", sk.vector, array.shape.x, array.shape.y);
  run.bind_imagef("border", border.vector, array.shape.x, array.shape.y);
  run.bind_imagef("rdist", rdist.vector, array.shape.x, array.shape.y, true);
  run.bind_arguments(array.shape.x, array.shape.y, ir_search);

  run.execute({array.shape.x, array.shape.y});

  run.read_imagef("rdist");

  return rdist;
}

Array skeleton(const Array &array, bool zero_at_borders)
{
  Array sk = array;
  Array prev;
  Array diff;

  auto run = clwrapper::Run("thinning");

  run.bind_imagef("in", sk.vector, array.shape.x, array.shape.y);
  run.bind_imagef("out", sk.vector, array.shape.x, sk.shape.y, true);
  run.bind_arguments(array.shape.x, array.shape.y, 0);

  do
  {
    prev = sk;

    run.set_argument(4, 0); // pass 1
    run.write_imagef("in");
    run.execute({array.shape.x, array.shape.y});
    run.read_imagef("out");

    run.set_argument(4, 1); // pass 2
    run.write_imagef("in");
    run.execute({array.shape.x, array.shape.y});
    run.read_imagef("out");

    diff = sk - prev;

  } while (diff.count_non_zero() > 0);

  // set border to zero
  if (zero_at_borders) zeroed_borders(sk);

  return sk;
}

} // namespace hmap::gpu
