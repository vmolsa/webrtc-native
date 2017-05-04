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

#include "mediastream.h"

using namespace WebRTC;
using namespace v8;

Nan::Persistent<Function> MediaStream::constructor;

MediaStream::MediaStream() {

}

MediaStream::~MediaStream() {

}

void MediaStream::Init(Local<Object> exports) {
  Nan::HandleScope scope;
  
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(MediaStream::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New<String>("MediaStream").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "clone", MediaStream::Clone);
  Nan::SetPrototypeMethod(tpl, "addTrack", MediaStream::AddTrack);
  Nan::SetPrototypeMethod(tpl, "removeTrack", MediaStream::RemoveTrack);
  Nan::SetPrototypeMethod(tpl, "getTrackById", MediaStream::GetTrackById);
  Nan::SetPrototypeMethod(tpl, "getAudioTracks", MediaStream::GetAudioTracks);
  Nan::SetPrototypeMethod(tpl, "getVideoTracks", MediaStream::GetVideoTracks);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                  Nan::New<String>("id").ToLocalChecked(), 
                  MediaStream::Id);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New<String>("onaddtrack").ToLocalChecked(),
                   MediaStream::onaddtrack,
                   MediaStream::onaddtrack);
  
  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New<String>("onremovetrack").ToLocalChecked(),
                   MediaStream::onremovetrack,
                   MediaStream::onremovetrack);

  constructor.Reset<Function>(tpl->GetFunction());
}

void MediaStream::New(const Nan::FunctionCallbackInfo<Value> &info) {
  if (info.IsConstructCall()) {
    MediaStream* mediaStream = new MediaStream();
    mediaStream->Wrap(info.This());
    return info.GetReturnValue().Set(info.This());
  } 
}

v8::Local<v8::Value> MediaStream::New(const crtc::Let<crtc::MediaStream> &mediaStream) {
  Nan::EscapableHandleScope scope;
  Local<Function> cons = Nan::New(MediaStream::constructor);
  
  if (cons.IsEmpty() || mediaStream.IsEmpty()) {
    return scope.Escape(Nan::Undefined());
  }
  
  Local<Value> argv[0];
  int argc = 0;
    
  Local<Object> obj = Nan::NewInstance(cons, argc, argv).ToLocalChecked();
  MediaStream *self = Nan::ObjectWrap::Unwrap<MediaStream>(obj);
  self->Init(mediaStream);

  return scope.Escape(obj);
}

crtc::Let<crtc::MediaStream> MediaStream::New(const v8::Local<v8::Object> &mediaStream) {
  MediaStream *self = Nan::ObjectWrap::Unwrap<MediaStream>(mediaStream);
  return self->_stream;
}

void MediaStream::Init(const crtc::Let<crtc::MediaStream> &mediaStream) {

  _stream = mediaStream;
}

void MediaStream::Clone(const Nan::FunctionCallbackInfo<Value> &info)  {

}

void MediaStream::AddTrack(const Nan::FunctionCallbackInfo<Value> &info)  {

}

void MediaStream::RemoveTrack(const Nan::FunctionCallbackInfo<Value> &info)  {

}

void MediaStream::GetTrackById(const Nan::FunctionCallbackInfo<Value> &info)  {

}

void MediaStream::GetAudioTracks(const Nan::FunctionCallbackInfo<Value> &info)  {

}

void MediaStream::GetVideoTracks(const Nan::FunctionCallbackInfo<Value> &info)  {

}

void MediaStream::Id(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  
}

void MediaStream::onaddtrack(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  MediaStream *self = Nan::ObjectWrap::Unwrap<MediaStream>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onaddtrack));
}

void MediaStream::onaddtrack(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  MediaStream *self = Nan::ObjectWrap::Unwrap<MediaStream>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onaddtrack.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onaddtrack.Reset();
  }
}

void MediaStream::onremovetrack(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  MediaStream *self = Nan::ObjectWrap::Unwrap<MediaStream>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onremovetrack));
}

void MediaStream::onremovetrack(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  MediaStream *self = Nan::ObjectWrap::Unwrap<MediaStream>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onremovetrack.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onremovetrack.Reset();
  }
}
