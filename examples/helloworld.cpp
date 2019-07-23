// LAF Library
// Copyright (c) 2019  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "os/os.h"

void draw_display(os::Display* display)
{
  os::Surface* surface = display->getSurface();
  os::SurfaceLock lock(surface);
  const gfx::Rect rc(0, 0, surface->width(), surface->height());
  surface->fillRect(gfx::rgba(0, 0, 0), rc);
  surface->drawLine(gfx::rgba(255, 0, 0), gfx::Point(0     , 0), gfx::Point(  rc.w, rc.h));
  surface->drawLine(gfx::rgba(0, 128, 0), gfx::Point(rc.w/2, 0), gfx::Point(rc.w/2, rc.h));
  surface->drawLine(gfx::rgba(0, 0, 255), gfx::Point(rc.w  , 0), gfx::Point(     0, rc.h));

  os::Paint paint;
  paint.color(gfx::rgba(255, 255, 255));
  paint.setFlags(os::Paint::kCenterAlign);
  surface->drawText("Hello World", rc.center(), &paint);

  // Invalidates the whole display to show it on the screen.
  display->invalidateRegion(gfx::Region(rc));
}

int app_main(int argc, char* argv[])
{
  const int pixelScale = 2;

  os::ScopedHandle<os::System> system(os::create_system());
  os::ScopedHandle<os::Display> display(system->createDisplay(400, 300, pixelScale));

  display->setNativeMouseCursor(os::kArrowCursor);
  display->setTitleBar("Hello World");

  // On macOS: With finishLaunching() we start processing
  // NSApplicationDelegate events. After calling this we'll start
  // receiving os::Event::DropFiles events. It's a way to say "ok
  // we're ready to process messages"
  system->finishLaunching();

  // On macOS, when we compile the program outside an app bundle, we
  // must active the app explicitly if we want to put the app on the
  // front. Remove this if you're planning to distribute your app on a
  // bundle or enclose it in something like #ifdef _DEBUG/#endif
  system->activateApp();

  // Wait until a key is pressed or the window is closed
  os::EventQueue* queue = system->eventQueue();
  bool running = true;
  bool redraw = true;
  while (running) {
    if (redraw) {
      redraw = false;
      draw_display(display);
    }
    // Wait for an event in the queue, the "true" parameter indicates
    // that we'll wait for a new event, and the next line will not be
    // processed until we receive a new event. If we use "false" and
    // there is no events in the queue, we receive an "ev.type() == Event::None
    os::Event ev;
    queue->getEvent(ev, true);

    switch (ev.type()) {

      case os::Event::CloseDisplay:
        running = false;
        break;

      case os::Event::KeyDown:
        switch (ev.scancode()) {
          case os::kKeyEsc:
            running = false;
            break;
          case os::kKey1:
          case os::kKey2:
          case os::kKey3:
          case os::kKey4:
          case os::kKey5:
          case os::kKey6:
          case os::kKey7:
          case os::kKey8:
          case os::kKey9:
            // Set scale
            display->setScale(1 + (int)(ev.scancode() - os::kKey1));
            redraw = true;
            break;
        }
        break;

      case os::Event::ResizeDisplay:
        redraw = true;
        break;

      default:
        // Do nothing
        break;
    }
  }

  return 0;
}