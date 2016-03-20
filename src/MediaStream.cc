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

Nan::Persistent<Function> MediaStream::constructor;

void MediaStream::Init() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(MediaStream::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("MediaStream").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "addTrack", MediaStream::AddTrack);
  Nan::SetPrototypeMethod(tpl, "removeTrack", MediaStream::RemoveTrack);
  Nan::SetPrototypeMethod(tpl, "clone", MediaStream::Clone);
  Nan::SetPrototypeMethod(tpl, "getAudioTracks", MediaStream::GetAudioTracks);
  Nan::SetPrototypeMethod(tpl, "getTrackById", MediaStream::GetTrackById);
  Nan::SetPrototypeMethod(tpl, "getVideoTracks", MediaStream::GetVideoTracks);
  Nan::SetPrototypeMethod(tpl, "getTracks", MediaStream::GetTracks);

  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("active").ToLocalChecked(), MediaStream::GetActive);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("ended").ToLocalChecked(), MediaStream::GetEnded);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("id").ToLocalChecked(), MediaStream::GetId);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onaddtrack").ToLocalChecked(), MediaStream::GetOnAddTrack, MediaStream::SetOnAddTrack);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("onremovetrack").ToLocalChecked(), MediaStream::GetOnRemoveTrack, MediaStream::SetOnRemoveTrack);

  constructor.Reset(tpl->GetFunction());
}

Local<Value> MediaStream::New(rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::EscapableHandleScope scope;

  Local<Value> empty;
  Local<Function> instance = Nan::New(MediaStream::constructor);

  if (instance.IsEmpty() || !mediaStream.get()) {
    return scope.Escape(Nan::Null());
  }

  Local<Object> ret = instance->NewInstance();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(ret, "MediaStream");
  
  if (self) {   
    self->_stream = mediaStream;
    self->_audio_tracks = self->_stream->GetAudioTracks();
    self->_video_tracks = self->_stream->GetVideoTracks();
    self->_stream->RegisterObserver(self->_observer.get());
    self->CheckState();
    
    return scope.Escape(ret);
  }

  return scope.Escape(Nan::Null());
}

MediaStream::MediaStream() :
  _active(false),
  _ended(true)
{
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  _observer = new rtc::RefCountedObject<MediaStreamObserver>(this);
}

MediaStream::~MediaStream() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_stream.get()) {
    _stream->UnregisterObserver(_observer.get());
  }
  
  _observer->RemoveListener(this);
}

void MediaStream::New(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  Nan::HandleScope scope;

  if (info.IsConstructCall()) {
    MediaStream* mediaStream = new MediaStream();
    mediaStream->Wrap(info.This(), "MediaStream");
    return info.GetReturnValue().Set(info.This());
  }
  
  Nan::ThrowError("Internal Error");
  info.GetReturnValue().SetUndefined();
}

rtc::scoped_refptr<webrtc::MediaStreamInterface> MediaStream::Unwrap(Local<Object> value) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (!value.IsEmpty()) {
    MediaStream *self = RTCWrap::Unwrap<MediaStream>(value, "MediaStream");

    if (self) {
      return self->_stream;
    }
  }

  return 0;
}

rtc::scoped_refptr<webrtc::MediaStreamInterface> MediaStream::Unwrap(Local<Value> value) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (!value.IsEmpty() && value->IsObject()) {
    Local<Object> stream = Local<Object>::Cast(value);
    return MediaStream::Unwrap(stream);
  }

  return 0;
}

void MediaStream::AddTrack(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = MediaStream::Unwrap(info.This());
  bool retval = false;

  if (stream.get()) {
    if (info.Length() >= 1 && info[0]->IsObject()) {
      rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track = MediaStreamTrack::Unwrap(info[0]);

      if (track.get()) {
        std::string kind = track->kind();

        if (kind.compare("audio") == 0) {
          rtc::scoped_refptr<webrtc::AudioTrackInterface> audio(static_cast<webrtc::AudioTrackInterface*>(track.get()));
          retval = stream->AddTrack(audio);
        } else {
          rtc::scoped_refptr<webrtc::VideoTrackInterface> video(static_cast<webrtc::VideoTrackInterface*>(track.get()));
          retval = stream->AddTrack(video);
        }
      }
    }
  } else {
    Nan::ThrowError("Internal Error");
  }

  return info.GetReturnValue().Set(Nan::New(retval));
}

