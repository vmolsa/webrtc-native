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

#include "BackTrace.h"
#ifdef USE_BACKTRACE
#ifdef __APPLE__
#include <stdio.h>
#include <signal.h>
#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>

static struct sigaction actsegv, actbus, actabrt;

inline static void ShowBacktrace(const char *event) {
  void *stack[50] = {0};
  int count = 0, cur = 0, skip = 2;
  
  count = backtrace(stack, sizeof(stack));
  
  if (count) {
    for (int index = (count - 1); index > skip && index >= 1; index--) {
      char addr[128] = {0};
      Dl_info info;
     
      snprintf(addr, sizeof(addr), "%p", stack[index]);
     
      if (dladdr(stack[index], &info) && info.dli_sname) {
        char buffer[128] = {0};
        size_t len = sizeof(buffer);
        int status = 0;

        (void) abi::__cxa_demangle(info.dli_sname, buffer, &len, &status);
        
        if (!status) {
          printf("%d: %s [%s] %s\n", cur, event, addr, buffer);
        } else {
          printf("%d: %s [%s] %s\n", cur, event, addr, info.dli_sname);
        }
      } else {
        printf("%d: %s [%s] function()\n", cur, event, addr);
      }
      
      cur++;
    }
  }
}

inline static void CloseBacktrace() {
  actsegv.sa_handler = SIG_DFL;
  actbus.sa_handler = SIG_DFL;
  actabrt.sa_handler = SIG_DFL;
  
  sigaction(SIGSEGV, &actsegv, 0);
  sigaction(SIGBUS, &actbus, 0);
  sigaction(SIGABRT, &actabrt, 0);
}

inline static void onSegv(int sig) {
  ShowBacktrace("SIGSEGV");
  CloseBacktrace();
}

inline static void onBus(int sig) {
  ShowBacktrace("SIGBUS");
  CloseBacktrace();
}

inline static void onAbort(int sig) {
  ShowBacktrace("SIGABRT");
  CloseBacktrace();
}

BackTrace::BackTrace() {
  sigemptyset(&actsegv.sa_mask);
  sigemptyset(&actbus.sa_mask);
  sigemptyset(&actabrt.sa_mask);

  actsegv.sa_flags = 0;
  actsegv.sa_handler = onSegv;

  actbus.sa_flags = 0;
  actbus.sa_handler = onBus;

  actabrt.sa_flags = 0;
  actabrt.sa_handler = onAbort; 

  sigaction(SIGSEGV, &actsegv, 0);
  sigaction(SIGBUS, &actbus, 0);
  sigaction(SIGABRT, &actabrt, 0);
}

BackTrace::~BackTrace() {
  CloseBacktrace();
}

#endif
#endif