// LAF Base Library
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "base/split_string.h"

TEST(SplitString, Empty)
{
  std::vector<std::string> result;
  base::split_string("", result, ",");
  ASSERT_EQ(1, result.size());
  EXPECT_EQ("", result[0]);
}

TEST(SplitString, NoSeparator)
{
  std::vector<std::string> result;
  base::split_string("Hello,World", result, "");
  ASSERT_EQ(1, result.size());
  EXPECT_EQ("Hello,World", result[0]);
}

TEST(SplitString, OneSeparator)
{
  std::vector<std::string> result;
  base::split_string("Hello,World", result, ",");
  ASSERT_EQ(2, result.size());
  EXPECT_EQ("Hello", result[0]);
  EXPECT_EQ("World", result[1]);
}

TEST(SplitString, MultipleSeparators)
{
  std::string string = "Hello,World";
  std::vector<std::string> result;
  base::split_string(string, result, ",r");
  ASSERT_EQ(3, result.size());
  EXPECT_EQ("Hello", result[0]);
  EXPECT_EQ("Wo", result[1]);
  EXPECT_EQ("ld", result[2]);
}

TEST(SplitStringView, Empty)
{
  const std::string_view string = "";
  std::vector<std::string_view> result;
  base::split_string("", result, ",");
  ASSERT_EQ(1, result.size());
  EXPECT_EQ("", result[0]);
}

TEST(SplitStringView, NoSeparator)
{
  const std::string_view string = "Hello,World";
  std::vector<std::string_view> result;
  base::split_string(string, result, "");
  ASSERT_EQ(1, result.size());
  EXPECT_EQ("Hello,World", result[0]);
}

TEST(SplitStringView, OneSeparator)
{
  const std::string_view string = "Hello,World";
  std::vector<std::string_view> result;
  base::split_string("Hello,World", result, ",");
  ASSERT_EQ(2, result.size());
  EXPECT_EQ("Hello", result[0]);
  EXPECT_EQ("World", result[1]);
}

TEST(SplitStringView, MultipleSeparators)
{
  const std::string_view string = "Hello,World";
  std::vector<std::string_view> result;
  base::split_string("Hello,World", result, ",r");
  ASSERT_EQ(3, result.size());
  EXPECT_EQ("Hello", result[0]);
  EXPECT_EQ("Wo", result[1]);
  EXPECT_EQ("ld", result[2]);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
