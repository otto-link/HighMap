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

// TODO add moisture map
void hydraulic_particle(OpenCLConfig &config,
                        Array        &array,
                        const int     nparticles,
                        const uint    seed,
                        const float   c_capacity,
                        const float   c_erosion,
                        const float   c_deposition,
                        const float   drag_rate,
                        const float   evap_rate,
                        const float   dt)
{
  int err = 0;

  Timer timer = Timer("hydraulic_particle");

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

  err = queue.finish();

  const cl::NDRange global_work_size(nparticles_resized);
  const cl::NDRange local_work_size(config.block_size, config.block_size);

  err = queue.enqueueNDRangeKernel(kernel,
                                   cl::NullRange,
                                   global_work_size,
                                   local_work_size);

  OPENCL_ERROR_MESSAGE(err, "enqueueNDRangeKernel");

  timer.start("core");

  err = queue.finish();

  OPENCL_ERROR_MESSAGE(err, "finish");
  timer.stop("core");

  cl::array<size_t, 3> origin = {0, 0, 0};
  cl::array<size_t, 3> region = {(size_t)array.shape.y,
                                 (size_t)array.shape.x,
                                 1};

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
  int    err = 0;
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

  cl::CommandQueue queue(config.context, config.device);

  cl::Buffer buffer_out(config.context,
                        CL_MEM_WRITE_ONLY,
                        array.get_sizeof(),
                        nullptr,
                        &err);
  OPENCL_ERROR_MESSAGE(err, "Buffer");

  cl::Buffer buffer_in = buffer_from_vector(config.context,
                                            queue,
                                            CL_MEM_READ_ONLY,
                                            array.vector);

  cl::Buffer buffer_weights = buffer_from_vector(config.context,
                                                 queue,
                                                 CL_MEM_READ_ONLY,
                                                 p_weights->vector);

  cl::Kernel kernel = cl::Kernel(config.program,
                                 "maximum_local_weighted",
                                 &err);
  OPENCL_ERROR_MESSAGE(err, "Kernel");

  {
    int iarg = 0;
    err = kernel.setArg(iarg++, buffer_in);
    err |= kernel.setArg(iarg++, buffer_out);
    err |= kernel.setArg(iarg++, buffer_weights);
    err |= kernel.setArg(iarg++, size.x);
    err |= kernel.setArg(iarg++, size.y);
    err |= kernel.setArg(iarg++, array.shape.x);
    err |= kernel.setArg(iarg++, array.shape.y);
    OPENCL_ERROR_MESSAGE(err, "setArg");
  }

  err = queue.finish();

  const cl::NDRange global_work_size(array.shape.x, array.shape.y);
  const cl::NDRange local_work_size(config.block_size, config.block_size);

  err = queue.enqueueNDRangeKernel(kernel,
                                   cl::NullRange,
                                   global_work_size,
                                   local_work_size);

  OPENCL_ERROR_MESSAGE(err, "enqueueNDRangeKernel");

  timer.start("core");

  err = queue.finish();

  OPENCL_ERROR_MESSAGE(err, "finish");
  timer.stop("core");

  err = queue.enqueueReadBuffer(buffer_out,
                                CL_TRUE,
                                0,
                                array.get_sizeof(),
                                array.vector.data());

  OPENCL_ERROR_MESSAGE(err, "enqueueReadBuffer");
}

void median_3x3(OpenCLConfig     &config,
                Array            &array,
                const cl::NDRange local_work_size)
{
  int err = 0;

  Timer timer = Timer("median_3x3");

  // --- wrapper to GPU host

  cl::CommandQueue queue(config.context, config.device);

  cl::Buffer buffer_out(config.context,
                        CL_MEM_WRITE_ONLY,
                        array.get_sizeof(),
                        nullptr,
                        &err);
  OPENCL_ERROR_MESSAGE(err, "Buffer");

  cl::Buffer buffer_in = buffer_from_vector(config.context,
                                            queue,
                                            CL_MEM_READ_WRITE,
                                            array.vector);

  cl::Kernel kernel = cl::Kernel(config.program, "median_3x3", &err);
  OPENCL_ERROR_MESSAGE(err, "Kernel");

  {
    int iarg = 0;
    err = kernel.setArg(iarg++, buffer_in);
    err |= kernel.setArg(iarg++, buffer_out);
    err |= kernel.setArg(iarg++, array.shape.x);
    err |= kernel.setArg(iarg++, array.shape.y);
    OPENCL_ERROR_MESSAGE(err, "setArg");
  }

  err = queue.finish();

  const cl::NDRange global_work_size(array.shape.x, array.shape.y);

  err = queue.enqueueNDRangeKernel(kernel,
                                   cl::NullRange,
                                   global_work_size,
                                   local_work_size);

  OPENCL_ERROR_MESSAGE(err, "enqueueNDRangeKernel");

  timer.start("core");

  err = queue.finish();

  OPENCL_ERROR_MESSAGE(err, "finish");
  timer.stop("core");

  err = queue.enqueueReadBuffer(buffer_out,
                                CL_TRUE,
                                0,
                                array.get_sizeof(),
                                array.vector.data());

  OPENCL_ERROR_MESSAGE(err, "enqueueReadBuffer");
}

