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
#pragma once

#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/opencl.hpp>

#include "highmap/array.hpp"

// https://streamhpc.com/blog/2013-04-28/opencl-error-codes/
#define OPENCL_ERROR_MESSAGE(err, msg)                                         \
  {                                                                            \
    if (err != CL_SUCCESS)                                                     \
    {                                                                          \
      LOG_ERROR("%s, error id: %d", msg, err);                                 \
      throw std::runtime_error("OpenCL error");                                \
    }                                                                          \
  }

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

// --- filters

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

} // namespace hmap::gpu