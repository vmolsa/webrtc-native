
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

#include "webrtc/system_wrappers/include/cpu_info.h"

#if defined(WEBRTC_WIN)
#include <webrtc/base/win32socketinit.h>
#include <webrtc/base/win32socketserver.h>
#endif

using namespace WebRTC;

uint32_t worker_count = 1, counter = 0;
rtc::Thread *signal_thread;
rtc::Thread *worker_thread;

void Platform::Init() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
#if defined(WEBRTC_WIN)
  rtc::EnsureWinsockInit();
#endif
  
  rtc::InitializeSSL();
   
  signal_thread = rtc::Thread::CreateWithSocketServer().release();
  signal_thread->SetAllowBlockingCalls(true);
  signal_thread->Start();
  
  rtc::ThreadManager::Instance()->SetCurrentThread(signal_thread);
  
  if (rtc::ThreadManager::Instance()->CurrentThread() != signal_thread) {
    Nan::ThrowError("Internal Thread Error!");
  }
  
#ifdef WEBRTC_THREAD_COUNT
  worker_count = WEBRTC_THREAD_COUNT;
#else
  worker_count = webrtc::CpuInfo::DetectNumberOfCores();
#endif

  worker_thread = new rtc::Thread[worker_count];
  
  for (uint32_t index = 0; index < worker_count; index++) {
    worker_thread[index].Start();
  }
}

void Platform::Dispose() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  signal_thread->SetAllowBlockingCalls(true);
  signal_thread->Stop();
  
  for (uint32_t index = 0; index < worker_count; index++) {
    worker_thread[index].SetAllowBlockingCalls(true);
    worker_thread[index].Stop();
  }

  if (rtc::ThreadManager::Instance()->CurrentThread() == signal_thread) {
    rtc::ThreadManager::Instance()->SetCurrentThread(NULL);
  }
  
  delete [] worker_thread;
  delete signal_thread;
  
  rtc::CleanupSSL();
}

rtc::Thread *Platform::GetWorker() {
  return &worker_thread[(counter++) % worker_count];
}