void MediaStream::RemoveTrack(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = MediaStream::Unwrap(info.This());
  bool retval = false;

  if (stream.get()) {
    if (info.Length() >= 1 && info[0]->IsObject()) {
      rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track = MediaStreamTrack::Unwrap(info[0]);

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
    Nan::ThrowError("Internal Error");
  }

  return info.GetReturnValue().Set(Nan::New(retval));
}

void MediaStream::Clone(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::MediaStreamInterface> self = MediaStream::Unwrap(info.This());
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

      return info.GetReturnValue().Set(MediaStream::New(stream));
    }
  }
  
  Nan::ThrowError("Internal Error");
  info.GetReturnValue().SetUndefined();
}

void MediaStream::GetTrackById(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = MediaStream::Unwrap(info.This());

  if (stream.get()) {
    if (info.Length() >= 1 && info[0]->IsString()) {
      v8::String::Utf8Value idValue(info[0]->ToString());
      std::string id(*idValue);

      rtc::scoped_refptr<webrtc::AudioTrackInterface> audio = stream->FindAudioTrack(id);

      if (audio.get()) {
        return info.GetReturnValue().Set(MediaStreamTrack::New(audio.get()));
      }

      rtc::scoped_refptr<webrtc::VideoTrackInterface> video = stream->FindVideoTrack(id);

      if (video.get()) {
        return info.GetReturnValue().Set(MediaStreamTrack::New(video.get()));
      }
    }

    return info.GetReturnValue().Set(Nan::Null());
  } else {
    Nan::ThrowError("Internal Error");
  }
  
  info.GetReturnValue().SetUndefined();
}

void MediaStream::GetAudioTracks(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::MediaStreamInterface> self = MediaStream::Unwrap(info.This());

  if (self.get()) {
    webrtc::AudioTrackVector audio_list = self->GetAudioTracks();
    std::vector<rtc::scoped_refptr<webrtc::AudioTrackInterface> >::iterator audio_it;
    Local<Array> list = Nan::New<Array>();
    uint32_t index = 0;

    for (audio_it = audio_list.begin(); audio_it != audio_list.end(); audio_it++) {
      rtc::scoped_refptr<webrtc::AudioTrackInterface> track(*audio_it);

      if (track.get()) {
        list->Set(index, MediaStreamTrack::New(track.get()));
        index++;
      }
    }

    return info.GetReturnValue().Set(list);
  } else {
    Nan::ThrowError("Internal Error");
  }
  
  info.GetReturnValue().SetUndefined();
}

void MediaStream::GetVideoTracks(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::MediaStreamInterface> self = MediaStream::Unwrap(info.This());

  if (self.get()) {
    webrtc::VideoTrackVector video_list = self->GetVideoTracks();
    std::vector<rtc::scoped_refptr<webrtc::VideoTrackInterface> >::iterator video_it;
    Local<Array> list = Nan::New<Array>();
    uint32_t index = 0;

    for (video_it = video_list.begin(); video_it != video_list.end(); video_it++) {
      rtc::scoped_refptr<webrtc::VideoTrackInterface> track(*video_it);

      if (track.get()) {
        list->Set(index, MediaStreamTrack::New(track.get()));
        index++;
      }
    }

    return info.GetReturnValue().Set(list);
  } else {
    Nan::ThrowError("Internal Error");
  }
  
  info.GetReturnValue().SetUndefined();
}

void MediaStream::GetTracks(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::MediaStreamInterface> self = MediaStream::Unwrap(info.This());

  if (self.get()) {
    webrtc::AudioTrackVector audio_list = self->GetAudioTracks();
    webrtc::VideoTrackVector video_list = self->GetVideoTracks();
    
    std::vector<rtc::scoped_refptr<webrtc::AudioTrackInterface> >::iterator audio_it;
    std::vector<rtc::scoped_refptr<webrtc::VideoTrackInterface> >::iterator video_it;
    
    Local<Array> list = Nan::New<Array>();
    uint32_t index = 0;

    for (audio_it = audio_list.begin(); audio_it != audio_list.end(); audio_it++) {
      rtc::scoped_refptr<webrtc::AudioTrackInterface> track(*audio_it);

      if (track.get()) {
        list->Set(index, MediaStreamTrack::New(track.get()));
        index++;
      }
    }
 
    for (video_it = video_list.begin(); video_it != video_list.end(); video_it++) {
      rtc::scoped_refptr<webrtc::VideoTrackInterface> track(*video_it);

      if (track.get()) {
        list->Set(index, MediaStreamTrack::New(track.get()));
        index++;
      }
    }

    return info.GetReturnValue().Set(list);
  } else {
    Nan::ThrowError("Internal Error");
  }
  
  info.GetReturnValue().SetUndefined();
}

void MediaStream::GetActive(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  MediaStream *self = RTCWrap::Unwrap<MediaStream>(info.Holder(), "MediaStream");
  return info.GetReturnValue().Set(Nan::New(self->_active));
}

