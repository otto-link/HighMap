/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file gpu.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifdef ENABLE_OPENCL
#pragma once

#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#include <CL/opencl.hpp>

#include "highmap/array.hpp"
#include "highmap/dbg.hpp"

// https://streamhpc.com/blog/2013-04-28/opencl-error-codes/
#define OPENCL_ERROR_MESSAGE(err, msg)                                         \
  {                                                                            \
    if (err != CL_SUCCESS)                                                     \
    {                                                                          \
      LOG_ERROR("%s, error id: %d", msg, err);                                 \
      throw std::runtime_error(                                                \
          "OpenCL error "                                                      \
          "(https://streamhpc.com/blog/2013-04-28/opencl-error-codes/)");      \
    }                                                                          \
  }

#define HMAP_CL_DEFAULT_LOCAL_WORK_SIZE cl::NDRange(8, 8)

namespace hmap::gpu
{

/**
 * @brief OpenCLConfig class, to ease manipulation of OpenCL context / device /
 * program.
 */
struct OpenCLConfig
{
  /**
   * @brief OpenCL context.
   */
  cl::Context context;

  /**
   * @brief Device.
   */
  cl::Device device;

  /**
   * @brief OpenCL program.
   */
  cl::Program program;

  /**
   * @brief Local work size (passed to be kernel builder preprocessor as
   * BLOCK_SIZE).
   */
  int block_size = 8;

  /**
   * @brief Construct a new OpenCLConfig object.
   *
   */
  OpenCLConfig();

  /**
   * @brief Set the block size object and rebuild the OpenCL program.
   *
   * @param new_block_size New block size.
   */
  void set_block_size(int new_block_size);

  /**
   * @brief Build the OpenCL program.
   */
  void build_program();

  /**
   * @brief Display a bunch of infos on the device.
   */
  void infos();

  /**
   * @brief Initialize GPU context.
   */
  void initialize_context();
};

// --- helpers

template <typename T> T closest_smaller_multiple(T value, T factor)
{
  return (T)std::floor((float)(value) / factor) * factor;
}

template <typename T> size_t vector_sizeof(const typename std::vector<T> &v)
{
  return sizeof(T) * v.size();
}

template <typename T>
cl::Buffer buffer_from_vector(cl::Context                   &context,
                              cl::CommandQueue              &queue,
                              cl_mem_flags                   flags,
                              const typename std::vector<T> &v)
{
  int err = 0;

  cl::Buffer buffer(context, flags, vector_sizeof<float>(v), nullptr, &err);
  err = queue.enqueueWriteBuffer(buffer,
                                 CL_TRUE,
                                 0,
                                 vector_sizeof<float>(v),
                                 v.data());
  OPENCL_ERROR_MESSAGE(err, "enqueueWriteBuffer");
  return buffer;
}

// --- filters

// TODO multiscale
void hydraulic_particle(OpenCLConfig &config,
                        Array        &array,
                        int           nparticles,
                        const uint    seed,
                        const float   c_capacity = 100.f,
                        const float   c_erosion = 0.005f,
                        const float   c_deposition = 0.01f,
                        const float   drag_rate = 1e-3f,
                        const float   evap_rate = 1e-3f,
                        const float   dt = 1.5f);

/**
 * @brief TODO
 *
 * @param config
 * @param array
 * @param kernel
 *
 * **Example**
 * @include ex_gpu_maximum_local_weighted.cpp
 *
 * **Result**
 * @image html ex_gpu_maximum_local_weighted.png
 */
void maximum_local_weighted(OpenCLConfig &config, Array &array, Array &kernel);

void median_3x3(
    OpenCLConfig     &config,
    Array            &array,
    const cl::NDRange local_work_size = HMAP_CL_DEFAULT_LOCAL_WORK_SIZE);

void median_3x3_img(
    OpenCLConfig     &config,
    Array            &array,
    const cl::NDRange local_work_size = HMAP_CL_DEFAULT_LOCAL_WORK_SIZE);

// TODO add fbm parameters as inputs and fix amplitude
// TODO fix amplitude
// TODO add x, y input noise
Array ridgelines(
    OpenCLConfig      &config,
    Vec2<int>          shape,
    std::vector<float> xr,
    std::vector<float> yr,
    std::vector<float> zr,
    float              slope,
    float              k_smoothing = 1.f,
    float              width = 0.1f,
    float              vmin = 0.f,
    Vec4<float>        bbox = {0.f, 1.f, 0.f, 1.f},
    const cl::NDRange  local_work_size = HMAP_CL_DEFAULT_LOCAL_WORK_SIZE);

// TODO poor noise generation algo in OpenCL kernel
Array simplex(
    OpenCLConfig     &config,
    Vec2<int>         shape,
    Vec2<float>       kw,
    uint              seed,
    const cl::NDRange local_work_size = HMAP_CL_DEFAULT_LOCAL_WORK_SIZE);

/**
 * @brief Return an array filled with "Voronoise" noise.
 *
 * See https://iquilezles.org/articles/voronoise/.
 *
 * @param config
 * @param shape
 * @param kw
 * @param u_param
 * @param v_param
 * @param seed
 * @param local_work_size
 * @return Array
 *
 * **Example**
 * @include ex_gpu_voronoise.cpp
 *
 * **Result**
 * @image ex_gpu_voronoise.png
 */
Array voronoise(
    OpenCLConfig     &config,
    Vec2<int>         shape,
    Vec2<float>       kw,
    float             u_param,
    float             v_param,
    uint              seed,
    const cl::NDRange local_work_size = HMAP_CL_DEFAULT_LOCAL_WORK_SIZE);

} // namespace hmap::gpu

#endif // ENABLE_OPENCL
