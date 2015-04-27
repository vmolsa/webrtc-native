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

#pragma warning( disable : 4005 )

#include <node.h>

#include "PeerConnection.h"
#include "DataChannel.h"
#include "BackTrace.h"
#include "GetSources.h"
#include "MediaStream.h"

using namespace v8;

class WebrtcInit {
 public:
  explicit WebrtcInit() {
    
    rtc::InitializeSSL(); 
  }
  
  ~WebrtcInit() {    
    rtc::CleanupSSL();
  }
};

WebrtcInit WebrtcMain;

void RTCGarbageCollect(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  isolate->LowMemoryNotification();
}

void RTCIceCandidate(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  
  if (args.Length() == 1 && args[0]->IsObject() && args.IsConstructCall()) {
    Local<Object> arg = args[0]->ToObject();
    Local<Object> retval = Object::New(isolate);
    
    retval->Set(String::NewFromUtf8(isolate, "candidate"), 
                arg->Get(String::NewFromUtf8(isolate, "candidate")));
                
    retval->Set(String::NewFromUtf8(isolate, "sdpMLineIndex"), 
                arg->Get(String::NewFromUtf8(isolate, "sdpMLineIndex")));
                
    retval->Set(String::NewFromUtf8(isolate, "sdpMid"), 
                arg->Get(String::NewFromUtf8(isolate, "sdpMid")));
    
    args.GetReturnValue().Set(retval);
  } else {
    args.GetReturnValue().Set(args[0]);
  }
}

void RTCSessionDescription(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  
  if (args.Length() == 1 && args[0]->IsObject() && args.IsConstructCall()) {
    Local<Object> arg = args[0]->ToObject();
    Local<Object> retval = Object::New(isolate);
    
    retval->Set(String::NewFromUtf8(isolate, "type"), 
                arg->Get(String::NewFromUtf8(isolate, "type")));
                
    retval->Set(String::NewFromUtf8(isolate, "sdp"), 
                arg->Get(String::NewFromUtf8(isolate, "sdp")));
    
    args.GetReturnValue().Set(retval);
  } else {
    args.GetReturnValue().Set(args[0]);
  }
}

void WebrtcModuleInit(Handle<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  
  WebRTC::PeerConnection::Init(exports);
  WebRTC::DataChannel::Init();
  WebRTC::GetSources::Init(exports);
  WebRTC::MediaStream::Init();
  
  exports->Set(String::NewFromUtf8(isolate, "RTCGarbageCollect"), 
               FunctionTemplate::New(isolate, RTCGarbageCollect)->GetFunction());
  
  exports->Set(String::NewFromUtf8(isolate, "RTCIceCandidate"), 
               FunctionTemplate::New(isolate, RTCIceCandidate)->GetFunction());
               
  exports->Set(String::NewFromUtf8(isolate, "RTCSessionDescription"), 
               FunctionTemplate::New(isolate, RTCSessionDescription)->GetFunction());
}

NODE_MODULE(webrtc, WebrtcModuleInit)