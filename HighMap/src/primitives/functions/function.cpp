/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/functions.hpp"

namespace hmap
{

HMAP_FCT_XY_TYPE Function::get_delegate() const
{
  return this->delegate;
}

float Function::get_value(float x, float y, float ctrl_param) const
{
  return this->delegate(x, y, ctrl_param);
}

void Function::set_delegate(HMAP_FCT_XY_TYPE new_delegate)
{
  this->delegate = std::move(new_delegate);
}

} // namespace hmap