void MediaStream::GetEnded(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  MediaStream *self = RTCWrap::Unwrap<MediaStream>(info.Holder(), "MediaStream");
  return info.GetReturnValue().Set(Nan::New(self->_ended));
}

void MediaStream::GetId(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = MediaStream::Unwrap(info.Holder());
  
  if (stream.get()) {
    return info.GetReturnValue().Set(Nan::New(stream->label().c_str()).ToLocalChecked());
  } else {
    Nan::ThrowError("Internal Error");
  }
  
  info.GetReturnValue().SetUndefined();
}

void MediaStream::GetOnAddTrack(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(info.Holder(), "MediaStream");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onaddtrack));
}

void MediaStream::GetOnRemoveTrack(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(info.Holder(), "MediaStream");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onremovetrack));
}

void MediaStream::ReadOnly(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

void MediaStream::SetOnAddTrack(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(info.Holder(), "MediaStream");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onaddtrack.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onaddtrack.Reset();
  }
}

void MediaStream::SetOnRemoveTrack(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(info.Holder(), "MediaStream");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onremovetrack.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onremovetrack.Reset();
  }
}

void MediaStream::CheckState() {
  _active = false;
  _ended = true;
  
  webrtc::AudioTrackVector new_audio_tracks = _stream->GetAudioTracks();
  webrtc::VideoTrackVector new_video_tracks = _stream->GetVideoTracks();

  for (const auto& cached_track : _audio_tracks) {
    auto it = std::find_if(
        new_audio_tracks.begin(), new_audio_tracks.end(),
        [cached_track](const webrtc::AudioTrackVector::value_type& new_track) {
          return new_track->id().compare(cached_track->id()) == 0;
        });
    if (it == new_audio_tracks.end()) {
      Local<Function> callback = Nan::New<Function>(_onremovetrack);
      Local<Value> argv[] = {
        MediaStreamTrack::New(cached_track.get())
      };

      if (!callback.IsEmpty() && callback->IsFunction()) {
        callback->Call(RTCWrap::This(), 1, argv);
      }
    }
  }
  
  for (const auto& new_track : new_audio_tracks) {
    if (new_track->state() == webrtc::MediaStreamTrackInterface::kLive) {
      _active = true;
      _ended = false;
    }
    
    auto it = std::find_if(
        _audio_tracks.begin(), _audio_tracks.end(),
        [new_track](const webrtc::AudioTrackVector::value_type& cached_track) {
          return new_track->id().compare(cached_track->id()) == 0;
        });
    if (it == _audio_tracks.end()) {
      Local<Function> callback = Nan::New<Function>(_onaddtrack);
      Local<Value> argv[] = {
        MediaStreamTrack::New(new_track.get())
      };

      if (!callback.IsEmpty() && callback->IsFunction()) {
        callback->Call(RTCWrap::This(), 1, argv);
      }
    }
  }
  
  for (const auto& cached_track : _video_tracks) {
    auto it = std::find_if(
        new_video_tracks.begin(), new_video_tracks.end(),
        [cached_track](const webrtc::VideoTrackVector::value_type& new_track) {
          return new_track->id().compare(cached_track->id()) == 0;
        });
    if (it == new_video_tracks.end()) {
      Local<Function> callback = Nan::New<Function>(_onremovetrack);
      Local<Value> argv[] = {
        MediaStreamTrack::New(cached_track.get())
      };

      if (!callback.IsEmpty() && callback->IsFunction()) {
        callback->Call(RTCWrap::This(), 1, argv);
      }
    }
  }
  
  for (const auto& new_track : new_video_tracks) {
    if (new_track->state() == webrtc::MediaStreamTrackInterface::kLive) {
      _active = true;
      _ended = false;
    }
    
    auto it = std::find_if(
        _video_tracks.begin(), _video_tracks.end(),
        [new_track](const webrtc::VideoTrackVector::value_type& cached_track) {
          return new_track->id().compare(cached_track->id()) == 0;
        });
    if (it == _video_tracks.end()) {
      Local<Function> callback = Nan::New<Function>(_onaddtrack);
      Local<Value> argv[] = {
        MediaStreamTrack::New(new_track.get())
      };

      if (!callback.IsEmpty() && callback->IsFunction()) {
        callback->Call(RTCWrap::This(), 1, argv);
      }
    }
  }
  
  _audio_tracks = new_audio_tracks;
  _video_tracks = new_video_tracks;
}

void MediaStream::On(Event *event) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;
  MediaStreamEvent type = event->Type<MediaStreamEvent>();

  if (type != kMediaStreamChanged) {
    Nan::ThrowError("Internal Error");
    return;
  }
  
  MediaStream::CheckState();
}
