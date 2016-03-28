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

Nan::Persistent<Function> MediaStreamTrack::constructor;

void MediaStreamTrack::Init() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(MediaStreamTrack::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("MediaStreamTrack").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "getConstraints", MediaStreamTrack::GetConstraints);
  Nan::SetPrototypeMethod(tpl, "applyConstraints", MediaStreamTrack::ApplyConstraints);
  Nan::SetPrototypeMethod(tpl, "setSettings", MediaStreamTrack::GetSettings);
  Nan::SetPrototypeMethod(tpl, "getCapabilities", MediaStreamTrack::GetCapabilities);
  Nan::SetPrototypeMethod(tpl, "clone", MediaStreamTrack::Clone);
  Nan::SetPrototypeMethod(tpl, "stop", MediaStreamTrack::Stop);

  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("enabled").ToLocalChecked(), MediaStreamTrack::GetEnabled, MediaStreamTrack::SetEnabled);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("id").ToLocalChecked(), MediaStreamTrack::GetId);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("kind").ToLocalChecked(), MediaStreamTrack::GetKind);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("label").ToLocalChecked(), MediaStreamTrack::GetLabel);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("muted").ToLocalChecked(), MediaStreamTrack::GetMuted);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("readonly").ToLocalChecked(), MediaStreamTrack::GetReadOnly);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("readyState").ToLocalChecked(), MediaStreamTrack::GetReadyState);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("remote").ToLocalChecked(), MediaStreamTrack::GetRemote);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onstarted").ToLocalChecked(), MediaStreamTrack::GetOnStarted, MediaStreamTrack::SetOnStarted);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onmute").ToLocalChecked(), MediaStreamTrack::GetOnMute, MediaStreamTrack::SetOnMute);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onunmute").ToLocalChecked(), MediaStreamTrack::GetOnUnMute, MediaStreamTrack::SetOnUnMute);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onoverconstrained").ToLocalChecked(), MediaStreamTrack::GetOnOverConstrained, MediaStreamTrack::SetOnOverConstrained);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onended").ToLocalChecked(), MediaStreamTrack::GetOnEnded, MediaStreamTrack::SetOnEnded);

  constructor.Reset<Function>(tpl->GetFunction());
}

Local<Value> MediaStreamTrack::New(rtc::scoped_refptr<webrtc::AudioTrackInterface> audioTrack) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::EscapableHandleScope scope;

  Local<Value> argv[1];
  Local<Function> instance = Nan::New(MediaStreamTrack::constructor);

  if (instance.IsEmpty() || !audioTrack.get()) {
    return scope.Escape(Nan::Null());
  }

  Local<Object> ret = instance->NewInstance(0, argv);
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(ret, "MediaStreamTrack");

  self->isAudioTrack = true;
  self->_track = audioTrack;
  self->_source = audioTrack->GetSource();
  self->_track_state = self->_track->state();
  self->_source_state = self->_source->state();
  self->_track->RegisterObserver(self->_observer.get());
  self->_source->RegisterObserver(self->_observer.get());
  self->CheckState();

  return scope.Escape(ret);
}

Local<Value> MediaStreamTrack::New(rtc::scoped_refptr<webrtc::VideoTrackInterface> videoTrack) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::EscapableHandleScope scope;

  Local<Value> argv[1];
  Local<Function> instance = Nan::New(MediaStreamTrack::constructor);

  if (instance.IsEmpty() || !videoTrack.get()) {
    return scope.Escape(Nan::Null());
  }

  Local<Object> ret = instance->NewInstance(0, argv);
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(ret, "MediaStreamTrack");

  self->isVideoTrack = true;
  self->_track = videoTrack;
  self->_source = videoTrack->GetSource();
  self->_track_state = self->_track->state();
  self->_source_state = self->_source->state();
  self->_track->RegisterObserver(self->_observer.get());
  self->_source->RegisterObserver(self->_observer.get());
  self->CheckState();

  return scope.Escape(ret);
}

MediaStreamTrack::MediaStreamTrack() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  _observer = new rtc::RefCountedObject<MediaStreamTrackObserver>(this);
}

MediaStreamTrack::~MediaStreamTrack() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_track.get()) {
    _track->UnregisterObserver(_observer.get());
  }
  
  if (_source.get()) {
    _source->UnregisterObserver(_observer.get());
  }
  
  _observer->RemoveListener(this);
}

void MediaStreamTrack::New(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;
  
  if (info.IsConstructCall()) {
    MediaStreamTrack* mediaStreamTrack = new MediaStreamTrack();
    mediaStreamTrack->Wrap(info.This(), "MediaStreamTrack");
    return info.GetReturnValue().Set(info.This());
  }

  Nan::ThrowError("Internal Error");
  info.GetReturnValue().SetUndefined();
}

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> MediaStreamTrack::Unwrap(Local<Object> value) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (!value.IsEmpty()) {
    MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(value, "MediaStreamTrack");

    if (self) {
      return self->_track;
    }
  }

  return 0;
}

rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> MediaStreamTrack::Unwrap(Local<Value> value) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (!value.IsEmpty() && value->IsObject()) {
    Local<Object> track = Local<Object>::Cast(value);
    return MediaStreamTrack::Unwrap(track);
  }

  return 0;
}

void MediaStreamTrack::GetConstraints(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.This(), "MediaStreamTrack");

  info.GetReturnValue().SetUndefined();
}

