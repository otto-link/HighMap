/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <string>

namespace hmap
{

std::string zfill(const std::string &str, int n_zero)
{
  return std::string(n_zero - std::min(n_zero, (int)str.length()), '0') + str;
}

} // namespace hmap
