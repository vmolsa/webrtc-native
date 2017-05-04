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

#include "mediastreamtrack.h"

using namespace WebRTC;
using namespace v8;

Nan::Persistent<Function> MediaStreamTrack::constructor;

MediaStreamTrack::MediaStreamTrack() {

}

MediaStreamTrack::~MediaStreamTrack() {

}

void MediaStreamTrack::Init(Local<Object> exports) {
  Nan::HandleScope scope;
  
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(MediaStreamTrack::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New<String>("MediaStreamTrack").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "clone", MediaStreamTrack::Clone);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                  Nan::New<String>("enabled").ToLocalChecked(), 
                  MediaStreamTrack::Enabled);
  
  Nan::SetAccessor(tpl->InstanceTemplate(), 
                  Nan::New<String>("muted").ToLocalChecked(), 
                  MediaStreamTrack::Muted);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                  Nan::New<String>("remote").ToLocalChecked(), 
                  MediaStreamTrack::Remote);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                  Nan::New<String>("id").ToLocalChecked(), 
                  MediaStreamTrack::Id);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                  Nan::New<String>("kind").ToLocalChecked(), 
                  MediaStreamTrack::Kind);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                  Nan::New<String>("readyState").ToLocalChecked(), 
                  MediaStreamTrack::ReadyState);
  
  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New<String>("onstarted").ToLocalChecked(),
                   MediaStreamTrack::onstarted,
                   MediaStreamTrack::onstarted);
  
  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New<String>("onended").ToLocalChecked(),
                   MediaStreamTrack::onended,
                   MediaStreamTrack::onended);
  
  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New<String>("onmute").ToLocalChecked(),
                   MediaStreamTrack::onmute,
                   MediaStreamTrack::onmute);
  
  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New<String>("onunmute").ToLocalChecked(),
                   MediaStreamTrack::onunmute,
                   MediaStreamTrack::onunmute);

  constructor.Reset<Function>(tpl->GetFunction());
}

void MediaStreamTrack::New(const Nan::FunctionCallbackInfo<Value> &info) {
  if (info.IsConstructCall()) {
    MediaStreamTrack* mediaStreamTrack = new MediaStreamTrack();
    mediaStreamTrack->Wrap(info.This());
    return info.GetReturnValue().Set(info.This());
  } 
}

v8::Local<v8::Value> MediaStreamTrack::New(const crtc::Let<crtc::MediaStreamTrack> &mediaStreamTrack) {
  Nan::EscapableHandleScope scope;
  Local<Function> cons = Nan::New(MediaStreamTrack::constructor);
  
  if (cons.IsEmpty() || mediaStreamTrack.IsEmpty()) {
    return scope.Escape(Nan::Undefined());
  }
  
  Local<Value> argv[0];
  int argc = 0;
    
  Local<Object> obj = Nan::NewInstance(cons, argc, argv).ToLocalChecked();
  MediaStreamTrack *self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(obj);
  self->Init(mediaStreamTrack);

  return scope.Escape(obj);
}

crtc::Let<crtc::MediaStreamTrack> MediaStreamTrack::New(const v8::Local<v8::Object> &mediaStreamTrack) {
  MediaStreamTrack *self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(mediaStreamTrack);
  return self->_track;
}

void MediaStreamTrack::Init(const crtc::Let<crtc::MediaStreamTrack> &mediaStreamTrack) {

  _track = mediaStreamTrack;
}

void MediaStreamTrack::Clone(const Nan::FunctionCallbackInfo<Value> &info)  {

}

void MediaStreamTrack::Enabled(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {

}

void MediaStreamTrack::Muted(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  
}

void MediaStreamTrack::Remote(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  
}

void MediaStreamTrack::Id(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  
}

void MediaStreamTrack::Kind(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  
}

void MediaStreamTrack::ReadyState(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  
}

void MediaStreamTrack::onstarted(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  MediaStreamTrack *self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onstarted));
}

void MediaStreamTrack::onstarted(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  MediaStreamTrack *self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onstarted.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onstarted.Reset();
  }
}

void MediaStreamTrack::onended(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  MediaStreamTrack *self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onended));
}

void MediaStreamTrack::onended(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  MediaStreamTrack *self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onended.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onended.Reset();
  }
}

void MediaStreamTrack::onmute(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  MediaStreamTrack *self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onmute));
}

void MediaStreamTrack::onmute(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  MediaStreamTrack *self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onmute.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onmute.Reset();
  }
}

void MediaStreamTrack::onunmute(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  MediaStreamTrack *self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onunmute));
}

void MediaStreamTrack::onunmute(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  MediaStreamTrack *self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onunmute.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onunmute.Reset();
  }
}
