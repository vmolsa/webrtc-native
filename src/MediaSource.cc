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

MediaSourceImage::MediaSourceImage() { }
MediaSourceAudio::MediaSourceAudio() { }

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
  
  exports->Set(NanNew("MediaSource"), tpl->GetFunction());
                 
  Local<Object> sources = NanNew<Object>();
  
  WebcamCapturer::Init(sources);
  MediaStreamCapturer::Init(sources);
  WindowRenderer::Init(sources);
  
  NanAssignPersistent(constructor, sources);
}

MediaSource::MediaSource() : _callback(false) {
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

void MediaSource::On(Event *event) {
  MediaSourceEvent type = event->Type<MediaSourceEvent>();

  NanScope();

  if (_callback) {
    Local<Function> callback = NanNew(_ondata);
    Local<Object> container;
    node::ArrayBuffer *arrayBuffer = 0;
    
    if (type == kMediaSourceImage) {
      MediaSourceImage image = event->Unwrap<MediaSourceImage>();
      arrayBuffer = node::ArrayBuffer::New(reinterpret_cast<char *>(image.buffer.data()), image.buffer.size());
      container = NanNew<Object>();
      
      container->Set(NanNew("type"), NanNew(image.mime.c_str()));
      container->Set(NanNew("width"), NanNew(image.width));
      container->Set(NanNew("height"), NanNew(image.height));
      
    } else if (type == kMediaSourceAudio) {
      MediaSourceAudio audio = event->Unwrap<MediaSourceAudio>();
      arrayBuffer = node::ArrayBuffer::New(reinterpret_cast<char *>(audio.buffer.data()), audio.buffer.size());
      container = NanNew<Object>();

      container->Set(NanNew("type"), NanNew(audio.mime.c_str()));
      container->Set(NanNew("bits"), NanNew(audio.bits));
      container->Set(NanNew("rate"), NanNew(audio.rate));
      container->Set(NanNew("channels"), NanNew(audio.channels));
      container->Set(NanNew("frames"), NanNew(audio.frames));
    } else if (type == kMediaSourceData) {
      rtc::Buffer buffer = event->Unwrap<rtc::Buffer>();
      arrayBuffer = node::ArrayBuffer::New(reinterpret_cast<char *>(buffer.data()), buffer.size());
      container = NanNew<Object>();
      
      container->Set(NanNew("type"), NanNew("application/octet-stream"));
    }
    
    if (!container.IsEmpty()) {
      if (arrayBuffer) {
        container->Set(NanNew("data"), arrayBuffer->ToArrayBuffer());
      }
      
      Local<Value> argv[1] = {
        container
      };
      
      callback->Call(RTCWrap::This(), 1, argv);
    }
  }
}