void MediaStreamTrack::ApplyConstraints(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.This(), "MediaStreamTrack");
  
  info.GetReturnValue().SetUndefined();
}

void MediaStreamTrack::GetSettings(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.This(), "MediaStreamTrack");

  info.GetReturnValue().SetUndefined();
}

void MediaStreamTrack::GetCapabilities(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.This(), "MediaStreamTrack");

  info.GetReturnValue().SetUndefined();
}

void MediaStreamTrack::Clone(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
 
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.This(), "MediaStreamTrack");

  info.GetReturnValue().SetUndefined();
}

void MediaStreamTrack::Stop(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.This(), "MediaStreamTrack");
  info.GetReturnValue().Set(Nan::New(self->_track->set_state(webrtc::MediaStreamTrackInterface::kEnded)));
}

void MediaStreamTrack::GetEnabled(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.This(), "MediaStreamTrack");
  info.GetReturnValue().Set(Nan::New(self->_track->enabled()));
}

void MediaStreamTrack::GetId(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");
  info.GetReturnValue().Set(Nan::New(self->_track->id().c_str()).ToLocalChecked());
}

void MediaStreamTrack::GetKind(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");
  info.GetReturnValue().Set(Nan::New(self->_track->kind().c_str()).ToLocalChecked());
}

void MediaStreamTrack::GetLabel(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  info.GetReturnValue().Set(Nan::New("").ToLocalChecked());
}

void MediaStreamTrack::GetMuted(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");
  info.GetReturnValue().Set(Nan::New((self->_source->state() == webrtc::MediaSourceInterface::kMuted) ? true : false));
}

void MediaStreamTrack::GetReadOnly(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  return info.GetReturnValue().Set(Nan::New(true));
}

void MediaStreamTrack::GetReadyState(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");

  if (self->_track->state() == webrtc::MediaStreamTrackInterface::kLive) {
    return info.GetReturnValue().Set(Nan::New("live").ToLocalChecked());
  }
  
  info.GetReturnValue().Set(Nan::New("ended").ToLocalChecked());
}

void MediaStreamTrack::GetRemote(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");
  info.GetReturnValue().Set(Nan::New(self->_source->remote()));
}

void MediaStreamTrack::GetOnStarted(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onstarted));
}

void MediaStreamTrack::GetOnMute(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onmute));
}

void MediaStreamTrack::GetOnUnMute(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onunmute));
}

void MediaStreamTrack::GetOnOverConstrained(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onoverconstrained));
}

void MediaStreamTrack::GetOnEnded(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onended));
}


void MediaStreamTrack::ReadOnly(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
}

void MediaStreamTrack::SetEnabled(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");
  
  if (!value.IsEmpty() && value->IsBoolean()) {
    self->_track->set_enabled(value->IsTrue() ? true : false);
  }
}

void MediaStreamTrack::SetOnStarted(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onstarted.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onstarted.Reset();
  }
}

void MediaStreamTrack::SetOnMute(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onmute.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onmute.Reset();
  }
}

void MediaStreamTrack::SetOnUnMute(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onunmute.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onunmute.Reset();
  }
}

void MediaStreamTrack::SetOnOverConstrained(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onoverconstrained.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onoverconstrained.Reset();
  }
}

void MediaStreamTrack::SetOnEnded(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(info.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onended.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onended.Reset();
  }
}

void MediaStreamTrack::CheckState() {
  webrtc::MediaStreamTrackInterface::TrackState new_state = _track->state();
  webrtc::MediaSourceInterface::SourceState new_source = _source->state();
  
  if (_track_state != new_state) {
    if (new_state == webrtc::MediaStreamTrackInterface::kEnded || new_state == webrtc::MediaStreamTrackInterface::kFailed) {
      Local<Function> callback = Nan::New<Function>(_onended);
      Local<Value> argv[1];

      if (!callback.IsEmpty() && callback->IsFunction()) {
        callback->Call(RTCWrap::This(), 0, argv);
      }
    } else if (_track_state == webrtc::MediaStreamTrackInterface::kInitializing && new_state == webrtc::MediaStreamTrackInterface::kLive) {
      Local<Function> callback = Nan::New<Function>(_onstarted);
      Local<Value> argv[1];

      if (!callback.IsEmpty() && callback->IsFunction()) {
        callback->Call(RTCWrap::This(), 0, argv);
      }
    }
  }

  if (_source_state != new_source) {
    if (new_source == webrtc::MediaSourceInterface::kMuted) {
      Local<Function> callback = Nan::New<Function>(_onmute);
      Local<Value> argv[1];

      if (!callback.IsEmpty() && callback->IsFunction()) {
        callback->Call(RTCWrap::This(), 0, argv);
      }
    } else {
      Local<Function> callback = Nan::New<Function>(_onunmute);
      Local<Value> argv[1];

      if (!callback.IsEmpty() && callback->IsFunction()) {
        callback->Call(RTCWrap::This(), 0, argv);
      }
    }
  }
 
  _source_state = new_source;  
  _track_state = new_state;
}

void MediaStreamTrack::On(Event *event) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;
  MediaStreamTrackEvent type = event->Type<MediaStreamTrackEvent>();

  if (type != kMediaStreamTrackChanged) {
    return;
  }
  
  MediaStreamTrack::CheckState();
}
