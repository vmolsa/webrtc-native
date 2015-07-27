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

#include "ArrayBuffer.h"
#include "MediaSource.h"
#include "WebcamCapturer.h"
#include "MediaStreamCapturer.h"
#include "WindowRenderer.h"

using namespace v8;
using namespace WebRTC;

Persistent<Object> MediaSource::constructor;

void MediaSource::Init(Local<Object> exports) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(MediaSource::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew("MediaSource"));
  
  tpl->PrototypeTemplate()->Set(NanNew("connect"), NanNew<FunctionTemplate>(MediaSource::Connect)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("disconnect"), NanNew<FunctionTemplate>(MediaSource::Disconnect)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("write"), NanNew<FunctionTemplate>(MediaSource::Write)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("end"), NanNew<FunctionTemplate>(MediaSource::End)->GetFunction());

  tpl->InstanceTemplate()->SetAccessor(NanNew("ondata"),
                                       MediaSource::OnData,
                                       MediaSource::OnData);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("onerror"),
                                       MediaSource::OnError,
                                       MediaSource::OnError);
                                       
  tpl->InstanceTemplate()->SetAccessor(NanNew("onend"),
                                       MediaSource::OnEnd,
                                       MediaSource::OnEnd);
  
  exports->Set(NanNew("MediaSource"), tpl->GetFunction());
                 
  Local<Object> sources = NanNew<Object>();
  
  WebcamCapturer::Init(sources);
  MediaStreamCapturer::Init(sources);
  WindowRenderer::Init(sources);
  
  NanAssignPersistent(constructor, sources);
}

MediaSource::MediaSource() : _closing(false), _callback(false) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

MediaSource::~MediaSource() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  EventEmitter::RemoveAllListeners();
}

NAN_METHOD(MediaSource::New) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();

  if (args.IsConstructCall() && args.Length() >= 1 && args[0]->IsString()) {
    Local<String> type = args[0]->ToString();
    Local<Object> instances = NanNew(MediaSource::constructor);
    Local<Function> instance = Local<Function>::Cast(instances->Get(type));
    
    if (!instance.IsEmpty()) {
      Local<Object> properties = Local<Object>::Cast(args[1]);
      Local<Value> argv[1] = {
        properties
      };
      
      NanReturnValue(instance->Call(args.This(), 1, argv));
    }
  }
  
  NanReturnUndefined();
}

bool MediaSource::Connect(MediaSource *source) {
  if (source) {
    EventEmitter::AddListener(source);
    
    return true;
  }
  
  return false;
}

bool MediaSource::Disconnect(MediaSource *source) {
  if (source) {
    EventEmitter::RemoveListener(source);
    
    return true;
  }
  
  return false;
}

void MediaSource::End() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  if (!_closing) {
    _closing = true;
    
    EventEmitter::Emit(kMediaSourceEnd);
    EventEmitter::SetReference(false);
    
    Local<Function> callback = NanNew<Function>(_onend);
      
    if (!callback.IsEmpty()) {
      Local<Value> argv[1];
      callback->Call(RTCWrap::This(), 1, argv);
    }
  }
}

bool MediaSource::End(Local<Value> data) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  bool retval = true;
  
  if (!data.IsEmpty()) {
    retval = Write(data);
  }
  
  End();
  
  return retval;
}

bool MediaSource::Write(Local<Value> data) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  if (!data.IsEmpty() && data->IsObject()) {
    Local<Object> frame = Local<Object>::Cast(data);
    Local<Value> type_value = frame->Get(NanNew("type"));
      
    if (!type_value.IsEmpty() && type_value->IsString()) {
      String::Utf8Value type_utf8(type_value->ToString());
      std::string type(*type_utf8);
      
      if (!type.compare("frame")) {
        Local<Int32> width(frame->Get(NanNew("width"))->ToInt32());
        Local<Int32> height(frame->Get(NanNew("height"))->ToInt32());
        
        node::ArrayBuffer *ybuf = node::ArrayBuffer::New(frame->Get(NanNew("yplane")));
        node::ArrayBuffer *ubuf = node::ArrayBuffer::New(frame->Get(NanNew("uplane")));
        node::ArrayBuffer *vbuf = node::ArrayBuffer::New(frame->Get(NanNew("vplane")));

        if (width->Value() && height->Value()) {
          webrtc::VideoFrame videoFrame;
          
          int uv_plane = ((width->Value() + 1) / 2);
          videoFrame.CreateEmptyFrame(width->Value(), height->Value(), width->Value(), uv_plane, uv_plane);
          
          if (videoFrame.allocated_size(webrtc::kYPlane) == static_cast<int>(ybuf->Length()) && 
              videoFrame.allocated_size(webrtc::kUPlane) == static_cast<int>(ubuf->Length()) && 
              videoFrame.allocated_size(webrtc::kVPlane) == static_cast<int>(vbuf->Length())) 
          {
          
            memcpy(videoFrame.buffer(webrtc::kYPlane), ybuf->ToUtf8(), videoFrame.allocated_size(webrtc::kYPlane));
            memcpy(videoFrame.buffer(webrtc::kUPlane), ubuf->ToUtf8(), videoFrame.allocated_size(webrtc::kUPlane));
            memcpy(videoFrame.buffer(webrtc::kVPlane), vbuf->ToUtf8(), videoFrame.allocated_size(webrtc::kVPlane));
            
            rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer = videoFrame.video_frame_buffer();
            Emit(kMediaSourceFrame, buffer);
          
            return true;
          }
        }
      }
    }
  }
  
  return false;
}

