/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file vector.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-06-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <vector>

namespace hmap
{

// TODO clean up

size_t upperbound_right(const std::vector<float> &v, float value);

std::vector<size_t> argsort(const std::vector<float> &v);

template <typename T>
void reindex_vector(std::vector<T> &v, std::vector<size_t> &idx)
{
  std::vector<T> v_new(v.size());
  for (uint k = 0; k < v.size(); k++)
    v_new[k] = v[idx[k]];
  v = v_new;
}

void vector_unique_values(std::vector<float> &v);

} // namespace hmap