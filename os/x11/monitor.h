#ifndef OS_X11_MONITOR_INCLUDED
#define OS_X11_MONITOR_INCLUDED
#pragma once

#include <X11/extensions/Xrandr.h>
#include <memory>

namespace os {

struct MonitorCloser {
  void operator()(XRRMonitorInfo* monitors) const noexcept { XRRFreeMonitors(monitors); }
};

typedef std::unique_ptr<XRRMonitorInfo, MonitorCloser> unique_monitors_ptr;

class MonitorsX11 {
public:
  MonitorsX11();
  int numMonitors() const;
  const XRRMonitorInfo& monitor(int monitorNum) const;

private:
  int m_numMonitors;
  unique_monitors_ptr m_monitors;
};

} // namespace os

#endif // OS_X11_MONITOR_INCLUDED
