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

#include "GetUserMedia.h"
#include "GetSources.h"
#include "MediaStream.h"
#include "Observers.h"

using namespace v8;
using namespace WebRTC;

void GetUserMedia::Init(v8::Handle<v8::Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  exports->Set(String::NewFromUtf8(isolate, "getUserMedia"), FunctionTemplate::New(isolate, GetUserMedia::GetMediaStream)->GetFunction());
}

void GetUserMedia::GetMediaStream(const FunctionCallbackInfo<Value> &args) {
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream;
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  Local<Object> constraints;

  const char *error = 0;
  bool use_audio = false;
  bool use_video = false;

  std::string audioId;
  std::string videoId;
  
  if (args.Length() >= 1 && args[0]->IsObject()) {
    constraints = Local<Object>::Cast(args[0]);
  }

  if (!constraints.IsEmpty()) {
    Local<Value> audio_value = constraints->Get(String::NewFromUtf8(isolate, "audio"));
    Local<Value> video_value = constraints->Get(String::NewFromUtf8(isolate, "video"));

    if (!audio_value.IsEmpty()) {
      if (audio_value->IsObject()) {
        Local<Object> audio_obj = Local<Object>::Cast(audio_value);
        use_audio = true;

        // TODO(): Walk through arguments

      } else if (audio_value->IsTrue()) {
        use_audio = true;
      }
    }

    if (!video_value.IsEmpty()) {
      if (video_value->IsObject()) {
        Local<Object> video_obj = Local<Object>::Cast(video_value);
        use_video = true;

        // TODO(): Walk through arguments

      }
      else if (video_value->IsTrue()) {
        use_video = true;
      }
    }
  }

  if (use_audio || use_video) {
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory = webrtc::CreatePeerConnectionFactory();

    if (factory.get()) {
      stream = factory->CreateLocalMediaStream("stream");

      if (stream.get()) {
        if (use_audio) {
          rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track;

          if (audioId.empty()) {
            audio_track = GetSources::GetAudioSource();
          } else {
            audio_track = GetSources::GetAudioSource(audioId);
          }

          if (audio_track.get()) {
            stream->AddTrack(audio_track);
          } else {
            if (!audioId.empty()) {
              error = "Invalid Audio Input";
            }
          }
        }

        if (use_video) {
          rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track;

          if (videoId.empty()) {
            video_track = GetSources::GetVideoSource();
          }
          else {
            video_track = GetSources::GetVideoSource(videoId);
          }

          if (video_track.get()) {
            stream->AddTrack(video_track);
          } else {
            if (!videoId.empty()) {
              error = "Invalid Video Input";
            }
          }
        }
      }
    }
  }

  Handle<Value> argv[1];

  if (!error) {
    if (stream.get()) {
      argv[0] = MediaStream::New(isolate, stream);
    } else {
      error = "Invalid MediaStream";
    }
  }

  if (error) {
    if (!args[2].IsEmpty() && args[2]->IsFunction()) {
      Local<Function> onerror = Local<Function>::Cast(args[2]);
      argv[0] = Exception::Error(String::NewFromUtf8(isolate, error));

      onerror->Call(args.This(), 1, argv);
    } else {
      isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, error)));
    }
  } else {
    if (!args[1].IsEmpty() && args[1]->IsFunction()) {
      Local<Function> onsuccess = Local<Function>::Cast(args[1]);
      onsuccess->Call(args.This(), 1, argv);
    }
  }
}
