/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/export.hpp"
#include "highmap/gradient.hpp"
#include "highmap/operator.hpp"
#include "highmap/tensor.hpp"

namespace hmap
{

void export_normal_map_png(const std::string &fname,
                           const Array       &array,
                           int                depth)
{
  Tensor nmap = normal_map(array);
  nmap.to_png(fname, depth);
}

} // namespace hmap
