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

#include "Platform.h"
#include "GetUserMedia.h"
#include "GetSources.h"
#include "MediaStream.h"
#include "MediaConstraints.h"

using namespace v8;
using namespace WebRTC;

void GetUserMedia::Init(Handle<Object> exports) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  exports->Set(Nan::New("getUserMedia").ToLocalChecked(), Nan::New<FunctionTemplate>(GetUserMedia::GetMediaStream)->GetFunction());
}

void GetUserMedia::GetMediaStream(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream;
  rtc::scoped_refptr<MediaConstraints> constraints = MediaConstraints::New(info[0]);
  const char *error = 0;
  bool have_source = false;

  std::string audioId = constraints->AudioId();
  std::string videoId = constraints->VideoId();

  if (constraints->UseAudio() || constraints->UseVideo()) {
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory = webrtc::CreatePeerConnectionFactory(rtc::Thread::Current(), Platform::GetWorker(), 0, 0, 0);
    
    if (factory.get()) {
      stream = factory->CreateLocalMediaStream("stream");

      if (stream.get()) {
        if (constraints->UseAudio()) {
          rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track;

          if (audioId.empty()) {
            audio_track = GetSources::GetAudioSource(constraints);
          } else {
            audio_track = GetSources::GetAudioSource(audioId, constraints);
          }

          if (audio_track.get()) {
            if (!stream->AddTrack(audio_track)) {
              error = "Invalid Audio Input";
            } else {
              have_source = true;
            }
          } else {
            if (!audioId.empty()) {
              error = "Invalid Audio Input";
            }
          }
        } 
        
        if (constraints->UseVideo()) {
          rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track;

          if (videoId.empty()) {
            video_track = GetSources::GetVideoSource(constraints);
          } else {
            video_track = GetSources::GetVideoSource(videoId, constraints);
          }

          if (video_track.get()) {
            if (!stream->AddTrack(video_track)) {
              error = "Invalid Video Input";
            } else {
              have_source = true;
            }
          } else {
            if (!videoId.empty()) {
              error = "Invalid Video Input";
            }
          }
        }
      } else {
        error = "Internal Error";
      }
    }
  }
  
  if (!have_source) {
    error = "No available inputs";
  }

  Handle<Value> argv[1];

  if (!error) {   
    if (stream.get()) {
      argv[0] = MediaStream::New(stream);
    } else {
      error = "Invalid MediaStream";
    }
  }

  if (error) {
    if (!info[2].IsEmpty() && info[2]->IsFunction()) {
      Local<Function> onerror = Local<Function>::Cast(info[2]);
      argv[0] = Nan::Error(error);

      onerror->Call(info.This(), 1, argv);
    } else {
      Nan::ThrowError(error);
    }
  } else {
    if (!info[1].IsEmpty() && info[1]->IsFunction()) {
      Local<Function> onsuccess = Local<Function>::Cast(info[1]);
      onsuccess->Call(info.This(), 1, argv);
    }
  }
  
  info.GetReturnValue().SetUndefined();
}
