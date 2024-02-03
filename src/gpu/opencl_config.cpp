/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include "highmap/gpu.hpp"
#include "macrologger.h"

namespace hmap::gpu
{

OpenCLConfig::OpenCLConfig()
{
  this->initialize_context();
  this->build_program();
}

void OpenCLConfig::build_program()
{
  LOG_DEBUG("loading kernel sources");

  cl::Program::Sources sources;

  const std::string kernel_code =
#include "kernel.cl"
      ;

  if (kernel_code.length() == 0)
    throw std::runtime_error("empty kernel code");

  sources.push_back({kernel_code.c_str(), kernel_code.length()});

  LOG_DEBUG("building OpenCL kernels");

  cl::Program program(context, sources);
  std::string building_options = "-DBLOCK_SIZE=" +
                                 std::to_string(this->block_size);

  LOG_DEBUG("building options: %s", building_options.c_str());

  if (program.build({this->device}, building_options.c_str()) != CL_SUCCESS)
  {
    LOG_ERROR("build error");
    std::cout << " Error building: "
              << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(this->device)
              << "\n";
    throw std::runtime_error("build error");
  }

  this->program = program;
}

void OpenCLConfig::infos()
{
  for (cl::Device device : this->context.getInfo<CL_CONTEXT_DEVICES>())
  {
    LOG_INFO("name: %s", device.getInfo<CL_DEVICE_NAME>().c_str());
    LOG_INFO("vendor: %s", device.getInfo<CL_DEVICE_VENDOR>().c_str());
    LOG_INFO("version: %s", device.getInfo<CL_DEVICE_VERSION>().c_str());
    for (auto p : device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>())
      LOG_INFO("work items: %ld", p);
    LOG_INFO("work groups: %ld",
             device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>());
    LOG_INFO("compute units: %d",
             device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>());
    LOG_INFO("global memory: %ld MB",
             device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() / 1024 / 1024);
    LOG_INFO("local memory: %ld kB",
             device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() / 1024);
  }
}

void OpenCLConfig::initialize_context()
{
  LOG_DEBUG("initializing context");

  std::vector<cl::Platform> all_platforms;
  cl::Platform::get(&all_platforms);
  cl::Platform default_platform = all_platforms[0];

  LOG_DEBUG("available platforms:");
  for (auto &p : all_platforms)
    LOG_DEBUG("- %s", p.getInfo<CL_PLATFORM_NAME>().c_str());

  // get default device
  std::vector<cl::Device> all_devices;
  default_platform.getDevices(CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU,
                              &all_devices);

  LOG_DEBUG("devices:");
  for (auto &dev : all_devices)
  {
    LOG_DEBUG("- device Name: %s", dev.getInfo<CL_DEVICE_NAME>().c_str());
    LOG_DEBUG(" - device Vendor: %s", dev.getInfo<CL_DEVICE_VENDOR>().c_str());
    LOG_DEBUG(" - device Version: %s",
              dev.getInfo<CL_DEVICE_VERSION>().c_str());

    switch (dev.getInfo<CL_DEVICE_TYPE>())
    {
    case CL_DEVICE_TYPE_GPU:
      LOG_DEBUG(" - device Type: GPU");
      break;
    case CL_DEVICE_TYPE_CPU:
      LOG_DEBUG(" - device Type: CPU");
      break;
    case CL_DEVICE_TYPE_ACCELERATOR:
      LOG_DEBUG(" - device Type: ACCELERATOR");
      break;
    default:
      LOG_DEBUG(" - device Type: unknown");
    }
  }

  if (all_devices.size() == 0)
    throw std::runtime_error(
        "No GPU devices found. Check OpenCL installation.");

  cl::Device default_device = all_devices.front();
  this->context = cl::Context({default_device});
  this->device = default_device;

  LOG_DEBUG("using device: %s", this->device.getInfo<CL_DEVICE_NAME>().c_str());
}

void OpenCLConfig::set_block_size(int new_block_size)
{
  this->block_size = new_block_size;
  this->build_program();
}

} // namespace hmap::gpu
