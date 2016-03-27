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

#include "MediaCapturer.h"

using namespace v8;
using namespace WebRTC;

Nan::Persistent<Function> MediaCapturer::constructor;

void MediaCapturer::Init() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(MediaCapturer::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("MediaCapturer").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "stop", MediaCapturer::Stop);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("data").ToLocalChecked(), MediaStreamTrack::GetOnData, MediaStreamTrack::SetOnData);

  constructor.Reset<Function>(tpl->GetFunction());
}

Local<Value> MediaCapturer::New(webrtc::AudioSourceInterface *track) {
LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::EscapableHandleScope scope;

  Local<Value> argv[1];
  Local<Function> instance = Nan::New(MediaCapturer::constructor);
  
  if (instance.IsEmpty() || !track) {
    return scope.Escape(Nan::Null());
  }
  
  Local<Object> ret = instance->NewInstance(0, argv);
  MediaCapturer *self = RTCWrap::Unwrap<MediaCapturer>(ret, "MediaCapturer");
  
  self->_audio = track;
  self->_source = track;
  self->_source->RegisterObserver(self->_observer.get());
  self->_audio->AddSink(self);
  
  if (self->_audio->state() == webrtc::MediaSourceInterface::kLive) {
    self->SetReference(true);
  }
  
  return scope.Escape(ret);
}

Local<Value> MediaCapturer::New(webrtc::VideoSourceInterface *track) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::EscapableHandleScope scope;

  Local<Value> argv[1];
  Local<Function> instance = Nan::New(MediaCapturer::constructor);
  
  if (instance.IsEmpty() || !track) {
    return scope.Escape(Nan::Null());
  }
  
  Local<Object> ret = instance->NewInstance(0, argv);
  MediaCapturer *self = RTCWrap::Unwrap<MediaCapturer>(ret, "MediaCapturer");
  
  self->_video = track;
  self->_source = track;
  self->_source->RegisterObserver(self->_observer.get());
  self->_video->AddOrUpdateSink(self, rtc::VideoSinkWants());
  
  if (self->_video->state() == webrtc::MediaSourceInterface::kLive) {
    self->SetReference(true);
  }
  
  return scope.Escape(ret);
}

MediaCapturer::MediaCapturer() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  _observer = new rtc::RefCountedObject<MediaStreamTrackObserver>(this);
}

MediaCapturer::~MediaCapturer() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_video.get()) {
    _video->RemoveSink(this);
  }
  
  if (_audio.get()) {
    _audio->RemoveSink(this);
  }
  
  if (_source.get()) {
    _source->UnregisterObserver(_observer.get());
  }
  
  _observer->RemoveListener(this);
}

void MediaCapturer::New(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;
  
  if (info.IsConstructCall()) {
    MediaCapturer* MediaCapturer = new MediaCapturer();
    MediaCapturer->Wrap(info.This(), "MediaCapturer");
    return info.GetReturnValue().Set(info.This());
  }

  Nan::ThrowError("Internal Error");
  info.GetReturnValue().SetUndefined();
}

void MediaCapturer::Stop(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaCapturer *self = RTCWrap::Unwrap<MediaCapturer>(info.This(), "MediaCapturer");

  self->SetReference(false);

  return info.GetReturnValue().SetUndefined();
}

void MediaCapturer::GetOnData(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  MediaCapturer *self = RTCWrap::Unwrap<MediaCapturer>(info.Holder(), "MediaCapturer");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_ondata));
}

void MediaCapturer::SetOnData(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaCapturer *self = RTCWrap::Unwrap<MediaCapturer>(info.Holder(), "MediaCapturer");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_ondata.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_ondata.Reset();
  }
}

void MediaCapturer::OnFrame(const cricket::VideoFrame &frame) {
  printf("Got Video Frame!\n");
}

void MediaCapturer::OnData(const void* audio_data, int bits_per_sample, int sample_rate, size_t number_of_channels, size_t number_of_frames) {
  printf("Got Audio Frame!\n");
}

void MediaCapturer::On(Event *event) {
  if (_source.get()) {
    if (_source->state() != webrtc::MediaSourceInterface::kLive) {
      EventEmitter::SetReference(false);
    }
  }
}