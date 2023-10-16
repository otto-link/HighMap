/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap
{

void flood_fill(Array &array,
                int    i,
                int    j,
                float  fill_value,
                float  background_value)
{
  std::vector<int>   queue_i = {i};
  std::vector<int>   queue_j = {j};

  queue_i.reserve(array.shape.x * array.shape.y);
  queue_j.reserve(array.shape.x * array.shape.y);

  while(queue_i.size() > 0)
    {
      int i = queue_i.back();
      int j = queue_j.back();
      queue_i.pop_back();
      queue_j.pop_back();

      if (array(i, j) == background_value)
	{
	  array(i, j) = fill_value;

	  if (i > 0)
	    {
	      queue_i.push_back(i - 1);
	      queue_j.push_back(j);
	    }
	  if (i < array.shape.x - 1)
	    {
	      queue_i.push_back(i + 1);
	      queue_j.push_back(j);
	    }
	  if (j > 0)
	    {
	      queue_i.push_back(i);
	      queue_j.push_back(j - 1);
	    }
	  if (j < array.shape.y - 1)
	    {
	      queue_i.push_back(i);
	      queue_j.push_back(j + 1);
	    }
	}
    }
}

} // namespace hmap
