#include "PeerConnection.h"
#include "DataChannel.h"

#define USE_BACKTRACE 1

#include "BackTrace.h"

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
  
  exports->Set(String::NewFromUtf8(isolate, "RTCGarbageCollect"), 
               FunctionTemplate::New(isolate, RTCGarbageCollect)->GetFunction());
  
  exports->Set(String::NewFromUtf8(isolate, "RTCIceCandidate"), 
               FunctionTemplate::New(isolate, RTCIceCandidate)->GetFunction());
               
  exports->Set(String::NewFromUtf8(isolate, "RTCSessionDescription"), 
               FunctionTemplate::New(isolate, RTCSessionDescription)->GetFunction());
}

NODE_MODULE(webrtc, WebrtcModuleInit)