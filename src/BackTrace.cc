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

#ifdef USE_BACKTRACE
#include "BackTrace.h"

BackTrace BackTrace::landmine;

void BackTrace::Dump(const char *event, int skip) {
  void *stack[50] = {0};
  int count = 0, cur = 0;
  
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

void BackTrace::Close() {
  _segv.sa_handler = SIG_DFL;
  _bus.sa_handler = SIG_DFL;
  _abrt.sa_handler = SIG_DFL;
  _ill.sa_handler = SIG_DFL;
  
  sigaction(SIGSEGV, &_segv, 0);
  sigaction(SIGBUS, &_bus, 0);
  sigaction(SIGABRT, &_abrt, 0);
  sigaction(SIGILL, &_ill, 0);
}

void BackTrace::OnSegv(int sig) {
  BackTrace::Dump("SIGSEGV", 2);
  landmine.Close();
}

void BackTrace::OnBus(int sig) {
  BackTrace::Dump("SIGBUS", 2);
  landmine.Close();
}

void BackTrace::OnAbort(int sig) {
  BackTrace::Dump("SIGABRT", 2);
  landmine.Close();
}

void BackTrace::OnIll(int sig) {
  BackTrace::Dump("SIGILL", 2);
  landmine.Close();
}

BackTrace::BackTrace() {
  sigemptyset(&_segv.sa_mask);
  sigemptyset(&_bus.sa_mask);
  sigemptyset(&_abrt.sa_mask);
  sigemptyset(&_ill.sa_mask);

  _segv.sa_flags = 0;
  _segv.sa_handler = BackTrace::OnSegv;

  _bus.sa_flags = 0;
  _bus.sa_handler = BackTrace::OnBus;

  _abrt.sa_flags = 0;
  _abrt.sa_handler = BackTrace::OnAbort; 
  
  _ill.sa_flags = 0;
  _ill.sa_handler = BackTrace::OnIll; 

  sigaction(SIGSEGV, &_segv, 0);
  sigaction(SIGBUS, &_bus, 0);
  sigaction(SIGABRT, &_abrt, 0);
  sigaction(SIGILL, &_ill, 0);
}

BackTrace::~BackTrace() {
  BackTrace::Close();
}

#endif
