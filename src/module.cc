/*
* The MIT License (MIT)
*
* Copyright (c) 2017 vmolsa <ville.molsa@gmail.com> (http://github.com/vmolsa)
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

#include <nan.h>
#include <crtc.h>
#include <iostream>

#include "rtcpeerconnection.h"
#include "rtcdatachannel.h"
#include "mediastream.h"
#include "mediastreamtrack.h"

#include "v8-promise.h"

using namespace v8;

uv_async_t async;
uv_idle_t idle;

void WebRTCModuleInit(v8::Handle<v8::Object> exports) {
  crtc::Module::Init();

  uv_idle_init(uv_default_loop(), &idle);
  uv_idle_start(&idle, [](uv_idle_t* handle) {
    if (crtc::Module::DispatchEvents(false)) {
      uv_ref((uv_handle_t*) &async);
    } else {
      uv_unref((uv_handle_t*) &async);
    }
  });

  uv_async_init(uv_default_loop(), &async, [](uv_async_t* handle) {
    if (crtc::Module::DispatchEvents(false)) {
      uv_ref((uv_handle_t*) &async);
    } else {
      uv_unref((uv_handle_t*) &async);
    }
  });

  uv_unref((uv_handle_t*) &idle);

  crtc::Module::RegisterAsyncCallback([]() {
    uv_async_send(&async);
  });

  Nan::HandleScope scope;

  WebRTC::RTCPeerConnection::Init(exports);
  WebRTC::RTCDataChannel::Init(exports);
  WebRTC::MediaStream::Init(exports);
  WebRTC::MediaStreamTrack::Init(exports);

  node::AtExit([](void *arg) {
    crtc::Module::Dispose();
    uv_close((uv_handle_t*) &async, nullptr);
    uv_idle_stop(&idle);
  });
}

NODE_MODULE(webrtc_native, WebRTCModuleInit)
