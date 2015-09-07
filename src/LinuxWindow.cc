/*
* The MIT License (MIT)
*
* Copyright (c) 2015 vmolsa <ville.molsa@gmail.com> (http://github.com/vmolsa)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
*/

#include "LinuxWindow.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <sys/time.h>

void *LinuxWindow::CreateWindow(int width, int height) {
  int screen;
  XEvent event;
  XSetWindowAttributes attr;
  XVisualInfo info;
  Window window = 0;
  Display* display = XOpenDisplay(NULL);
  unsigned long mask;
  
  if (display) {
    screen = DefaultScreen(display);
    
    (void) XMatchVisualInfo(display, screen, 24, TrueColor, &info);
    
    attr.colormap = XCreateColormap(display, DefaultRootWindow(display), info.visual, AllocNone);
    attr.event_mask = StructureNotifyMask | ExposureMask;
    attr.background_pixel = 0;
    attr.border_pixel = 0;

    mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
    
    window = XCreateWindow(display, DefaultRootWindow(display), 0, 0, width, height, 0, info.depth, InputOutput, info.visual, mask, &attr);    
  }
    
  if (window) {
    XStoreName(display, window, "WebRTC @ NodeJS");
    XSetIconName(display, window, "WebRTC @ NodeJS");
    XSelectInput(display, window, StructureNotifyMask);
    XMapWindow(display, window);
    
    do {
        XNextEvent(display, &event);
    } while (event.type != MapNotify || event.xmap.event != window);
    
    return (void*) window;
  } 
  
  return 0;
}
