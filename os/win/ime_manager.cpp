// LAF OS Library
// Copyright (C) 2025  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "os/win/ime_manager.h"

#include <windows.h>

// Must be included after windows.h
#include <imm.h>

namespace os {

class ImmCtx {
public:
  ImmCtx(HWND hwnd) : m_hwnd(hwnd), m_imc(ImmGetContext(hwnd)) {}
  ~ImmCtx()
  {
    if (m_imc)
      ImmReleaseContext(m_hwnd, m_imc);
  }
  operator HIMC() { return m_imc; }

private:
  HWND m_hwnd;
  HIMC m_imc;
};

static IMEManagerWin g_imeManager;

IMEManagerWin* IMEManagerWin::instance()
{
  return &g_imeManager;
}

IMEManagerWin::IMEManagerWin()
{
  // Initialize the IME manager
  m_screenCaretPos = gfx::Point(0, 0);
  m_textInput = false;
}

void IMEManagerWin::onStartComposition(HWND hwnd) const
{
  ImmCtx imc(hwnd);
  if (!imc)
    return;

  RECT rc;
  GetClientRect(hwnd, &rc);
  ClientToScreen(hwnd, (LPPOINT)&rc);

  // Get caret relative position to the window
  POINT pos = {
    m_screenCaretPos.x - rc.left,
    m_screenCaretPos.y - rc.top,
  };

  // Set IME form position: just below the caret
  COMPOSITIONFORM cf = { 0 };
  cf.dwStyle = CFS_POINT;
  cf.ptCurrentPos = pos;
  ImmSetCompositionWindow(imc, &cf);
}

} // namespace os
