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
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(MediaStreamTrack::New);
  tpl->SetClassName(NanNew("MediaStreamTrack"));

  tpl->PrototypeTemplate()->Set(NanNew("getConstraints"), NanNew<FunctionTemplate>(MediaStreamTrack::GetConstraints)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("applyConstraints"), NanNew<FunctionTemplate>(MediaStreamTrack::ApplyConstraints)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("setSettings"), NanNew<FunctionTemplate>(MediaStreamTrack::GetSettings)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("getCapabilities"), NanNew<FunctionTemplate>(MediaStreamTrack::GetCapabilities)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("clone"), NanNew<FunctionTemplate>(MediaStreamTrack::Clone)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("stop"), NanNew<FunctionTemplate>(MediaStreamTrack::Stop)->GetFunction());

  tpl->InstanceTemplate()->SetAccessor(NanNew("enabled"),
                                       MediaStreamTrack::GetEnabled,
                                       MediaStreamTrack::SetEnabled);

  tpl->InstanceTemplate()->SetAccessor(NanNew("id"),
                                       MediaStreamTrack::GetId,
                                       MediaStreamTrack::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(NanNew("kind"),
                                       MediaStreamTrack::GetKind,
                                       MediaStreamTrack::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(NanNew("label"),
                                       MediaStreamTrack::GetLabel,
                                       MediaStreamTrack::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(NanNew("muted"),
                                       MediaStreamTrack::GetMuted,
                                       MediaStreamTrack::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(NanNew("readonly"),
                                       MediaStreamTrack::GetReadOnly,
                                       MediaStreamTrack::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(NanNew("readyState"),
                                       MediaStreamTrack::GetReadyState,
                                       MediaStreamTrack::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(NanNew("remote"),
                                       MediaStreamTrack::GetRemote,
                                       MediaStreamTrack::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(NanNew("onstarted"),
                                       MediaStreamTrack::GetOnStarted,
                                       MediaStreamTrack::SetOnStarted);

  tpl->InstanceTemplate()->SetAccessor(NanNew("onmute"),
                                       MediaStreamTrack::GetOnMute,
                                       MediaStreamTrack::SetOnMute);

  tpl->InstanceTemplate()->SetAccessor(NanNew("onunmute"),
                                       MediaStreamTrack::GetOnUnMute,
                                       MediaStreamTrack::SetOnUnMute);

  tpl->InstanceTemplate()->SetAccessor(NanNew("onoverconstrained"),
                                       MediaStreamTrack::GetOnOverConstrained,
                                       MediaStreamTrack::SetOnOverConstrained);

  tpl->InstanceTemplate()->SetAccessor(NanNew("onended"),
                                       MediaStreamTrack::GetOnEnded,
                                       MediaStreamTrack::SetOnEnded);

  NanAssignPersistent(constructor, tpl->GetFunction());
}

Local<Value> MediaStreamTrack::New(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> mediaStreamTrack) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanEscapableScope();

  Local<Value> argv[1];
  Local<Function> instance = NanNew(MediaStreamTrack::constructor);

  if (instance.IsEmpty() || !mediaStreamTrack.get()) {
    return NanEscapeScope(NanNull());
  }

  Local<Object> ret = instance->NewInstance(0, argv);
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(ret, "MediaStreamTrack");

  self->_track = mediaStreamTrack;
  self->_track->RegisterObserver(self->_observer.get());
  self->Emit(kMediaStreamTrackChanged);

  return NanEscapeScope(ret);
}

MediaStreamTrack::MediaStreamTrack() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  _observer = new rtc::RefCountedObject<MediaStreamTrackObserver>(this);
  EventEmitter::Start(true);
}

MediaStreamTrack::~MediaStreamTrack() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_track.get()) {
    _track->UnregisterObserver(_observer.get());
  }

  EventEmitter::End();
}

NAN_METHOD(MediaStreamTrack::New) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();

  if (args.IsConstructCall()) {
    MediaStreamTrack* mediaStreamTrack = new MediaStreamTrack();
    mediaStreamTrack->Wrap(args.This(), "MediaStreamTrack");
    NanReturnValue(args.This());
  }

  NanThrowError("Internal Error");
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

NAN_METHOD(MediaStreamTrack::GetConstraints) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.This(), "MediaStreamTrack");

  NanReturnUndefined();
}

NAN_METHOD(MediaStreamTrack::ApplyConstraints) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.This(), "MediaStreamTrack");
  
  NanReturnUndefined();
}

NAN_METHOD(MediaStreamTrack::GetSettings) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.This(), "MediaStreamTrack");

  NanReturnUndefined();
}

NAN_METHOD(MediaStreamTrack::GetCapabilities) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.This(), "MediaStreamTrack");

  NanReturnUndefined();
}

NAN_METHOD(MediaStreamTrack::Clone) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.This(), "MediaStreamTrack");

  NanReturnUndefined();
}

NAN_METHOD(MediaStreamTrack::Stop) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.This(), "MediaStreamTrack");

  NanReturnUndefined();
}

NAN_GETTER(MediaStreamTrack::GetEnabled) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
  
  NanReturnUndefined();
}

NAN_GETTER(MediaStreamTrack::GetId) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
  
  NanReturnUndefined();
}

NAN_GETTER(MediaStreamTrack::GetKind) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
  
  NanReturnUndefined();
}

NAN_GETTER(MediaStreamTrack::GetLabel) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
  
  NanReturnUndefined();
}

NAN_GETTER(MediaStreamTrack::GetMuted) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
  
  NanReturnUndefined();
}

NAN_GETTER(MediaStreamTrack::GetReadOnly) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanReturnValue(NanNew(true));
}

NAN_GETTER(MediaStreamTrack::GetReadyState) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
  
  NanReturnUndefined();
}

NAN_GETTER(MediaStreamTrack::GetRemote) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
  
  NanReturnUndefined();
}

NAN_GETTER(MediaStreamTrack::GetOnStarted) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");
  NanReturnValue(NanNew(self->_onstarted));
}

NAN_GETTER(MediaStreamTrack::GetOnMute) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");
  NanReturnValue(NanNew(self->_onmute));
}

NAN_GETTER(MediaStreamTrack::GetOnUnMute) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");
  NanReturnValue(NanNew(self->_onunmute));
}

NAN_GETTER(MediaStreamTrack::GetOnOverConstrained) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");
  NanReturnValue(NanNew(self->_onoverconstrained));
}

NAN_GETTER(MediaStreamTrack::GetOnEnded) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");
  NanReturnValue(NanNew(self->_onended));
}

NAN_SETTER(MediaStreamTrack::ReadOnly) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
}

NAN_SETTER(MediaStreamTrack::SetEnabled) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  //MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");

  // TODO(): Implement This
}

NAN_SETTER(MediaStreamTrack::SetOnStarted) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onstarted, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onstarted);
  }
}

NAN_SETTER(MediaStreamTrack::SetOnMute) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onmute, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onmute);
  }
}

NAN_SETTER(MediaStreamTrack::SetOnUnMute) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onunmute, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onunmute);
  }
}

NAN_SETTER(MediaStreamTrack::SetOnOverConstrained) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onoverconstrained, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onoverconstrained);
  }
}

NAN_SETTER(MediaStreamTrack::SetOnEnded) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  MediaStreamTrack *self = RTCWrap::Unwrap<MediaStreamTrack>(args.Holder(), "MediaStreamTrack");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onended, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onended);
  }
}

void MediaStreamTrack::On(Event *event) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamTrackEvent type = event->Type<MediaStreamTrackEvent>();

  if (type != kMediaStreamTrackChanged) {
    return;
  }
}
