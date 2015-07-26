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

#include "MediaStreamCapturer.h"
#include "MediaStream.h"
 
using namespace v8;
using namespace WebRTC;

MediaStreamCapturer::MediaStreamCapturer(v8::Local<v8::Object> properties) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream;
  
  if (!properties.IsEmpty()) {
    mediaStream = MediaStream::Unwrap(properties->Get(NanNew("stream")));
  }
  
  if (mediaStream.get()) {
    _audioTracks = mediaStream->GetAudioTracks();
    _videoTracks = mediaStream->GetVideoTracks();
  }
  
  MediaStreamCapturer::Start();
}

MediaStreamCapturer::~MediaStreamCapturer() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaStreamCapturer::End();
}

void MediaStreamCapturer::Init(v8::Local<v8::Object> constructor) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  NanScope();
  
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(MediaStreamCapturer::New);
  constructor->Set(NanNew("mediastream"), tpl->GetFunction());
}

NAN_METHOD(MediaStreamCapturer::New) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  NanScope();

  MediaStreamCapturer* capturer = new MediaStreamCapturer(Local<Object>::Cast(args[0]));
  capturer->Wrap(args.This(), "MediaSource");
  NanReturnValue(args.This());
}

void MediaStreamCapturer::Start() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  std::vector<rtc::scoped_refptr<webrtc::AudioTrackInterface> >::iterator audio;
  std::vector<rtc::scoped_refptr<webrtc::VideoTrackInterface> >::iterator video;
  bool found = false;
  
  for (audio = _audioTracks.begin(); audio != _audioTracks.end(); audio++) {
    rtc::scoped_refptr<webrtc::AudioTrackInterface> track(*audio);

    if (track.get()) {
      found = true;
      track->set_enabled(true);
      track->AddSink(this);
    }
  }
  
  for (video = _videoTracks.begin(); video != _videoTracks.end(); video++) {
    rtc::scoped_refptr<webrtc::VideoTrackInterface> track(*video);

    if (track.get()) {
      found = true;
      track->set_enabled(true);
      track->AddRenderer(this);
    }
  }
  
  if (found) {
    EventEmitter::SetReference(true);
  }
}

void MediaStreamCapturer::End() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
 
  std::vector<rtc::scoped_refptr<webrtc::AudioTrackInterface> >::iterator audio;
  std::vector<rtc::scoped_refptr<webrtc::VideoTrackInterface> >::iterator video;
  
  for (audio = _audioTracks.begin(); audio != _audioTracks.end(); audio++) {
    rtc::scoped_refptr<webrtc::AudioTrackInterface> track(*audio);

    if (track.get()) {
      track->RemoveSink(this);
    }
  }
  
  for (video = _videoTracks.begin(); video != _videoTracks.end(); video++) {
    rtc::scoped_refptr<webrtc::VideoTrackInterface> track(*video);

    if (track.get()) {
      track->RemoveRenderer(this);
    }
  }
  
  MediaSource::End();
}

void MediaStreamCapturer::RenderFrame(const cricket::VideoFrame* frame) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer = frame->GetVideoFrameBuffer();  
  Emit(kMediaSourceFrame, buffer);
}

void MediaStreamCapturer::OnData(const void* data, int bits, int rate, int channels, int frames) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  //Emit(kMediaSourceAudio, audio);
}