void median_3x3_img(OpenCLConfig     &config,
                    Array            &array,
                    const cl::NDRange local_work_size)
{
  int err = 0;

  Timer timer = Timer("median_3x3_img");

  // --- wrapper to GPU host

  cl::Image2D img_in(config.context,
                     CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                     cl::ImageFormat(CL_LUMINANCE, CL_FLOAT),
                     array.shape.y,
                     array.shape.x,
                     0,
                     (void *)array.vector.data(),
                     &err);

  OPENCL_ERROR_MESSAGE(err, "Image2D");

  cl::Image2D img_out(config.context,
                      CL_MEM_WRITE_ONLY,
                      cl::ImageFormat(CL_LUMINANCE, CL_FLOAT),
                      array.shape.y,
                      array.shape.x,
                      0,
                      nullptr,
                      &err);

  OPENCL_ERROR_MESSAGE(err, "Image2D");

  cl::CommandQueue queue(config.context, config.device);

  cl::Kernel kernel = cl::Kernel(config.program, "median_3x3_img", &err);
  OPENCL_ERROR_MESSAGE(err, "Kernel");

  {
    int iarg = 0;
    err = kernel.setArg(iarg++, img_in);
    err |= kernel.setArg(iarg++, img_out);
    OPENCL_ERROR_MESSAGE(err, "setArg");
  }

  err = queue.finish();

  const cl::NDRange global_work_size(array.shape.x, array.shape.y);

  err = queue.enqueueNDRangeKernel(kernel,
                                   cl::NullRange,
                                   global_work_size,
                                   local_work_size);

  OPENCL_ERROR_MESSAGE(err, "enqueueNDRangeKernel");

  timer.start("core");

  err = queue.finish();

  OPENCL_ERROR_MESSAGE(err, "finish");
  timer.stop("core");

  cl::array<size_t, 3> origin = {0, 0, 0};
  cl::array<size_t, 3> region = {(size_t)array.shape.y,
                                 (size_t)array.shape.x,
                                 1};

  err = queue.enqueueReadImage(img_out,
                               CL_TRUE,
                               origin,
                               region,
                               0,
                               0,
                               (void *)array.vector.data());
  OPENCL_ERROR_MESSAGE(err, "enqueueReadImage");
}

Array ridgelines(OpenCLConfig      &config,
                 Vec2<int>          shape,
                 std::vector<float> xr,
                 std::vector<float> yr,
                 std::vector<float> zr,
                 float              slope,
                 float              k_smoothing,
                 float              width,
                 float              vmin,
                 Vec4<float>        bbox,
                 const cl::NDRange  local_work_size)
{
  int   err = 0;
  Array array = Array(shape); // output

  // normalized (x, y) coordinates according to tge domain bounding
  // box (OpenCL buffer assumes that the domain is a unit square)
  std::vector<float> xr_scaled = xr;
  std::vector<float> yr_scaled = yr;
  std::vector<float> zr_scaled = zr;

  for (size_t k = 0; k < xr.size(); k++)
  {
    xr_scaled[k] = (xr[k] - bbox.a) / (bbox.b - bbox.a);
    yr_scaled[k] = (yr[k] - bbox.c) / (bbox.d - bbox.c);
  }

  Timer timer = Timer("ridgelines");

  cl::CommandQueue queue(config.context, config.device);

  cl::Buffer buffer_out(config.context,
                        CL_MEM_WRITE_ONLY,
                        array.get_sizeof(),
                        nullptr,
                        &err);
  OPENCL_ERROR_MESSAGE(err, "Buffer");

  cl::Buffer buffer_xr = buffer_from_vector(config.context,
                                            queue,
                                            CL_MEM_READ_ONLY,
                                            xr_scaled);

  cl::Buffer buffer_yr = buffer_from_vector(config.context,
                                            queue,
                                            CL_MEM_READ_ONLY,
                                            yr_scaled);

  cl::Buffer buffer_zr = buffer_from_vector(config.context,
                                            queue,
                                            CL_MEM_READ_ONLY,
                                            zr_scaled);

  cl::Kernel kernel = cl::Kernel(config.program, "ridgelines", &err);
  OPENCL_ERROR_MESSAGE(err, "Kernel");

  {
    int iarg = 0;
    err = kernel.setArg(iarg++, buffer_out);
    err |= kernel.setArg(iarg++, buffer_xr);
    err |= kernel.setArg(iarg++, buffer_yr);
    err |= kernel.setArg(iarg++, buffer_zr);
    err |= kernel.setArg(iarg++, (int)xr.size());
    err |= kernel.setArg(iarg++, slope);
    err |= kernel.setArg(iarg++, k_smoothing);
    err |= kernel.setArg(iarg++, width);
    err |= kernel.setArg(iarg++, vmin);
    err |= kernel.setArg(iarg++, array.shape.x);
    err |= kernel.setArg(iarg++, array.shape.y);
    OPENCL_ERROR_MESSAGE(err, "setArg");
  }

  err = queue.finish();

  const cl::NDRange global_work_size(array.shape.x, array.shape.y);

  err = queue.enqueueNDRangeKernel(kernel,
                                   cl::NullRange,
                                   global_work_size,
                                   local_work_size);

  OPENCL_ERROR_MESSAGE(err, "enqueueNDRangeKernel");

  timer.start("core");

  err = queue.finish();

  OPENCL_ERROR_MESSAGE(err, "finish");
  timer.stop("core");

  err = queue.enqueueReadBuffer(buffer_out,
                                CL_TRUE,
                                0,
                                array.get_sizeof(),
                                array.vector.data());

  OPENCL_ERROR_MESSAGE(err, "enqueueReadBuffer");

  return array;
}

