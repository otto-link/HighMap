#include <vector>

#include <gtest/gtest.h>
#include <highmap/array.hpp>

#define NI 3
#define NJ 2

TEST(ArrayClass, BasicOperations)
{
  const std::vector<int> shape = {NI, NJ};
  Array                  array = Array(shape);

  // nb of elements
  EXPECT_EQ(array.vector.size(), NI * NJ);

  // newly created arrays are filled with zeros
  for (auto &v : array.vector)
    EXPECT_EQ(v, 0.f);
}
