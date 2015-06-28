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

#include <nan.h>

#include "Core.h"
#include "Stats.h"
#include "PeerConnection.h"
#include "DataChannel.h"
#include "BackTrace.h"
#include "GetSources.h"
#include "GetUserMedia.h"
#include "MediaStream.h"
#include "MediaStreamTrack.h"

using namespace v8;

NAN_METHOD(SetDebug) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();

  if (args.Length() && !args[0].IsEmpty()) {
    if (args[0]->IsTrue()) {
      rtc::LogMessage::LogToDebug(rtc::LS_VERBOSE);
    } else {
      rtc::LogMessage::LogToDebug(rtc::LS_NONE);
    }
  }

  NanReturnUndefined();
}

NAN_METHOD(RTCGarbageCollect) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
#if (NODE_MODULE_VERSION < NODE_0_12_MODULE_VERSION)
  V8::LowMemoryNotification();
#else
  Isolate* isolate = args.GetIsolate();
  isolate->LowMemoryNotification();
#endif

  NanReturnUndefined();
}

NAN_METHOD(RTCIceCandidate) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  if (args.Length() == 1 && args[0]->IsObject() && args.IsConstructCall()) {
    Local<Object> arg = args[0]->ToObject();
    Local<Object> retval = NanNew<Object>();
    
    retval->Set(NanNew("candidate"), arg->Get(NanNew("candidate")));               
    retval->Set(NanNew("sdpMLineIndex"), arg->Get(NanNew("sdpMLineIndex")));
    retval->Set(NanNew("sdpMid"), arg->Get(NanNew("sdpMid")));
    
    NanReturnValue(retval);
  } else {
    NanReturnValue(args[0]);
  }
}

NAN_METHOD(RTCSessionDescription) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  if (args.Length() == 1 && args[0]->IsObject() && args.IsConstructCall()) {
    Local<Object> arg = args[0]->ToObject();
    Local<Object> retval = NanNew<Object>();
    
    retval->Set(NanNew("type"), arg->Get(NanNew("type")));
    retval->Set(NanNew("sdp"), arg->Get(NanNew("sdp")));

    NanReturnValue(retval);
  } else {
    NanReturnValue(args[0]);
  }
}

void WebrtcModuleDispose(void *arg) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  WebRTC::Core::Dispose();
}

void WebrtcModuleInit(Handle<Object> exports) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  NanScope();

  WebRTC::Core::Init();
  WebRTC::RTCStatsResponse::Init();
  WebRTC::RTCStatsReport::Init();
  WebRTC::PeerConnection::Init(exports);
  WebRTC::DataChannel::Init();
  WebRTC::GetSources::Init(exports);
  WebRTC::GetUserMedia::Init(exports);
  WebRTC::MediaStream::Init();
  WebRTC::MediaStreamTrack::Init();
  
  exports->Set(NanNew("RTCGarbageCollect"), NanNew<FunctionTemplate>(RTCGarbageCollect)->GetFunction()); 
  exports->Set(NanNew("RTCIceCandidate"), NanNew<FunctionTemplate>(RTCIceCandidate)->GetFunction());
  exports->Set(NanNew("RTCSessionDescription"), NanNew<FunctionTemplate>(RTCSessionDescription)->GetFunction());
  exports->Set(NanNew("setDebug"), NanNew<FunctionTemplate>(SetDebug)->GetFunction());

  node::AtExit(WebrtcModuleDispose);
}

NODE_MODULE(webrtc_native, WebrtcModuleInit)