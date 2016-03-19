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

#include "Common.h"

#include "Global.h"
#include "Platform.h"
#include "Stats.h"
#include "PeerConnection.h"
#include "DataChannel.h"
#include "BackTrace.h"
#include "GetSources.h"
#include "GetUserMedia.h"
#include "MediaStream.h"
#include "MediaStreamTrack.h"

using namespace v8;

void SetDebug(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (info.Length() && !info[0].IsEmpty()) {
    if (info[0]->IsTrue()) {
      rtc::LogMessage::LogToDebug(rtc::LS_VERBOSE);
    } else {
      rtc::LogMessage::LogToDebug(rtc::LS_NONE);
    }
  }

  info.GetReturnValue().SetUndefined();
}

void RTCGarbageCollect(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::LowMemoryNotification();
  info.GetReturnValue().SetUndefined();
}

void RTCIceCandidate(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (info.Length() == 1 && info[0]->IsObject() && info.IsConstructCall()) {
    Local<Object> arg = info[0]->ToObject();
    Local<Object> retval = Nan::New<Object>();
    
    retval->Set(Nan::New("candidate").ToLocalChecked(), arg->Get(Nan::New("candidate").ToLocalChecked()));               
    retval->Set(Nan::New("sdpMLineIndex").ToLocalChecked(), arg->Get(Nan::New("sdpMLineIndex").ToLocalChecked()));
    retval->Set(Nan::New("sdpMid").ToLocalChecked(), arg->Get(Nan::New("sdpMid").ToLocalChecked()));
    
    return info.GetReturnValue().Set(retval);
  } else {
    return info.GetReturnValue().Set(info[0]);
  }
}

void RTCSessionDescription(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (info.Length() == 1 && info[0]->IsObject() && info.IsConstructCall()) {
    Local<Object> arg = info[0]->ToObject();
    Local<Object> retval = Nan::New<Object>();
    
    retval->Set(Nan::New("type").ToLocalChecked(), arg->Get(Nan::New("type").ToLocalChecked()));
    retval->Set(Nan::New("sdp").ToLocalChecked(), arg->Get(Nan::New("sdp").ToLocalChecked()));

    return info.GetReturnValue().Set(retval);
  } else {
    return info.GetReturnValue().Set(info[0]);
  }
}

void WebrtcModuleDispose(void *arg) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  WebRTC::Platform::Dispose();
}

void WebrtcModuleInit(Handle<Object> exports) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;

  WebRTC::Global::Init(exports);
  WebRTC::Platform::Init();
  WebRTC::RTCStatsResponse::Init();
  WebRTC::RTCStatsReport::Init();
  WebRTC::PeerConnection::Init(exports);
  WebRTC::DataChannel::Init();
  WebRTC::GetSources::Init(exports);
  WebRTC::GetUserMedia::Init(exports);
  WebRTC::MediaStream::Init();
  WebRTC::MediaStreamTrack::Init();
  
  exports->Set(Nan::New("RTCGarbageCollect").ToLocalChecked(), Nan::New<FunctionTemplate>(RTCGarbageCollect)->GetFunction()); 
  exports->Set(Nan::New("RTCIceCandidate").ToLocalChecked(), Nan::New<FunctionTemplate>(RTCIceCandidate)->GetFunction());
  exports->Set(Nan::New("RTCSessionDescription").ToLocalChecked(), Nan::New<FunctionTemplate>(RTCSessionDescription)->GetFunction());
  exports->Set(Nan::New("setDebug").ToLocalChecked(), Nan::New<FunctionTemplate>(SetDebug)->GetFunction());

  node::AtExit(WebrtcModuleDispose);
}

NODE_MODULE(webrtc, WebrtcModuleInit)