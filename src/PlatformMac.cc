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

using namespace WebRTC;

class PlatformWorker : public rtc::Thread {
  public:
    PlatformWorker(rtc::SocketServer *server) : rtc::Thread(server) {
      LOG(LS_INFO) << __PRETTY_FUNCTION__;
    }
    
    virtual void Run() {
      rtc::Thread::ProcessMessages(0);
    }
};

rtc::MacCocoaSocketServer server;
PlatformWorker worker(&server);
uv_prepare_t runLoop;

#if (UV_VERSION_MAJOR > 0)
void OnRunLoop(uv_prepare_t *handle) {
#else
void OnRunLoop(uv_prepare_t *handle, int unused) {
#endif
  worker.Run();
}

void Platform::Init() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::InitCocoaMultiThreading();
  
  uv_prepare_init(uv_default_loop(), &runLoop);
  uv_prepare_start(&runLoop, OnRunLoop);
  uv_unref(reinterpret_cast<uv_handle_t*>(&runLoop));
}

void Platform::Dispose() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  uv_prepare_stop(&runLoop);
}
