/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <memory>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/gpu.hpp"

#include "highmap/dbg.hpp"

namespace hmap::gpu
{

void maximum_local_weighted(OpenCLConfig &config, Array &array, Array &weights)
{
  int    err;
  Array *p_weights;

  // --- perform some checkings

  Vec2<int> size = Vec2<int>(
      hmap::gpu::smallest_multiple(weights.shape.x, config.block_size),
      hmap::gpu::smallest_multiple(weights.shape.y, config.block_size));

  Array weights_resized = Array();

  if (size != weights.shape)
  {
    LOG_DEBUG("effective filter size: {%d, %d} (requested size: {%d, %d})",
              size.x,
              size.y,
              weights.shape.x,
              weights.shape.y);

    weights_resized = weights.resample_to_shape(size);
    p_weights = &weights_resized;
  }
  else
    p_weights = &weights;

  Timer timer = Timer("maximum_local_weighted");

  // --- wrapper to GPU host

  cl::Buffer buffer_in(config.context, CL_MEM_READ_ONLY, array.get_sizeof());

  cl::Buffer buffer_weights(config.context,
                            CL_MEM_READ_ONLY,
                            p_weights->get_sizeof());

  cl::Buffer buffer_out(config.context, CL_MEM_WRITE_ONLY, array.get_sizeof());

  cl::CommandQueue queue(config.context, config.device);

  err = queue.enqueueWriteBuffer(buffer_in,
                                 CL_TRUE,
                                 0,
                                 array.get_sizeof(),
                                 array.vector.data());

  OPENCL_ERROR_MESSAGE(err, "enqueueWriteBuffer");

  err = queue.enqueueWriteBuffer(buffer_weights,
                                 CL_TRUE,
                                 0,
                                 p_weights->get_sizeof(),
                                 p_weights->vector.data());

  OPENCL_ERROR_MESSAGE(err, "enqueueWriteBuffer");

  cl::Kernel kernel = cl::Kernel(config.program, "maximum_local_weighted");

  err = kernel.setArg(0, buffer_in);
  err |= kernel.setArg(1, buffer_out);
  err |= kernel.setArg(2, buffer_weights);
  err |= kernel.setArg(3, size.x);
  err |= kernel.setArg(4, size.y);
  err |= kernel.setArg(5, array.shape.x);
  err |= kernel.setArg(6, array.shape.y);

  OPENCL_ERROR_MESSAGE(err, "setArg");

  err = queue.finish();
  timer.start("test");

  const cl::NDRange global_work_size(array.shape.x, array.shape.y);
  const cl::NDRange local_work_size(config.block_size, config.block_size);

  err = queue.enqueueNDRangeKernel(kernel,
                                   cl::NullRange,
                                   global_work_size,
                                   local_work_size);

  OPENCL_ERROR_MESSAGE(err, "enqueueNDRangeKernel");

  err = queue.finish();
  OPENCL_ERROR_MESSAGE(err, "finish");
  timer.stop("test");

  err = queue.enqueueReadBuffer(buffer_out,
                                CL_TRUE,
                                0,
                                array.get_sizeof(),
                                array.vector.data());

  OPENCL_ERROR_MESSAGE(err, "enqueueReadBuffer");
}

} // namespace hmap::gpu
