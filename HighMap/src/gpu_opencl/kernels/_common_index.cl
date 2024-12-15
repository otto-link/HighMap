R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
int linear_index(const int i, const int j, const int nx)
{
  return j * nx + i;
}
)""
