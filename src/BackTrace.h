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

#ifndef BACKTRACE_H
#define BACKTRACE_H
#ifdef USE_BACKTRACE

#include <stdio.h>
#include <signal.h>
#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>

class BackTrace {
 public:
  static void Dump(const char *event = "NOEVENT", int skip = 1);
  
 private:
  explicit BackTrace();
  virtual ~BackTrace();
  
  void Close();
  
  static void OnSegv(int sig);
  static void OnBus(int sig);
  static void OnAbort(int sig);
  static void OnIll(int sig);

 protected:
  struct sigaction _segv;
  struct sigaction _bus;
  struct sigaction _abrt;
  struct sigaction _ill;
  
  static BackTrace landmine;
};

#else

#endif
#endif
