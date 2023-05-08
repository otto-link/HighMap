#include <iostream>
#include <vector>

#include <gtest/gtest.h>
#include <highmap/array.hpp>

#define NI 3
#define NJ 2

TEST(ArrayClass, OverloadsAssign)
{
  const std::vector<int> shape = {NI, NJ};
  hmap::Array            a = hmap::Array(shape);

  // nb of elements
  EXPECT_EQ(a.get_vector().size(), NI * NJ);

  // newly created arrays are filled with zeros
  for (auto &v : a.get_vector())
    EXPECT_FLOAT_EQ(v, 0.f);

  // =
  a = 1.f;
  for (auto &v : a.get_vector())
    EXPECT_FLOAT_EQ(v, 1.f);

  {
    hmap::Array b = hmap::Array(shape);
    a = b;
    for (auto &v : a.get_vector())
      EXPECT_FLOAT_EQ(v, 0.f);
  }
}

TEST(ArrayClass, OverloadsMultiply)
{
  const std::vector<int> shape = {NI, NJ};
  hmap::Array            a = hmap::Array(shape);

  a = 1.f;
  a = 2.f * a;
  a = a * 2.f;
  for (auto &v : a.get_vector())
    EXPECT_FLOAT_EQ(v, 4.f);

  {
    hmap::Array b = hmap::Array(shape);
    a = 1.f;
    b = 2.f;
    a = a * b;
    for (auto &v : a.get_vector())
      EXPECT_FLOAT_EQ(v, 2.f);
  }
}

TEST(ArrayClass, OverloadsDivide)
{
  const std::vector<int> shape = {NI, NJ};
  hmap::Array            a = hmap::Array(shape);

  a = 0.5f;
  a = 1.f / a / 2.f;
  for (auto &v : a.get_vector())
    EXPECT_FLOAT_EQ(v, 1.f);

  {
    hmap::Array b = hmap::Array(shape);
    a = 1.f;
    b = 2.f;
    a = a / b;
    for (auto &v : a.get_vector())
      EXPECT_FLOAT_EQ(v, 0.5f);
  }
}

TEST(ArrayClass, OverloadsAdd)
{
  const std::vector<int> shape = {NI, NJ};
  hmap::Array            a = hmap::Array(shape);

  a = 1.f;
  a = 1.f + a + 1.f;
  for (auto &v : a.get_vector())
    EXPECT_FLOAT_EQ(v, 3.f);

  {
    hmap::Array b = hmap::Array(shape);
    a = 1.f;
    b = 1.f;
    a = a + b;
    for (auto &v : a.get_vector())
      EXPECT_FLOAT_EQ(v, 2.f);
  }
}

TEST(ArrayClass, OverloadsSub)
{
  const std::vector<int> shape = {NI, NJ};
  hmap::Array            a = hmap::Array(shape);

  a = 1.f;
  a = 1.f - a - 1.f;
  for (auto &v : a.get_vector())
    EXPECT_FLOAT_EQ(v, -1.f);

  a = -a;
  for (auto &v : a.get_vector())
    EXPECT_FLOAT_EQ(v, 1.f);

  {
    hmap::Array b = hmap::Array(shape);
    a = 1.f;
    b = 1.f;
    a = a - b;
    for (auto &v : a.get_vector())
      EXPECT_FLOAT_EQ(v, 0.f);
  }
}
