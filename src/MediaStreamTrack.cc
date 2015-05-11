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

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "getConstraints"),
                                FunctionTemplate::New(isolate, MediaStreamTrack::GetConstraints));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "applyConstraints"),
                                FunctionTemplate::New(isolate, MediaStreamTrack::ApplyConstraints));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "setSettings"),
                                FunctionTemplate::New(isolate, MediaStreamTrack::GetSettings));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "getCapabilities"),
                                FunctionTemplate::New(isolate, MediaStreamTrack::GetCapabilities));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "clone"),
                                FunctionTemplate::New(isolate, MediaStreamTrack::Clone));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "stop"),
                                FunctionTemplate::New(isolate, MediaStreamTrack::Stop));

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "enabled"),
                                       MediaStreamTrack::GetEnabled,
                                       MediaStreamTrack::SetEnabled);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "id"),
                                       MediaStreamTrack::GetId,
                                       MediaStreamTrack::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "kind"),
                                       MediaStreamTrack::GetKind,
                                       MediaStreamTrack::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "label"),
                                       MediaStreamTrack::GetLabel,
                                       MediaStreamTrack::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "muted"),
                                       MediaStreamTrack::GetMuted,
                                       MediaStreamTrack::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "readonly"),
                                       MediaStreamTrack::GetReadOnly,
                                       MediaStreamTrack::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "readyState"),
                                       MediaStreamTrack::GetReadyState,
                                       MediaStreamTrack::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "remote"),
                                       MediaStreamTrack::GetRemote,
                                       MediaStreamTrack::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onstarted"),
                                       MediaStreamTrack::GetOnStarted,
                                       MediaStreamTrack::SetOnStarted);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onmute"),
                                       MediaStreamTrack::GetOnMute,
                                       MediaStreamTrack::SetOnMute);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onunmute"),
                                       MediaStreamTrack::GetOnUnMute,
                                       MediaStreamTrack::SetOnUnMute);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onoverconstrained"),
                                       MediaStreamTrack::GetOnOverConstrained,
                                       MediaStreamTrack::SetOnOverConstrained);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onended"),
                                       MediaStreamTrack::GetOnEnded,
                                       MediaStreamTrack::SetOnEnded);

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

  self->_track = mediaStreamTrack;
  self->_track->RegisterObserver(self->_observer.get());
  self->Emit(kMediaStreamTrackChanged);

  return scope.Escape(ret);
}

MediaStreamTrack::MediaStreamTrack() {
  _observer = new rtc::RefCountedObject<MediaStreamTrackObserver>(this);
  EventEmitter::Start(true);
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

void MediaStreamTrack::GetConstraints(const v8::FunctionCallbackInfo<v8::Value>& args) {
  //Isolate *isolate = args.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, args.This(), "MediaStreamTrack");

}

void MediaStreamTrack::ApplyConstraints(const v8::FunctionCallbackInfo<v8::Value>& args) {
  //Isolate *isolate = args.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, args.This(), "MediaStreamTrack");

}

void MediaStreamTrack::GetSettings(const v8::FunctionCallbackInfo<v8::Value>& args) {
  //Isolate *isolate = args.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, args.This(), "MediaStreamTrack");

}

void MediaStreamTrack::GetCapabilities(const v8::FunctionCallbackInfo<v8::Value>& args) {
  //Isolate *isolate = args.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, args.This(), "MediaStreamTrack");

}

void MediaStreamTrack::Clone(const v8::FunctionCallbackInfo<v8::Value>& args) {
  //Isolate *isolate = args.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, args.This(), "MediaStreamTrack");

}

void MediaStreamTrack::Stop(const v8::FunctionCallbackInfo<v8::Value>& args) {
  //Isolate *isolate = args.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, args.This(), "MediaStreamTrack");

}

void MediaStreamTrack::GetEnabled(Local<String> property,
                                  const PropertyCallbackInfo<Value> &info)
{
  //Isolate *isolate = info.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
}

void MediaStreamTrack::GetId(Local<String> property,
                             const PropertyCallbackInfo<Value> &info)
{
  //Isolate *isolate = info.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
}

void MediaStreamTrack::GetKind(Local<String> property,
                               const PropertyCallbackInfo<Value> &info)
{
  //Isolate *isolate = info.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
}

void MediaStreamTrack::GetLabel(Local<String> property,
                                const PropertyCallbackInfo<Value> &info)
{
  //Isolate *isolate = info.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
}

void MediaStreamTrack::GetMuted(Local<String> property,
                                const PropertyCallbackInfo<Value> &info)
{
  //Isolate *isolate = info.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
}

void MediaStreamTrack::GetReadOnly(Local<String> property,
                                   const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  info.GetReturnValue().Set(v8::Boolean::New(isolate, true));
}

void MediaStreamTrack::GetReadyState(Local<String> property,
                                     const PropertyCallbackInfo<Value> &info)
{
  //Isolate *isolate = info.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
}

void MediaStreamTrack::GetRemote(Local<String> property,
                                 const PropertyCallbackInfo<Value> &info)
{
  //Isolate *isolate = info.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
}

void MediaStreamTrack::GetOnStarted(Local<String> property,
                                    const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onstarted));
}

void MediaStreamTrack::GetOnMute(Local<String> property,
                                 const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onmute));
}

void MediaStreamTrack::GetOnUnMute(Local<String> property,
                                   const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onunmute));
}

void MediaStreamTrack::GetOnOverConstrained(Local<String> property,
                                            const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onoverconstrained));
}

void MediaStreamTrack::GetOnEnded(Local<String> property,
                                  const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onended));
}

void MediaStreamTrack::ReadOnly(Local<String> property,
                                Local<Value> value,
                                const PropertyCallbackInfo<void> &info)
{
  //Isolate *isolate = info.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");
}

void MediaStreamTrack::SetEnabled(Local<String> property,
                                  Local<Value> value,
                                  const PropertyCallbackInfo<void> &info)
{
  //Isolate *isolate = info.GetIsolate();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
}

void MediaStreamTrack::SetOnStarted(Local<String> property,
                                    Local<Value> value,
                                    const PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onstarted.Reset(isolate, Local<Function>::Cast(value));
  }
  else {
    self->_onstarted.Reset();
  }
}

void MediaStreamTrack::SetOnMute(Local<String> property,
                                 Local<Value> value,
                                 const PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onmute.Reset(isolate, Local<Function>::Cast(value));
  }
  else {
    self->_onmute.Reset();
  }
}

void MediaStreamTrack::SetOnUnMute(Local<String> property,
                                   Local<Value> value,
                                   const PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onunmute.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_onunmute.Reset();
  }
}

void MediaStreamTrack::SetOnOverConstrained(Local<String> property,
                                            Local<Value> value,
                                            const PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onoverconstrained.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_onoverconstrained.Reset();
  }
}

void MediaStreamTrack::SetOnEnded(Local<String> property,
                                  Local<Value> value,
                                  const PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(isolate, info.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onended.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_onended.Reset();
  }
}

void MediaStreamTrack::On(Event *event) {
  MediaStreamTrackEvent type = event->Type<MediaStreamTrackEvent>();

  if (type != kMediaStreamTrackChanged) {
    return;
  }
}