NAN_METHOD(MediaSource::Connect) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.This(), "MediaSource");
  
  if (args.Length() >= 1 && args[0]->IsObject()) {
    NanReturnValue(NanNew(self->Connect(RTCWrap::Unwrap<MediaSource>(Local<Object>::Cast(args[0]), "MediaSource"))));
  }
  
  NanReturnValue(NanFalse());
}

NAN_METHOD(MediaSource::Disconnect) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.This(), "MediaSource");
  
  if (args.Length() >= 1 && args[0]->IsObject()) {
    NanReturnValue(NanNew(self->Disconnect(RTCWrap::Unwrap<MediaSource>(Local<Object>::Cast(args[0]), "MediaSource"))));
  }
  
  NanReturnValue(NanFalse());
}

NAN_METHOD(MediaSource::Write) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.This(), "MediaSource");
  NanReturnValue(NanNew(self->Write(args[0])));
}

NAN_METHOD(MediaSource::End) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.This(), "MediaSource");
  NanReturnValue(NanNew(self->End(args[0])));
}

NAN_GETTER(MediaSource::OnData) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.Holder(), "MediaSource");
  NanReturnValue(NanNew(self->_ondata));
}

NAN_GETTER(MediaSource::OnError) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.Holder(), "MediaSource");
  NanReturnValue(NanNew(self->_onerror));
}

NAN_GETTER(MediaSource::OnEnd) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.Holder(), "MediaSource");
  NanReturnValue(NanNew(self->_onend));
}

NAN_SETTER(MediaSource::OnData) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.Holder(), "MediaSource");
  
  if (!value.IsEmpty() && value->IsFunction()) {
    self->_callback = true;
    NanAssignPersistent(self->_ondata, Local<Function>::Cast(value));
  } else {
    self->_callback = false;
    NanDisposePersistent(self->_ondata);
  }
}

NAN_SETTER(MediaSource::OnError) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.Holder(), "MediaSource");
  
  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onerror, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onerror);
  }
}

NAN_SETTER(MediaSource::OnEnd) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(args.Holder(), "MediaSource");
  
  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onend, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onend);
  }
}

void MediaSource::On(Event *event) {
  MediaSourceEvent type = event->Type<MediaSourceEvent>();

  NanScope();

  if (_callback) {
    Local<Function> callback = NanNew(_ondata);
    Local<Object> container;
    
    if (type == kMediaSourceFrame) {
      container = NanNew<Object>();
      webrtc::VideoFrame videoFrame;
      
      rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer = event->Unwrap<rtc::scoped_refptr<webrtc::VideoFrameBuffer> >();
      videoFrame.set_video_frame_buffer(buffer);
      
      node::ArrayBuffer *ybuf = node::ArrayBuffer::New(videoFrame.buffer(webrtc::kYPlane), videoFrame.allocated_size(webrtc::kYPlane));
      node::ArrayBuffer *ubuf = node::ArrayBuffer::New(videoFrame.buffer(webrtc::kUPlane), videoFrame.allocated_size(webrtc::kUPlane));
      node::ArrayBuffer *vbuf = node::ArrayBuffer::New(videoFrame.buffer(webrtc::kVPlane), videoFrame.allocated_size(webrtc::kVPlane));

      container->Set(NanNew("type"), NanNew("frame"));
      container->Set(NanNew("yplane"), ybuf->ToArrayBuffer());
      container->Set(NanNew("uplane"), ubuf->ToArrayBuffer());
      container->Set(NanNew("vplane"), vbuf->ToArrayBuffer());
      container->Set(NanNew("width"), NanNew(buffer->width()));
      container->Set(NanNew("height"), NanNew(buffer->height()));
    } else if (type == kMediaSourceEnd) {
      End();
    }
    
    if (!container.IsEmpty()) {      
      Local<Value> argv[1] = {
        container
      };
      
      callback->Call(RTCWrap::This(), 1, argv);
    }
  }
}