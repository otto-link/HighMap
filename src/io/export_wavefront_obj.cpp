/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cstdint>

#include "WavefrontObjWriter/obj.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"

namespace hmap
{

void export_wavefront_obj(std::string  fname,
                          const Array &array,
                          float        elevation_scaling)
{
  wow::Obj obj;

  std::vector<float> x = linspace(0.f, 1.f, array.shape.x);
  std::vector<float> y = linspace(0.f, 1.f, array.shape.y);

  for (int i = 0; i < array.shape.x - 1; i++)
    for (int j = 0; j < array.shape.y - 1; j++)
    {
      obj.appendVertex(x[i], elevation_scaling * array(i, j), y[j]);
      obj.appendVertex(x[i + 1], elevation_scaling * array(i + 1, j), y[j]);
      obj.appendVertex(x[i + 1],
                       elevation_scaling * array(i + 1, j + 1),
                       y[j + 1]);
      obj.appendVertex(x[i], elevation_scaling * array(i, j + 1), y[j + 1]);
      obj.closeFace();
    }

  // remove extension if already provided
  size_t      lastindex = fname.find_last_of(".");
  std::string fname_raw = fname.substr(0, lastindex);

  obj.output(fname_raw);
}

} // namespace hmap
