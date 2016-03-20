
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

#import <Cocoa/Cocoa.h>
#include "Platform.h"

using namespace WebRTC;

#ifndef WEBRTC_THREAD_COUNT
#define WEBRTC_THREAD_COUNT 4
#endif

uv_check_t mainLoop;
rtc::Thread signal_thread;
rtc::Thread worker_thread[WEBRTC_THREAD_COUNT];
rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory[WEBRTC_THREAD_COUNT];
uint32_t counter = 0;

void ProcessMessages(uv_check_t *handle) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  
  if (![[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.1]]) {
    Nan::ThrowError("Internal RunLoop Error!");
  }
  
  [pool release];
}

void Platform::Init() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  [NSApplication sharedApplication];
  
  uv_check_init(uv_default_loop(), &mainLoop);
  uv_check_start(&mainLoop, ProcessMessages);
  
  signal_thread.Start();
  
  rtc::InitializeSSL();
  rtc::ThreadManager::Instance()->SetCurrentThread(&signal_thread);
  
  if (rtc::ThreadManager::Instance()->CurrentThread() != &signal_thread) {
    Nan::ThrowError("Internal Thread Error!");
  }
  
  for (int index = 0; index < WEBRTC_THREAD_COUNT; index++) {
    worker_thread[index].Start();
    
    factory[index] = webrtc::CreatePeerConnectionFactory(&signal_thread, &worker_thread[index], 0, 0, 0);
  
    if (!factory[index].get()) {
      Nan::ThrowError("Internal Factory Error");
    }
  }
  
  [pool release];
}

void Platform::Dispose() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  uv_check_stop(&mainLoop);
  
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

webrtc::PeerConnectionFactoryInterface* Platform::GetFactory() {
  return factory[(counter++) % WEBRTC_THREAD_COUNT].get();
}