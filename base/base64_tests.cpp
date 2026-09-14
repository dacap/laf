// LAF Base Library
// Copyright (c) 2022-present Igara Studio S.A.
// Copyright (c) 2015-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/base64.h"
#include "base/string.h"

#include <string_view>

using namespace base;

TEST(Base64, Encode)
{
  EXPECT_EQ("", encode_base64(buffer()));
  EXPECT_EQ("Cg==", encode_base64(buffer{ '\n' }));
  EXPECT_EQ("YQ==", encode_base64(buffer{ 'a' }));
  EXPECT_EQ("YWJjZGU=", encode_base64(buffer{ 'a', 'b', 'c', 'd', 'e' }));
  EXPECT_EQ("YWJjZGU=", encode_base64("abcde"));
  EXPECT_EQ("YWJj", encode_base64("abc"));
  EXPECT_EQ("5pel5pys6Kqe", encode_base64("\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E")); // "日本語"
}

TEST(Base64, Decode)
{
  EXPECT_EQ(buffer(), decode_base64(""));
  EXPECT_EQ(buffer{ '\n' }, decode_base64("Cg=="));
  EXPECT_EQ(buffer{ 'a' }, decode_base64("YQ=="));
  EXPECT_EQ(buffer({ 'a', 'b', 'c', 'd', 'e' }), decode_base64("YWJjZGU="));
  EXPECT_EQ("abcde", decode_base64s("YWJjZGU="));
  EXPECT_EQ("abc", decode_base64s("YWJj"));
  EXPECT_EQ("\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E", decode_base64s("5pel5pys6Kqe")); // "日本語"
}

TEST(Base64, EncodeDecode)
{
  buffer tests[] = {
    { 0 },
    { 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 2, 0 },
    { 0, 1, 2, 0, 0 },
    { 0, 1, 2, 3, 0, 0 },
    { 0, 1, 2, 3, 0, 0, 0 },
    { 0, 1, 2, 0 },
    { 0, 1, 2, 0, 0 },
    { 0, 1, 2, 3, 0, 0 },
    { 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 255 },
    { 0, 1, 2, 3, 0, 0, 0, 1, 2, 3, 254, 255 },
  };

  for (const buffer& test : tests) {
    auto encoded = encode_base64(test);
    EXPECT_EQ(0, encoded.size() % 4);

    // Check that there is no buggy '===' padding
    auto sv = std::string_view((const char*)encoded.data(), encoded.size());
    EXPECT_EQ(std::string_view::npos, sv.find("===")) << " encoded= " << encoded;

    auto decoded = decode_base64(encoded);
    EXPECT_EQ(decoded, test);

    auto encoded2 = encode_base64(decoded);
    EXPECT_EQ(encoded2, encoded);
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
