#include <cmath>

#include "highmap/array.hpp"

namespace hmap
{

void extrapolate_borders(Array &array)
{
  const int ni = array.shape[0];
  const int nj = array.shape[1];

  for (int j = 0; j < nj; j++)
  {
    array(0, j) = 2.f * array(1, j) - array(2, j);
    array(ni - 1, j) = 2.f * array(ni - 2, j) - array(ni - 3, j);
  }

  for (int i = 0; i < ni; i++)
  {
    array(i, 0) = 2.f * array(i, 1) - array(i, 2);
    array(i, nj - 1) = 2.f * array(i, nj - 2) - array(i, nj - 3);
  }
}

void fill_borders(Array &array)
{
  const int ni = array.shape[0];
  const int nj = array.shape[1];

  for (int j = 0; j < nj; j++)
  {
    array(0, j) = array(1, j);
    array(ni - 1, j) = array(ni - 2, j);
  }

  for (int i = 0; i < ni; i++)
  {
    array(i, 0) = array(i, 1);
    array(i, nj - 1) = array(i, nj - 2);
  }
}

} // namespace hmap