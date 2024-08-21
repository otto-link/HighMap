/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/export.hpp"
#include "highmap/operator.hpp"
#include "highmap/tensor.hpp"

namespace hmap
{

Tensor compute_splatmap(Array *p_r, Array *p_g, Array *p_b, Array *p_a)
{
  Tensor smap = Tensor(p_r->shape, 4);

  smap.set_slice(0, *p_r);

  if (p_g) smap.set_slice(1, *p_g);
  if (p_b) smap.set_slice(2, *p_b);
  if (p_a) smap.set_slice(3, *p_a);
  return smap;
}

void export_splatmap_png(std::string fname,
                         Array      *p_r,
                         Array      *p_g,
                         Array      *p_b,
                         Array      *p_a,
                         int         depth)
{
  Tensor smap = compute_splatmap(p_r, p_g, p_b, p_a);
  smap.to_png(fname, depth);
}

} // namespace hmap