Array simplex(OpenCLConfig     &config,
              Vec2<int>         shape,
              Vec2<float>       kw,
              uint              seed,
              const cl::NDRange local_work_size)
{
  int   err = 0;
  Array array = Array(shape); // output

  Timer timer = Timer("simplex");

  cl::CommandQueue queue(config.context, config.device);

  cl::Buffer buffer_out(config.context,
                        CL_MEM_WRITE_ONLY,
                        array.get_sizeof(),
                        nullptr,
                        &err);
  OPENCL_ERROR_MESSAGE(err, "Buffer");

  cl::Kernel kernel = cl::Kernel(config.program, "simplex", &err);
  OPENCL_ERROR_MESSAGE(err, "Kernel");

  {
    int iarg = 0;
    err = kernel.setArg(iarg++, buffer_out);
    err |= kernel.setArg(iarg++, kw.x);
    err |= kernel.setArg(iarg++, kw.y);
    err |= kernel.setArg(iarg++, seed);
    err |= kernel.setArg(iarg++, array.shape.x);
    err |= kernel.setArg(iarg++, array.shape.y);
    OPENCL_ERROR_MESSAGE(err, "setArg");
  }

  err = queue.finish();

  const cl::NDRange global_work_size(array.shape.x, array.shape.y);

  err = queue.enqueueNDRangeKernel(kernel,
                                   cl::NullRange,
                                   global_work_size,
                                   local_work_size);

  OPENCL_ERROR_MESSAGE(err, "enqueueNDRangeKernel");

  timer.start("core");

  err = queue.finish();

  OPENCL_ERROR_MESSAGE(err, "finish");
  timer.stop("core");

  err = queue.enqueueReadBuffer(buffer_out,
                                CL_TRUE,
                                0,
                                array.get_sizeof(),
                                array.vector.data());

  OPENCL_ERROR_MESSAGE(err, "enqueueReadBuffer");

  return array;
}

Array voronoise(OpenCLConfig     &config,
                Vec2<int>         shape,
                Vec2<float>       kw,
                float             u_param,
                float             v_param,
                uint              seed,
                const cl::NDRange local_work_size)
{
  int   err = 0;
  Array array = Array(shape); // output

  Timer timer = Timer("voronoise");

  cl::CommandQueue queue(config.context, config.device);

  cl::Buffer buffer_out(config.context,
                        CL_MEM_WRITE_ONLY,
                        array.get_sizeof(),
                        nullptr,
                        &err);
  OPENCL_ERROR_MESSAGE(err, "Buffer");

  cl::Kernel kernel = cl::Kernel(config.program, "voronoise", &err);
  OPENCL_ERROR_MESSAGE(err, "Kernel");

  {
    int iarg = 0;
    err = kernel.setArg(iarg++, buffer_out);
    err |= kernel.setArg(iarg++, kw.x);
    err |= kernel.setArg(iarg++, kw.y);
    err |= kernel.setArg(iarg++, u_param);
    err |= kernel.setArg(iarg++, v_param);
    err |= kernel.setArg(iarg++, seed);
    err |= kernel.setArg(iarg++, array.shape.x);
    err |= kernel.setArg(iarg++, array.shape.y);
    OPENCL_ERROR_MESSAGE(err, "setArg");
  }

  err = queue.finish();

  const cl::NDRange global_work_size(array.shape.x, array.shape.y);

  err = queue.enqueueNDRangeKernel(kernel,
                                   cl::NullRange,
                                   global_work_size,
                                   local_work_size);

  OPENCL_ERROR_MESSAGE(err, "enqueueNDRangeKernel");

  timer.start("core");

  err = queue.finish();

  OPENCL_ERROR_MESSAGE(err, "finish");
  timer.stop("core");

  err = queue.enqueueReadBuffer(buffer_out,
                                CL_TRUE,
                                0,
                                array.get_sizeof(),
                                array.vector.data());

  OPENCL_ERROR_MESSAGE(err, "enqueueReadBuffer");

  return array;
}

} // namespace hmap::gpu
