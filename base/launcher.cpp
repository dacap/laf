// LAF Base Library
// Copyright (c) 2018-2021  Igara Studio S.A.
// Copyright (c) 2001-2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "base/exception.h"
#include "base/fs.h"
#include "base/launcher.h"
#include "base/string.h"

#include <cstdlib>

#if LAF_WINDOWS
  #include <windows.h>
  #ifndef SEE_MASK_DEFAULT
    #define SEE_MASK_DEFAULT 0x00000000
  #endif

static int win32_shell_execute(const wchar_t* verb, const wchar_t* file, const wchar_t* params)
{
  SHELLEXECUTEINFO sh;
  ZeroMemory((LPVOID)&sh, sizeof(sh));
  sh.cbSize = sizeof(sh);
  sh.fMask = SEE_MASK_DEFAULT;
  sh.lpVerb = verb;
  sh.lpFile = file;
  sh.lpParameters = params;
  sh.nShow = SW_SHOWNORMAL;

  if (!ShellExecuteEx(&sh)) {
    int ret = GetLastError();
  #if 0
    if (ret != 0) {
      DWORD flags =
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS;
      LPSTR msgbuf;

      if (FormatMessageA(flags, NULL, ret,
                         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                         reinterpret_cast<LPSTR>(&msgbuf),
                         0, NULL)) {
        ui::Alert::show("Problem<<Cannot open:<<%s<<%s||&Close", file, msgbuf);
        LocalFree(msgbuf);

        ret = 0;
      }
    }
  #endif
    return ret;
  }
  else
    return 0;
}
#endif // LAF_WINDOWS

namespace base { namespace launcher {

bool open_url(const std::string& url)
{
  return open_file(url);
}

bool open_file(const std::string& file)
{
  int ret = -1;

#if LAF_WINDOWS

  ret = win32_shell_execute(L"open", base::from_utf8(file).c_str(), NULL);

#elif HAVE_SYSTEM

  #if __APPLE__
  ret = std::system(("open \"" + file + "\"").c_str());
  #else
  ret = std::system(("setsid xdg-open \"" + file + "\"").c_str());
  #endif

#endif

  return (ret == 0);
}

bool open_folder(const std::string& _file)
{
  std::string file = base::fix_path_separators(_file);

#if LAF_WINDOWS

  int ret;
  if (base::is_directory(file)) {
    ret =
      win32_shell_execute(NULL, L"explorer", (L"/n,/e,\"" + base::from_utf8(file) + L"\"").c_str());
  }
  else {
    ret = win32_shell_execute(NULL,
                              L"explorer",
                              (L"/e,/select,\"" + base::from_utf8(file) + L"\"").c_str());
  }
  return (ret == 0);

#elif HAVE_SYSTEM

  #if __APPLE__

  int ret;
  if (base::is_directory(file))
    ret = std::system(("open \"" + file + "\"").c_str());
  else
    ret = std::system(("open --reveal \"" + file + "\"").c_str());
  return (ret == 0);

  #else

  if (!base::is_directory(file))
    file = base::get_file_path(file);

  const int ret = std::system(("setsid xdg-open \"" + file + "\"").c_str());
  return (ret == 0);

  #endif

#else // HAVE_SYSTEM

  return false;

#endif
}

}} // namespace base::launcher
