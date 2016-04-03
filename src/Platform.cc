
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

#include "Platform.h"

#if defined(WEBRTC_WIN)
#include <webrtc/base/win32socketinit.h>
#include <webrtc/base/win32socketserver.h>
#endif

using namespace WebRTC;

#ifndef WEBRTC_THREAD_COUNT
#define WEBRTC_THREAD_COUNT 4
#endif

rtc::Thread signal_thread;
rtc::Thread worker_thread[WEBRTC_THREAD_COUNT];
uint32_t counter = 0;

void Platform::Init() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
#if defined(WEBRTC_WIN)
  rtc::EnsureWinsockInit();
#endif
  
  rtc::InitializeSSL();
   
  signal_thread.Start();
  
  rtc::ThreadManager::Instance()->SetCurrentThread(&signal_thread);
  
  if (rtc::ThreadManager::Instance()->CurrentThread() != &signal_thread) {
    Nan::ThrowError("Internal Thread Error!");
  }
  
  for (int index = 0; index < WEBRTC_THREAD_COUNT; index++) {
    worker_thread[index].Start();
  }
}

void Platform::Dispose() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  signal_thread.SetAllowBlockingCalls(true);
  signal_thread.Stop();
  
  for (int index = 0; index < WEBRTC_THREAD_COUNT; index++) {
    worker_thread[index].SetAllowBlockingCalls(true);
    worker_thread[index].Stop();
  }

  if (rtc::ThreadManager::Instance()->CurrentThread() == &signal_thread) {
    rtc::ThreadManager::Instance()->SetCurrentThread(NULL);
  }
  
  rtc::CleanupSSL();
}

rtc::Thread *Platform::GetWorker() {
  return &worker_thread[(counter++) % WEBRTC_THREAD_COUNT];
}