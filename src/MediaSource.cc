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

Nan::PersistentBase<Object> MediaSource::constructor;

void MediaSource::Init(Handle<Object> exports) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope();
  
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(MediaSource::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("MediaSource").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "connect", MediaSource::Connect);
  Nan::SetPrototypeMethod(tpl, "disconnect", MediaSource::Disconnect);
  Nan::SetPrototypeMethod(tpl, "write", MediaSource::Write);
  Nan::SetPrototypeMethod(tpl, "end", MediaSource::End);
  
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("ondata").ToLocalChecked(), MediaSource::OnData, MediaSource::OnData);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onerror").ToLocalChecked(), MediaSource::OnError, MediaSource::OnError);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onend").ToLocalChecked(), MediaSource::OnEnd, MediaSource::OnEnd);
  
  exports->Set(Nan::New("MediaSource").ToLocalChecked(), tpl->GetFunction());
                 
  Local<Object> sources = Nan::New<Object>();
  
  WebcamCapturer::Init(sources);
  MediaStreamCapturer::Init(sources);
  WindowRenderer::Init(sources);
  
  constructor.Reset(sources);
}

MediaSource::MediaSource() : _closing(false), _callback(false) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

MediaSource::~MediaSource() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  EventEmitter::RemoveAllListeners();
}

void MediaSource::New(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  if (info.IsConstructCall() && info.Length() >= 1 && info[0]->IsString()) {
    Local<String> type = info[0]->ToString();
    Local<Object> instances = Nan::New(MediaSource::constructor);
    Local<Function> instance = Local<Function>::Cast(instances->Get(type));
    
    if (!instance.IsEmpty()) {
      Local<Object> properties = Local<Object>::Cast(info[1]);
      Local<Value> argv[1] = {
        properties
      };
      
      return info.GetReturnValue().Set(instance->Call(info.This(), 1, argv));
    }
  }
  
  info.GetReturnValue().SetUndefined();
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
  
  Nan::HandleScope();
  
  if (!_closing) {
    _closing = true;
    
    EventEmitter::Emit(kMediaSourceEnd);
    EventEmitter::SetReference(false);
    
    Local<Function> callback = Nan::New<Function>(_onend);
      
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
  
  Nan::HandleScope();
  
  if (!data.IsEmpty() && data->IsObject()) {
    Local<Object> frame = Local<Object>::Cast(data);
    Local<Value> type_value = frame->Get(Nan::New("type").ToLocalChecked());
      
    if (!type_value.IsEmpty() && type_value->IsString()) {
      String::Utf8Value type_utf8(type_value->ToString());
      std::string type(*type_utf8);
      
      if (!type.compare("frame")) {
        Local<Int32> width(frame->Get(Nan::New("width").ToLocalChecked())->ToInt32());
        Local<Int32> height(frame->Get(Nan::New("height").ToLocalChecked())->ToInt32());
        
        node::ArrayBuffer *ybuf = node::ArrayBuffer::New(frame->Get(Nan::New("yplane").ToLocalChecked()));
        node::ArrayBuffer *ubuf = node::ArrayBuffer::New(frame->Get(Nan::New("uplane").ToLocalChecked()));
        node::ArrayBuffer *vbuf = node::ArrayBuffer::New(frame->Get(Nan::New("vplane").ToLocalChecked()));

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

void MediaSource::Connect(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(info.This(), "MediaSource");
  
  if (info.Length() >= 1 && info[0]->IsObject()) {
    return info.GetReturnValue().Set(Nan::New(self->Connect(RTCWrap::Unwrap<MediaSource>(Local<Object>::Cast(info[0]), "MediaSource"))));
  }
  
  return info.GetReturnValue().Set(Nan::False());
}

void MediaSource::Disconnect(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  MediaSource *self = RTCWrap::Unwrap<MediaSource>(info.This(), "MediaSource");
  
  if (info.Length() >= 1 && info[0]->IsObject()) {
    return info.GetReturnValue().Set(Nan::New(self->Disconnect(RTCWrap::Unwrap<MediaSource>(Local<Object>::Cast(info[0]), "MediaSource"))));
  }
  
  return info.GetReturnValue().Set(Nan::False());
}

void MediaSource::Write(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(info.This(), "MediaSource");
  return info.GetReturnValue().Set(Nan::New(self->Write(info[0])));
}

void MediaSource::End(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(info.This(), "MediaSource");
  return info.GetReturnValue().Set(Nan::New(self->End(info[0])));
}

void MediaSource::OnData(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(info.Holder(), "MediaSource");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_ondata));
}

void MediaSource::OnError(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(info.Holder(), "MediaSource");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onerror));
}

void MediaSource::OnEnd(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  MediaSource *self = RTCWrap::Unwrap<MediaSource>(info.Holder(), "MediaSource");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onend));
}

void MediaSource::OnData(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(info.Holder(), "MediaSource");
  
  if (!value.IsEmpty() && value->IsFunction()) {
    self->_callback = true;
    self->_ondata.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_callback = false;
    self->_ondata.Reset();
  }
}

void MediaSource::OnError(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(info.Holder(), "MediaSource");
  
  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onerror.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onerror.Reset();
  }
}

void MediaSource::OnEnd(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaSource *self = RTCWrap::Unwrap<MediaSource>(info.Holder(), "MediaSource");
  
  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onend.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onend.Reset();
  }
}

void MediaSource::On(Event *event) {
  MediaSourceEvent type = event->Type<MediaSourceEvent>();

  if (_callback) {
    Local<Function> callback = Nan::New<Function>(_ondata);
    Local<Object> container;
    
    if (type == kMediaSourceFrame) {
      container = Nan::New<Object>();
      webrtc::VideoFrame videoFrame;
      
      rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer = event->Unwrap<rtc::scoped_refptr<webrtc::VideoFrameBuffer> >();
      videoFrame.set_video_frame_buffer(buffer);
      
      node::ArrayBuffer *ybuf = node::ArrayBuffer::New(videoFrame.buffer(webrtc::kYPlane), videoFrame.allocated_size(webrtc::kYPlane));
      node::ArrayBuffer *ubuf = node::ArrayBuffer::New(videoFrame.buffer(webrtc::kUPlane), videoFrame.allocated_size(webrtc::kUPlane));
      node::ArrayBuffer *vbuf = node::ArrayBuffer::New(videoFrame.buffer(webrtc::kVPlane), videoFrame.allocated_size(webrtc::kVPlane));

      container->Set(Nan::New("type").ToLocalChecked(), Nan::New("frame").ToLocalChecked());
      container->Set(Nan::New("yplane").ToLocalChecked(), ybuf->ToArrayBuffer());
      container->Set(Nan::New("uplane").ToLocalChecked(), ubuf->ToArrayBuffer());
      container->Set(Nan::New("vplane").ToLocalChecked(), vbuf->ToArrayBuffer());
      container->Set(Nan::New("width").ToLocalChecked(), Nan::New(buffer->width()));
      container->Set(Nan::New("height").ToLocalChecked(), Nan::New(buffer->height()));
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