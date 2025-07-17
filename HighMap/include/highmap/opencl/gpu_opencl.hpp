/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file math.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include "cl_wrapper.hpp"

#include "highmap/array.hpp"

namespace hmap::gpu
{

void helper_bind_optional_buffer(clwrapper::Run    &run,
                                 const std::string &id,
                                 const Array       *p_array);

bool init_opencl();

} // namespace hmap::gpu