#include "highmap/array.hpp"

namespace hmap
{

void export_banner_png(std::string fname, std::vector<Array> arrays, int cmap)
{
  // build up big array by stacking input arrays
  if (arrays.size() > 1)
  {
    Array banner_array = hstack(arrays[0], arrays[1]);
    for (uint i = 2; i < arrays.size(); i++)
      banner_array = hstack(banner_array, arrays[i]);
    banner_array.to_png(fname, cmap);
  }
  else
  {
    Array banner_array = arrays[0];
    banner_array.to_png(fname, cmap);
  }
}

} // namespace hmap
