// LAF OS Library
// Copyright (c) 2020-2025  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_X11_SCREEN_H
#define OS_X11_SCREEN_H
#pragma once

#include "os/screen.h"

namespace os {

class ScreenX11 : public Screen {
public:
  ScreenX11(int monitorNum);

  bool isMainScreen() const override { return m_isPrimary; }
  gfx::Rect bounds() const override { return m_bounds; }
  gfx::Rect workarea() const override { return m_workarea; }

  os::ColorSpaceRef colorSpace() const override;

  void* nativeHandle() const override { return reinterpret_cast<void*>(m_monitorNum); }

private:
  int m_monitorNum = 0;
  bool m_isPrimary = false;
  gfx::Rect m_bounds;
  gfx::Rect m_workarea;
};

} // namespace os

#endif
