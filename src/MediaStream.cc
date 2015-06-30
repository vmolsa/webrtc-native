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
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(MediaStream::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew("MediaStream"));

  tpl->PrototypeTemplate()->Set(NanNew("addTrack"), NanNew<FunctionTemplate>(MediaStream::AddTrack)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("removeTrack"), NanNew<FunctionTemplate>(MediaStream::RemoveTrack)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("clone"), NanNew<FunctionTemplate>(MediaStream::Clone)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("getAudioTracks"), NanNew<FunctionTemplate>(MediaStream::GetAudioTracks)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("getTrackById"), NanNew<FunctionTemplate>(MediaStream::GetTrackById)->GetFunction());
  tpl->PrototypeTemplate()->Set(NanNew("getVideoTracks"), NanNew<FunctionTemplate>(MediaStream::GetVideoTracks)->GetFunction());

  tpl->InstanceTemplate()->SetAccessor(NanNew("ended"),
                                       MediaStream::GetEnded,
                                       MediaStream::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(NanNew("id"),
                                       MediaStream::GetId,
                                       MediaStream::ReadOnly);

  tpl->InstanceTemplate()->SetAccessor(NanNew("onaddtrack"),
                                       MediaStream::GetOnAddTrack,
                                       MediaStream::SetOnAddTrack);

  tpl->InstanceTemplate()->SetAccessor(NanNew("onremovetrack"),
                                       MediaStream::GetOnRemoveTrack,
                                       MediaStream::SetOnRemoveTrack);

  tpl->InstanceTemplate()->SetAccessor(NanNew("onended"),
                                       MediaStream::GetOnEnded,
                                       MediaStream::SetOnEnded);

  NanAssignPersistent(constructor, tpl->GetFunction());
}

Local<Value> MediaStream::New(rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanEscapableScope();

  Local<Value> empty;
  Local<Value> argv[1];
  Local<Function> instance = NanNew(MediaStream::constructor);

  if (instance.IsEmpty() || !mediaStream.get()) {
    return NanEscapeScope(NanNull());
  }

  Local<Object> ret = instance->NewInstance(0, argv);
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(ret, "MediaStream");
  
  if (self) {
    self->_stream = mediaStream;
    self->_stream->RegisterObserver(self->_observer.get());
    self->Emit(kMediaStreamChanged);
    
    return NanEscapeScope(ret);
  }

  return NanEscapeScope(NanNull());
}

MediaStream::MediaStream() :
  _ended(true)
{
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  _observer = new rtc::RefCountedObject<MediaStreamObserver>(this);
}

MediaStream::~MediaStream() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_stream.get()) {
    _stream->UnregisterObserver(_observer.get());
    _observer->SetEmitter();
  }
}

NAN_METHOD(MediaStream::New) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();

  if (args.IsConstructCall()) {
    MediaStream* mediaStream = new MediaStream();
    mediaStream->Wrap(args.This(), "MediaStream");
    NanReturnValue(args.This());
  }
  
  NanThrowError("Internal Error");
  NanReturnUndefined();
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

NAN_METHOD(MediaStream::AddTrack) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
   
  NanScope();
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = MediaStream::Unwrap(args.This());
  bool retval = false;

  if (stream.get()) {
    if (args.Length() >= 1 && args[0]->IsObject()) {
      rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track = MediaStreamTrack::Unwrap(args[0]);

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
    NanThrowError("Internal Error");
  }

  NanReturnValue(NanNew(retval));
}

NAN_METHOD(MediaStream::RemoveTrack) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = MediaStream::Unwrap(args.This());
  bool retval = false;

  if (stream.get()) {
    if (args.Length() >= 1 && args[0]->IsObject()) {
      rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track = MediaStreamTrack::Unwrap(args[0]);

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
    NanThrowError("Internal Error");
  }

  NanReturnValue(NanNew(retval));
}

NAN_METHOD(MediaStream::Clone) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  rtc::scoped_refptr<webrtc::MediaStreamInterface> self = MediaStream::Unwrap(args.This());
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

      NanReturnValue(MediaStream::New(stream));
    }
  }
  
  NanThrowError("Internal Error");
  NanReturnUndefined();
}

NAN_METHOD(MediaStream::GetTrackById) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = MediaStream::Unwrap(args.This());

  if (stream.get()) {
    if (args.Length() >= 1 && args[0]->IsString()) {
      v8::String::Utf8Value idValue(args[0]->ToString());
      std::string id(*idValue);

      rtc::scoped_refptr<webrtc::AudioTrackInterface> audio = stream->FindAudioTrack(id);

      if (audio.get()) {
        NanReturnValue(MediaStreamTrack::New(audio.get()));
      }

      rtc::scoped_refptr<webrtc::VideoTrackInterface> video = stream->FindVideoTrack(id);

      if (video.get()) {
        NanReturnValue(MediaStreamTrack::New(video.get()));
      }
    }

    NanReturnValue(NanNull());
  } else {
    NanThrowError("Internal Error");
  }
  
  NanReturnUndefined();
}

