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

#include "MediaStreamTrack.h"

using namespace v8;
using namespace WebRTC;

Persistent<Function> MediaStreamTrack::constructor;

void MediaStreamTrack::Init() {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, MediaStreamTrack::New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "MediaStreamTrack"));

  constructor.Reset(isolate, tpl->GetFunction());
}

Local<Value> MediaStreamTrack::New(Isolate *isolate, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> mediaStreamTrack) {
  EscapableHandleScope scope(isolate);

  Local<Value> argv[1];
  Local<Function> instance = Local<Function>::New(isolate, MediaStreamTrack::constructor);

  if (instance.IsEmpty() || !mediaStreamTrack.get()) {
    Local<Value> empty = Null(isolate);
    return scope.Escape(empty);
  }

  Local<Object> ret = instance->NewInstance(0, argv);
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, ret, "MediaStreamTrack");

  self->Start();
  self->_track = mediaStreamTrack;
  self->_track->RegisterObserver(self->_observer.get());
  self->Emit(kMediaStreamTrackChanged);

  return scope.Escape(ret);
}

MediaStreamTrack::MediaStreamTrack() {
  _observer = new rtc::RefCountedObject<MediaStreamTrackObserver>(this);
}

MediaStreamTrack::~MediaStreamTrack() {
  if (_track.get()) {
    _track->UnregisterObserver(_observer.get());
  }

  EventEmitter::End();
}

void MediaStreamTrack::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    MediaStreamTrack* mediaStreamTrack = new MediaStreamTrack();
    mediaStreamTrack->Wrap(isolate, args.This(), "MediaStreamTrack");
    return args.GetReturnValue().Set(args.This());
  }

  isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
}

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> MediaStreamTrack::Unwrap(Isolate *isolate, Local<Object> value) {
  if (!value.IsEmpty()) {
    MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, value, "MediaStreamTrack");

    if (self) {
      return self->_track;
    }
  }

  return 0;
}

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> MediaStreamTrack::Unwrap(Isolate *isolate, Local<Value> value) {
  if (!value.IsEmpty() && value->IsObject()) {
    Local<Object> track = Local<Object>::Cast(value);
    return MediaStreamTrack::Unwrap(isolate, track);
  }

  return 0;
}

void MediaStreamTrack::On(Event *event) {
  MediaStreamTrackEvent type = event->Type<MediaStreamTrackEvent>();

  if (type != kMediaStreamTrackChanged) {
    return;
  }

  EventEmitter::Stop();
}
