// LAF OS Library
// Copyright (c) 2020-2025  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "os/x11/screen.h"

#include "os/system.h"
#include "os/x11/monitor.h"
#include "os/x11/x11.h"

#include "gfx/rect_io.h"

#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>

namespace os {

ScreenX11::ScreenX11(const int monitorNum) : m_monitorNum(monitorNum)
{
  auto* x11 = X11::instance();
  auto* x11display = x11->display();
  ::Window root = XDefaultRootWindow(x11display);
  MonitorsX11* monitors = X11::instance()->monitors();
  int nmonitors = monitors->numMonitors();

  if (monitorNum >= 0 && monitorNum < nmonitors) {
    const XRRMonitorInfo& monitor = monitors->monitorInfo(monitorNum);

    m_bounds.x = monitor.x;
    m_bounds.y = monitor.y;
    m_bounds.w = monitor.width;
    m_bounds.h = monitor.height;
    m_workarea = m_bounds;
    m_isPrimary = (monitor.primary ? true : false);
  }
  else {
    const int screen = XDefaultScreen(x11display);
    nmonitors = 1; // Use _NET_WORKAREA
    m_bounds.x = m_bounds.y = 0;
    m_bounds.w = XDisplayWidth(x11display, screen);
    m_bounds.h = XDisplayHeight(x11display, screen);
    m_workarea = m_bounds;
    m_isPrimary = true;
  }

  Atom actual_type;
  int actual_format;
  unsigned long bytes_after;
  unsigned long nitems;
  gfx::Rect wa = m_bounds;

  // _NET_WORKAREA works correctly when we have just one monitor.
  if (nmonitors == 1) {
    Atom _NET_WORKAREA = XInternAtom(x11display, "_NET_WORKAREA", False);
    unsigned long* prop;
    int res = XGetWindowProperty(x11display,
                                 root,
                                 _NET_WORKAREA,
                                 0,
                                 4,
                                 False,
                                 XA_CARDINAL,
                                 &actual_type,
                                 &actual_format,
                                 &nitems,
                                 &bytes_after,
                                 (unsigned char**)&prop);
    if (res == Success && nitems == 4) {
      wa.x = prop[0];
      wa.y = prop[1];
      wa.w = prop[2];
      wa.h = prop[3];
      m_workarea = wa;
      XFree(prop);
    }
  }
  // It looks like there is no standard way to get the workarea
  // correctly on X11 when we have multiple monitors. So we try to use
  // the _GTK_WORKAREAS_D0 property which solves the issue for GNOME
  // desktops.
  else if (nmonitors >= 2) {
    Atom _GTK_WORKAREAS_D0 = XInternAtom(x11display, "_GTK_WORKAREAS_D0", False);
    unsigned long* prop;
    int res = XGetWindowProperty(x11display,
                                 root,
                                 _GTK_WORKAREAS_D0,
                                 0,
                                 4 * nmonitors,
                                 False,
                                 XA_CARDINAL,
                                 &actual_type,
                                 &actual_format,
                                 &nitems,
                                 &bytes_after,
                                 (unsigned char**)&prop);
    if (res == Success && nitems == 4 * nmonitors) {
      for (int i = 0; i < nmonitors; ++i) {
        wa.x = prop[4 * i];
        wa.y = prop[4 * i + 1];
        wa.w = prop[4 * i + 2];
        wa.h = prop[4 * i + 3];

        // Check if the given workarea is inside the monitor bounds,
        // in that case this is the workarea associated to the monitor.
        if (!(m_bounds & wa).isEmpty()) {
          m_workarea = wa;
          break;
        }
      }
      XFree(prop);
    }
  }
}

os::ColorSpaceRef ScreenX11::colorSpace() const
{
  // TODO get screen color space
  return System::instance()->makeColorSpace(gfx::ColorSpace::MakeSRGB());
}

} // namespace os
