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