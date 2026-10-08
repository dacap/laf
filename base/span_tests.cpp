// LAF Base Library
// Copyright (c) 2026-present  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/span.h"

TEST(Span, Empty)
{
  base::span<int> empty;
  EXPECT_TRUE(empty.empty());
  EXPECT_EQ(0, empty.size());
}

TEST(Span, PtrCtor)
{
  int p[] = { 3, 2, 0, 1 };
  base::span<int> ints(p, 4);
  EXPECT_FALSE(ints.empty());
  EXPECT_EQ(4, ints.size());
  EXPECT_EQ(3, ints[0]);
  EXPECT_EQ(2, ints[1]);
  EXPECT_EQ(0, ints[2]);
  EXPECT_EQ(1, ints[3]);

  EXPECT_NO_THROW(ints.at(3));
  EXPECT_THROW(ints.at(4), std::out_of_range);
}

TEST(Span, AssignOp)
{
  int a[] = { 3, 2, 0, 1 };
  int b[] = { 4, 1, 3, 5 };
  base::span<int> x(a, a + 4);
  base::span<int> y(b, b + 4);

  EXPECT_EQ(3, x[0]);
  EXPECT_EQ(2, x[1]);
  EXPECT_EQ(0, x[2]);
  EXPECT_EQ(1, x[3]);
  x = y;
  EXPECT_EQ(4, x[0]);
  EXPECT_EQ(1, x[1]);
  EXPECT_EQ(3, x[2]);
  EXPECT_EQ(5, x[3]);
}

TEST(Span, Iterators)
{
  int p[] = { 4, 1, 3, 5 };
  base::span<int> x(p, p + 4);

  {
    auto it = x.begin();
    for (int i = 0; i < 4; ++i, ++it)
      EXPECT_EQ(p[i], *it);
    EXPECT_EQ(it, x.end());
  }

  {
    auto it = x.rbegin();
    for (int i = 0; i < 4; ++i, ++it)
      EXPECT_EQ(p[3 - i], *it);
    EXPECT_EQ(it, x.rend());
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