NAN_METHOD(MediaStream::GetAudioTracks) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  rtc::scoped_refptr<webrtc::MediaStreamInterface> self = MediaStream::Unwrap(args.This());

  if (self.get()) {
    webrtc::AudioTrackVector audio_list = self->GetAudioTracks();
    std::vector<rtc::scoped_refptr<webrtc::AudioTrackInterface> >::iterator audio_it;
    Local<Array> list = NanNew<Array>();
    uint32_t index = 0;

    for (audio_it = audio_list.begin(); audio_it != audio_list.end(); audio_it++) {
      rtc::scoped_refptr<webrtc::AudioTrackInterface> track(*audio_it);

      if (track.get()) {
        list->Set(index, MediaStreamTrack::New(track.get()));
        index++;
      }
    }

    NanReturnValue(list);
  } else {
    NanThrowError("Internal Error");
  }
  
  NanReturnUndefined();
}

NAN_METHOD(MediaStream::GetVideoTracks) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope(); 
  rtc::scoped_refptr<webrtc::MediaStreamInterface> self = MediaStream::Unwrap(args.This());

  if (self.get()) {
    webrtc::VideoTrackVector video_list = self->GetVideoTracks();
    std::vector<rtc::scoped_refptr<webrtc::VideoTrackInterface> >::iterator video_it;
    Local<Array> list = NanNew<Array>();
    uint32_t index = 0;

    for (video_it = video_list.begin(); video_it != video_list.end(); video_it++) {
      rtc::scoped_refptr<webrtc::VideoTrackInterface> track(*video_it);

      if (track.get()) {
        list->Set(index, MediaStreamTrack::New(track.get()));
        index++;
      }
    }

    NanReturnValue(list);
  } else {
    NanThrowError("Internal Error");
  }
  
  NanReturnUndefined();
}

NAN_GETTER(MediaStream::GetEnded) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope(); 
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(args.Holder(), "MediaStream");
  NanReturnValue(NanNew(self->_ended));
}

NAN_GETTER(MediaStream::GetId) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = MediaStream::Unwrap(args.Holder());
  
  if (stream.get()) {
    NanReturnValue(NanNew(stream->label().c_str()));
  } else {
    NanThrowError("Internal Error");
  }
  
  NanReturnUndefined();
}

NAN_GETTER(MediaStream::GetOnAddTrack) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(args.Holder(), "MediaStream");
  NanReturnValue(NanNew(self->_onaddtrack));
}

NAN_GETTER(MediaStream::GetOnRemoveTrack) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(args.Holder(), "MediaStream");
  NanReturnValue(NanNew(self->_onremovetrack));
}

NAN_GETTER(MediaStream::GetOnEnded) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(args.Holder(), "MediaStream");
  NanReturnValue(NanNew(self->_onended));
}

NAN_SETTER(MediaStream::ReadOnly) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

NAN_SETTER(MediaStream::SetOnAddTrack) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(args.Holder(), "MediaStream");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onaddtrack, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onaddtrack);
  }
}

NAN_SETTER(MediaStream::SetOnRemoveTrack) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(args.Holder(), "MediaStream");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onremovetrack, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onremovetrack);
  }
}

NAN_SETTER(MediaStream::SetOnEnded) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  MediaStream *self = RTCWrap::Unwrap<MediaStream>(args.Holder(), "MediaStream");

  if (!value.IsEmpty() && value->IsFunction()) {
    NanAssignPersistent(self->_onended, Local<Function>::Cast(value));
  } else {
    NanDisposePersistent(self->_onended);
  }
}

