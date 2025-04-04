// LAF Base Library
// Copyright (c) 2021 Igara Studio S.A.
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "base/memory_dump.h"

#if LAF_WINDOWS
  #include "base/memory_dump_win32.h"
#else
  #include "base/memory_dump_none.h"
#endif

namespace base {

MemoryDump::MemoryDump() : m_impl(new MemoryDumpImpl)
{
}

MemoryDump::~MemoryDump()
{
  delete m_impl;
}

void MemoryDump::setFileName(const std::string& fileName)
{
  m_impl->setFileName(fileName);
}

} // namespace base
