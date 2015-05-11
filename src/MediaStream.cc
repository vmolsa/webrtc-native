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
#include "MediaStreamTrack.h"

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
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, ret, "MediaStream");

  self->Start();
  self->_stream = mediaStream;
  self->_stream->RegisterObserver(self->_observer.get());
  self->Emit(kMediaStreamChanged);

  return scope.Escape(ret);
}

MediaStream::MediaStream() :
  _ended(true)
{
  _observer = new rtc::RefCountedObject<MediaStreamObserver>(this);
}

MediaStream::~MediaStream() {
  if (_stream.get()) {
    _stream->UnregisterObserver(_observer.get());
  }

  EventEmitter::End();
}

void MediaStream::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    MediaStream* mediaStream = new MediaStream();
    mediaStream->Wrap(isolate, args.This(), "MediaStream");
    return args.GetReturnValue().Set(args.This());
  }

  isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
}

rtc::scoped_refptr<webrtc::MediaStreamInterface> MediaStream::Unwrap(Isolate *isolate, Local<Object> value) {
  if (!value.IsEmpty()) {
    MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, value, "MediaStream");

    if (self) {
      return self->_stream;
    }
  }

  return 0;
}

rtc::scoped_refptr<webrtc::MediaStreamInterface> MediaStream::Unwrap(Isolate *isolate, Local<Value> value) {
  if (!value.IsEmpty() && value->IsObject()) {
    Local<Object> stream = Local<Object>::Cast(value);
    return MediaStream::Unwrap(isolate, stream);
  }

  return 0;
}

void MediaStream::AddTrack(const FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetIsolate();
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = MediaStream::Unwrap(isolate, args.This());
  bool retval = false;

  if (stream.get()) {
    if (args.Length() >= 1 && args[0]->IsObject()) {
      rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track = MediaStreamTrack::Unwrap(isolate, args[0]);

      if (track.get()) {
        std::string kind = track->kind();

        if (kind.compare("audio") == 0) {
          rtc::scoped_refptr<webrtc::AudioTrackInterface> audio(static_cast<webrtc::AudioTrackInterface*>(track.get()));
          retval = stream->AddTrack(audio);
        }
        else {
          rtc::scoped_refptr<webrtc::VideoTrackInterface> video(static_cast<webrtc::VideoTrackInterface*>(track.get()));
          retval = stream->AddTrack(video);
        }
      }
    }
  }
  else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
  }

  args.GetReturnValue().Set(v8::Boolean::New(isolate, retval));
}

void MediaStream::RemoveTrack(const FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetIsolate();
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = MediaStream::Unwrap(isolate, args.This());
  bool retval = false;

  if (stream.get()) {
    if (args.Length() >= 1 && args[0]->IsObject()) {
      rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track = MediaStreamTrack::Unwrap(isolate, args[0]);

      if (track.get()) {
        std::string kind = track->kind();

        if (kind.compare("audio") == 0) {
          rtc::scoped_refptr<webrtc::AudioTrackInterface> audio(static_cast<webrtc::AudioTrackInterface*>(track.get()));
          retval = stream->RemoveTrack(audio);
        } else {
          rtc::scoped_refptr<webrtc::VideoTrackInterface> video(static_cast<webrtc::VideoTrackInterface*>(track.get()));
          retval = stream->RemoveTrack(video);
        }
      }
    }
  } else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
  }

  args.GetReturnValue().Set(v8::Boolean::New(isolate, retval));
}

void MediaStream::Clone(const FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetIsolate();
  rtc::scoped_refptr<webrtc::MediaStreamInterface> self = MediaStream::Unwrap(isolate, args.This());
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory = webrtc::CreatePeerConnectionFactory();
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream;

  if (self.get() && factory.get()) {
    stream = factory->CreateLocalMediaStream("stream");

    if (stream.get()) {
      webrtc::AudioTrackVector audio_list = self->GetAudioTracks();
      std::vector<rtc::scoped_refptr<webrtc::AudioTrackInterface> >::iterator audio_it;

      for (audio_it = audio_list.begin(); audio_it != audio_list.end(); audio_it++) {
        rtc::scoped_refptr<webrtc::AudioTrackInterface> track(*audio_it);

        if (track.get()) {
          stream->AddTrack(track.get());
        }
      }

      webrtc::VideoTrackVector video_list = self->GetVideoTracks();
      std::vector<rtc::scoped_refptr<webrtc::VideoTrackInterface> >::iterator video_it;

      for (video_it = video_list.begin(); video_it != video_list.end(); video_it++) {
        rtc::scoped_refptr<webrtc::VideoTrackInterface> track(*video_it);

        if (track.get()) {
          stream->AddTrack(track.get());
        }
      }

      return args.GetReturnValue().Set(MediaStream::New(isolate, stream));
    }
  }
  
  isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
}

