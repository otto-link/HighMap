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

void hydraulic_particle(OpenCLConfig &config,
                        Array        &array,
                        const int nparticles,
                        const uint    seed,
                        const float   c_capacity,
                        const float   c_erosion,
                        const float   c_deposition,
                        const float   drag_rate,
                        const float   evap_rate,
                        const float   dt)
{
  int err;

  int nparticles_resized = closest_smaller_multiple<int>(nparticles,
                                                         config.block_size);

  if (nparticles_resized != nparticles)
    LOG_DEBUG("effective number of particles: %d (requested: %d)",
              nparticles_resized,
              nparticles);


  cl::CommandQueue queue(config.context, config.device);

  cl::Image2D img_io(config.context,
                     CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                     cl::ImageFormat(CL_LUMINANCE, CL_FLOAT),
                     array.shape.y,
                     array.shape.x,
                     0,
                     (void *)array.vector.data(),
                     &err);

  OPENCL_ERROR_MESSAGE(err, "Image2D");

  cl::Kernel kernel = cl::Kernel(config.program, "hydraulic_particle", &err);

  OPENCL_ERROR_MESSAGE(err, "Kernel");

  {
    int iarg = 0;
    err = kernel.setArg(iarg++, img_io);
    err |= kernel.setArg(iarg++, seed);
    err |= kernel.setArg(iarg++, c_capacity);
    err |= kernel.setArg(iarg++, c_erosion);
    err |= kernel.setArg(iarg++, c_deposition);
    err |= kernel.setArg(iarg++, drag_rate);
    err |= kernel.setArg(iarg++, evap_rate);
    err |= kernel.setArg(iarg++, dt);
    OPENCL_ERROR_MESSAGE(err, "setArg");
  }

  const cl::NDRange global_work_size(nparticles_resized);
  const cl::NDRange local_work_size(config.block_size, config.block_size);

  err = queue.enqueueNDRangeKernel(kernel,
                                   cl::NullRange,
                                   global_work_size,
                                   local_work_size);

  OPENCL_ERROR_MESSAGE(err, "enqueueNDRangeKernel");

  err = queue.finish();
  OPENCL_ERROR_MESSAGE(err, "finish");

  cl::array<size_t, 2> origin = {0, 0};
  cl::array<size_t, 2> region = {(size_t)array.shape.y, (size_t)array.shape.x};

  err = queue.enqueueReadImage(img_io,
                               CL_TRUE,
                               origin,
                               region,
                               0,
                               0,
                               (void *)array.vector.data());
  OPENCL_ERROR_MESSAGE(err, "enqueueReadImage");
}

void maximum_local_weighted(OpenCLConfig &config, Array &array, Array &weights)
{
  int    err;
  Array *p_weights;

  // --- perform some checkings

  Vec2<int> size = Vec2<int>(
      hmap::gpu::closest_smaller_multiple(weights.shape.x, config.block_size),
      hmap::gpu::closest_smaller_multiple(weights.shape.y, config.block_size));

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

  cl::Buffer buffer_in(config.context,
                       CL_MEM_READ_ONLY,
                       array.get_sizeof(),
                       &err);

  OPENCL_ERROR_MESSAGE(err, "Buffer");

  cl::Buffer buffer_weights(config.context,
                            CL_MEM_READ_ONLY,
                            p_weights->get_sizeof(),
                            &err);
  OPENCL_ERROR_MESSAGE(err, "Buffer");

  cl::Buffer buffer_out(config.context,
                        CL_MEM_WRITE_ONLY,
                        array.get_sizeof(),
                        &err);
  OPENCL_ERROR_MESSAGE(err, "Buffer");

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

  cl::Kernel kernel = cl::Kernel(config.program,
                                 "maximum_local_weighted",
                                 &err);
  OPENCL_ERROR_MESSAGE(err, "Kernel");

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