void MediaStream::On(Event *event) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  MediaStreamEvent type = event->Type<MediaStreamEvent>();
  Local<Function> onended;
  Local<Value> onended_argv[1];

  if (type != kMediaStreamChanged) {
    NanThrowError("Internal Error");
    return;
  }

  if (_stream.get()) {
    webrtc::AudioTrackVector audio_list = _stream->GetAudioTracks();
    webrtc::VideoTrackVector video_list = _stream->GetVideoTracks();

    if (!audio_list.empty() || !video_list.empty()) {
      _ended = false;
    } else {
      _ended = true;

      if (!_audio_tracks.empty() || !_video_tracks.empty()) {
        onended = NanNew(_onended);
      }
    }

    if (audio_list.size() != _audio_tracks.size()) {
      std::vector<rtc::scoped_refptr<webrtc::AudioTrackInterface> >::iterator last_audio, cur_audio;

      if (audio_list.size() > _audio_tracks.size()) {
        for (cur_audio = audio_list.begin(); cur_audio != audio_list.end(); cur_audio++) {
          rtc::scoped_refptr<webrtc::AudioTrackInterface> cur_track(*cur_audio);
          std::string cur_id;
          bool found = false;

          if (cur_track.get()) {
            cur_id = cur_track->id();
          }

          if (_audio_tracks.size()) {
            for (last_audio = _audio_tracks.begin(); last_audio != _audio_tracks.end(); last_audio++) {
              rtc::scoped_refptr<webrtc::AudioTrackInterface> last_track(*last_audio);
              std::string last_id;

              if (last_track.get()) {
                last_id = last_track->id();
              }

              if (cur_id.compare(last_id) == 0) {
                found = true;
                break;
              }
            }
          }

          if (!found) {
            Local<Function> callback = NanNew(_onaddtrack);
            Local<Value> argv[] = {
              MediaStreamTrack::New(cur_track.get())
            };

            if (!callback.IsEmpty() && callback->IsFunction()) {
              callback->Call(RTCWrap::This(), 1, argv);
            }
          }
        }
      } else {
        for (cur_audio = _audio_tracks.begin(); cur_audio != _audio_tracks.end(); cur_audio++) {
          rtc::scoped_refptr<webrtc::AudioTrackInterface> cur_track(*cur_audio);
          std::string cur_id;
          bool found = false;

          if (cur_track.get()) {
            cur_id = cur_track->id();
          }

          if (audio_list.size()) {
            for (last_audio = audio_list.begin(); last_audio != audio_list.end(); last_audio++) {
              rtc::scoped_refptr<webrtc::AudioTrackInterface> last_track(*last_audio);
              std::string last_id;

              if (last_track.get()) {
                last_id = last_track->id();
              }

              if (cur_id.compare(last_id) == 0) {
                found = true;
                break;
              }
            }
          }

          if (!found) {
            Local<Function> callback = NanNew(_onremovetrack);
            Local<Value> argv[] = {
              MediaStreamTrack::New(cur_track.get())
            };

            if (!callback.IsEmpty() && callback->IsFunction()) {
              callback->Call(RTCWrap::This(), 1, argv);
            }
          }
        }
      }

      _audio_tracks = audio_list;
    }

    if (video_list.size() != _video_tracks.size()) {
      std::vector<rtc::scoped_refptr<webrtc::VideoTrackInterface> >::iterator last_video, cur_video;

      if (video_list.size() > _video_tracks.size()) {
        for (cur_video = video_list.begin(); cur_video != video_list.end(); cur_video++) {
          rtc::scoped_refptr<webrtc::VideoTrackInterface> cur_track(*cur_video);
          std::string cur_id;
          bool found = false;

          if (cur_track.get()) {
            cur_id = cur_track->id();
          }

          if (_video_tracks.size()) {
            for (last_video = _video_tracks.begin(); last_video != _video_tracks.end(); last_video++) {
              rtc::scoped_refptr<webrtc::VideoTrackInterface> last_track(*last_video);
              std::string last_id;

              if (last_track.get()) {
                last_id = last_track->id();
              }

              if (cur_id.compare(last_id) == 0) {
                found = true;
                break;
              }
            }
          }

          if (!found) {
            Local<Function> callback = NanNew(_onaddtrack);
            Local<Value> argv[] = {
              MediaStreamTrack::New(cur_track.get())
            };

            if (!callback.IsEmpty() && callback->IsFunction()) {
              callback->Call(RTCWrap::This(), 1, argv);
            }
          }
        }
      } else {
        for (cur_video = _video_tracks.begin(); cur_video != _video_tracks.end(); cur_video++) {
          rtc::scoped_refptr<webrtc::VideoTrackInterface> cur_track(*cur_video);
          std::string cur_id;
          bool found = false;

          if (cur_track.get()) {
            cur_id = cur_track->id();
          }

          if (video_list.size()) {
            for (last_video = video_list.begin(); last_video != video_list.end(); last_video++) {
              rtc::scoped_refptr<webrtc::VideoTrackInterface> last_track(*last_video);
              std::string last_id;

              if (last_track.get()) {
                last_id = last_track->id();
              }

              if (cur_id.compare(last_id) == 0) {
                found = true;
                break;
              }
            }
          }

          if (!found) {
            Local<Function> callback = NanNew(_onremovetrack);
            Local<Value> argv[] = {
              MediaStreamTrack::New(cur_track.get())
            };

            if (!callback.IsEmpty() && callback->IsFunction()) {
              callback->Call(RTCWrap::This(), 1, argv);
            }
          }
        }
      }

      _video_tracks = video_list;
    }
  }

  if (!onended.IsEmpty() && onended->IsFunction()) {
    onended->Call(RTCWrap::This(), 0, onended_argv);
  }
}