void MediaStream::GetTrackById(const FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetIsolate();
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = MediaStream::Unwrap(isolate, args.This());

  if (stream.get()) {
    if (args.Length() >= 1 && args[0]->IsString()) {
      v8::String::Utf8Value idValue(args[0]->ToString());
      std::string id(*idValue);

      rtc::scoped_refptr<webrtc::AudioTrackInterface> audio = stream->FindAudioTrack(id);

      if (audio.get()) {
        return args.GetReturnValue().Set(MediaStreamTrack::New(isolate, audio.get()));
      }

      rtc::scoped_refptr<webrtc::VideoTrackInterface> video = stream->FindVideoTrack(id);

      if (video.get()) {
        return args.GetReturnValue().Set(MediaStreamTrack::New(isolate, video.get()));
      }
    }

    Local<Value> empty = Null(isolate);
    args.GetReturnValue().Set(empty);
  } else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
  }
}

void MediaStream::GetAudioTracks(const FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetIsolate();
  rtc::scoped_refptr<webrtc::MediaStreamInterface> self = MediaStream::Unwrap(isolate, args.This());

  if (self.get()) {
    webrtc::AudioTrackVector audio_list = self->GetAudioTracks();
    std::vector<rtc::scoped_refptr<webrtc::AudioTrackInterface> >::iterator audio_it;
    Local<Array> list = Array::New(isolate);
    uint32_t index = 0;

    for (audio_it = audio_list.begin(); audio_it != audio_list.end(); audio_it++) {
      rtc::scoped_refptr<webrtc::AudioTrackInterface> track(*audio_it);

      if (track.get()) {
        list->Set(index, MediaStreamTrack::New(isolate, track.get()));
        index++;
      }
    }

    args.GetReturnValue().Set(list);
  } else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
  }
}

void MediaStream::GetVideoTracks(const FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetIsolate();
  rtc::scoped_refptr<webrtc::MediaStreamInterface> self = MediaStream::Unwrap(isolate, args.This());

  if (self.get()) {
    webrtc::VideoTrackVector video_list = self->GetVideoTracks();
    std::vector<rtc::scoped_refptr<webrtc::VideoTrackInterface> >::iterator video_it;
    Local<Array> list = Array::New(isolate);
    uint32_t index = 0;

    for (video_it = video_list.begin(); video_it != video_list.end(); video_it++) {
      rtc::scoped_refptr<webrtc::VideoTrackInterface> track(*video_it);

      if (track.get()) {
        list->Set(index, MediaStreamTrack::New(isolate, track.get()));
        index++;
      }
    }

    args.GetReturnValue().Set(list);
  } else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
  }
}

void MediaStream::GetEnded(Local<String> property,
                           const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, info.Holder(), "MediaStream");
  info.GetReturnValue().Set(v8::Boolean::New(isolate, self->_ended));
}

void MediaStream::GetId(Local<String> property,
                        const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = MediaStream::Unwrap(isolate, info.Holder());
  
  if (stream.get()) {
    std::string label = stream->label();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, label.c_str()));
  } else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Internal Error")));
  }
}

void MediaStream::GetOnAddTrack(Local<String> property,
                                const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, info.Holder(), "MediaStream");
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onaddtrack));
}

void MediaStream::GetOnRemoveTrack(Local<String> property,
                                   const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, info.Holder(), "MediaStream");
  info.GetReturnValue().Set(Local<Function>::New(isolate, self->_onremovetrack));
}

void MediaStream::GetOnEnded(Local<String> property,
                             const PropertyCallbackInfo<Value> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, info.Holder(), "MediaStream");
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
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, info.Holder(), "MediaStream");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onaddtrack.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_onaddtrack.Reset();
  }
}

void MediaStream::SetOnRemoveTrack(Local<String> property,
                                   Local<Value> value,
                                   const PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, info.Holder(), "MediaStream");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onremovetrack.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_onremovetrack.Reset();
  }
}

void MediaStream::SetOnEnded(Local<String> property,
                             Local<Value> value,
                             const PropertyCallbackInfo<void> &info)
{
  Isolate *isolate = info.GetIsolate();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(isolate, info.Holder(), "MediaStream");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onended.Reset(isolate, Local<Function>::Cast(value));
  } else {
    self->_onended.Reset();
  }
}

void MediaStream::On(Event *event) {
  MediaStreamEvent type = event->Type<MediaStreamEvent>();

  if (type != kMediaStreamChanged) {
    return;
  }

  EventEmitter::Stop();
}
