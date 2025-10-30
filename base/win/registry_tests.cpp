// LAF Base Library
// Copyright (c) 2024-2025 Igara Studio S.A.
// Copyright (c) 2017 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/win/registry.h"
#include "base/win/win32_exception.h"

using namespace base;

TEST(Registry, OpenKey)
{
  try {
    hkey k = hkey::classes_root();
    k = k.open(".txt", hkey::read);
    EXPECT_TRUE(k.string("") == "txtfile" || k.string("") == "txtfilelegacy");
    EXPECT_EQ("text/plain", k.string("Content Type"));
  }
  catch (Win32Exception& ex) {
    printf("Win32Exception: %s\nError Code: %d\n", ex.what(), ex.errorCode());
    throw;
  }
  catch (const std::exception& ex) {
    printf("std::exception: %s\n", ex.what());
    throw;
  }
}

TEST(Registry, CreateKey)
{
  try {
    hkey hkcu = hkey::current_user();
    hkey k = hkcu.create("Software\\Classes\\.laf-base-test-extension");
    k.string("", "testing");
    k.string("A", "value A");
    k.string("B", "value B");
    k.dword("C", 32);
    k.dword("D", 64);

    EXPECT_EQ("value A", k.string("A"));
    EXPECT_EQ("value B", k.string("B"));
    EXPECT_EQ(32, k.dword("C"));
    EXPECT_EQ(64, k.dword("D"));

    // We cannot use k.delete_tree("") because it does delete the
    // whole tree, but leaves the root key untouched.

    hkcu.delete_tree("Software\\Classes\\.laf-base-test-extension");
  }
  catch (Win32Exception& ex) {
    printf("Win32Exception: %s\nError Code: %d\n", ex.what(), ex.errorCode());
    throw;
  }
  catch (const std::exception& ex) {
    printf("std::exception: %s\n", ex.what());
    throw;
  }
}

TEST(Registry, DeleteValue)
{
  try {
    hkey hkcu = hkey::current_user();
    hkey k = hkcu.create("Software\\Classes\\.laf-base-test-delete-value");
    k.string("A", "value A");
    k.string("B", "");
    k.dword("C", 2);

    EXPECT_TRUE(k.exists("A"));
    EXPECT_TRUE(k.exists("B"));
    EXPECT_TRUE(k.exists("C"));
    EXPECT_EQ("value A", k.string("A"));
    EXPECT_EQ("", k.string("B"));
    EXPECT_EQ(2, k.dword("C"));

    k.delete_value("A");
    k.delete_value("B");
    k.delete_value("C");

    EXPECT_FALSE(k.exists("A"));
    EXPECT_FALSE(k.exists("B"));
    EXPECT_FALSE(k.exists("C"));
    EXPECT_EQ("", k.string("A"));
    EXPECT_EQ("", k.string("B"));
    EXPECT_EQ(0, k.dword("C"));

    hkcu.delete_tree("Software\\Classes\\.laf-base-test-delete-value");
  }
  catch (Win32Exception& ex) {
    printf("Win32Exception: %s\nError Code: %d\n", ex.what(), ex.errorCode());
    throw;
  }
  catch (const std::exception& ex) {
    printf("std::exception: %s\n", ex.what());
    throw;
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
