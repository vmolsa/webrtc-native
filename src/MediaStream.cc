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

#include "MediaStream.h"

using namespace v8;
using namespace WebRTC;

Persistent<Function> MediaStream::constructor;

void MediaStream::Init() {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, MediaStream::New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "MediaStream"));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "addTrack"),
                                FunctionTemplate::New(isolate, MediaStream::AddTrack));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "removeTrack"),
                                FunctionTemplate::New(isolate, MediaStream::RemoveTrack));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "clone"),
                                FunctionTemplate::New(isolate, MediaStream::Clone));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "getAudioTracks"),
                                FunctionTemplate::New(isolate, MediaStream::GetAudioTracks));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "getTrackById"),
                                FunctionTemplate::New(isolate, MediaStream::GetTrackById));

  tpl->PrototypeTemplate()->Set(String::NewFromUtf8(isolate, "getVideoTracks"),
                                FunctionTemplate::New(isolate, MediaStream::GetVideoTracks));


  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "ended"),
                                       MediaStream::GetEnded,
                                       MediaStream::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "id"),
                                       MediaStream::GetId,
                                       MediaStream::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onaddtrack"),
                                       MediaStream::GetOnAddTrack,
                                       MediaStream::SetOnAddTrack);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onremovetrack"),
                                       MediaStream::GetOnRemoveTrack,
                                       MediaStream::SetOnRemoveTrack);

  tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "onended"),
                                       MediaStream::GetOnEnded,
                                       MediaStream::SetOnEnded);

  constructor.Reset(isolate, tpl->GetFunction());
}

Local<Value> MediaStream::New(Isolate *isolate, rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream) {
  EscapableHandleScope scope(isolate);

  Local<Value> argv[1];
  Local<Function> instance = Local<Function>::New(isolate, MediaStream::constructor);

  if (instance.IsEmpty() || !mediaStream.get()) {
    Local<Value> empty = Null(isolate);
    return scope.Escape(empty);
  }

  Local<Object> ret = instance->NewInstance(0, argv);
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, ret);

  self->_stream = mediaStream;

  // TODO(): Implement This

  return scope.Escape(ret);
}

MediaStream::MediaStream() {
  
}

MediaStream::~MediaStream() {

}

void MediaStream::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    MediaStream* mediaStream = new MediaStream();
    mediaStream->Wrap(isolate, args.This());
    return args.GetReturnValue().Set(args.This());
  }

  isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
}

void MediaStream::AddTrack(const FunctionCallbackInfo<Value>& args) {
  //Isolate *isolate = args.GetIsolate();
  //MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, args.This());

  // TODO(): Implement This
}

void MediaStream::Clone(const FunctionCallbackInfo<Value>& args) {
  //Isolate *isolate = args.GetIsolate();
  //MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, args.This());

  // TODO(): Implement This
}

void MediaStream::GetTrackById(const FunctionCallbackInfo<Value>& args) {
  //Isolate *isolate = args.GetIsolate();
  //MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, args.This());

  // TODO(): Implement This
}

void MediaStream::GetAudioTracks(const FunctionCallbackInfo<Value>& args) {
  //Isolate *isolate = args.GetIsolate();
  //MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, args.This());

  // TODO(): Implement This
}

void MediaStream::GetVideoTracks(const FunctionCallbackInfo<Value>& args) {
  //Isolate *isolate = args.GetIsolate();
  //MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, args.This());

  // TODO(): Implement This
}

void MediaStream::RemoveTrack(const FunctionCallbackInfo<Value>& args) {
  //Isolate *isolate = args.GetIsolate();
  //MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, args.This());

  // TODO(): Implement This
}

void MediaStream::GetEnded(Local<String> property,
                           const PropertyCallbackInfo<Value> &info)
{
  //Isolate *isolate = args.GetIsolate();
  //MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, args.This());

  // TODO(): Implement This
}
void MediaStream::GetId(Local<String> property,
                        const PropertyCallbackInfo<Value> &info)
{
  //Isolate *isolate = args.GetIsolate();
  //MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, args.This());

  // TODO(): Implement This
}

void MediaStream::GetOnAddTrack(Local<String> property,
                                const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, info.Holder());
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onaddtrack));
}

void MediaStream::GetOnRemoveTrack(Local<String> property,
                                   const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, info.Holder());
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onremovetrack));
}

void MediaStream::GetOnEnded(Local<String> property,
                             const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, info.Holder());
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onended));
}

void MediaStream::ReadOnly(Local<String> property,
                           Local<Value> value,
                           const PropertyCallbackInfo<void> &info)
{

}

void MediaStream::SetOnAddTrack(Local<String> property,
                                Local<Value> value,
                                const PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onaddtrack.Reset(isolate, Local<Function>::Cast(value));
  }
  else {
    self->_onaddtrack.Reset();
  }
}

void MediaStream::SetOnRemoveTrack(Local<String> property,
                                   Local<Value> value,
                                   const PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onremovetrack.Reset(isolate, Local<Function>::Cast(value));
  }
  else {
    self->_onremovetrack.Reset();
  }
}

void MediaStream::SetOnEnded(Local<String> property,
                             Local<Value> value,
                             const PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onended.Reset(isolate, Local<Function>::Cast(value));
  }
  else {
    self->_onended.Reset();
  }
}

void MediaStream::On(Event *event) {

}
