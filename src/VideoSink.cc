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

#include "VideoSink.h"
#include "MediaStreamTrack.h"

using namespace v8;
using namespace WebRTC;

Nan::Persistent<Function> VideoSink::constructor;

void VideoSink::Init(Handle<Object> exports) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(VideoSink::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("VideoSink").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "stop", VideoSink::Stop);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("data").ToLocalChecked(), VideoSink::GetOnData, VideoSink::SetOnData);

  constructor.Reset<Function>(tpl->GetFunction());
  exports->Set(Nan::New("VideoSink").ToLocalChecked(), tpl->GetFunction());
}

VideoSink::VideoSink() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  _observer = new rtc::RefCountedObject<MediaStreamTrackObserver>(this);
}

VideoSink::~VideoSink() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_video.get()) {
    _video->RemoveSink(this);
  }
  
  if (_source.get()) {
    _source->UnregisterObserver(_observer.get());
  }
  
  _observer->RemoveListener(this);
}

void VideoSink::New(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;
  
  if (info.Length() >= 1 && info[0]->IsObject()) {
    rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track = MediaStreamTrack::Unwrap(Local<Object>::Cast(info[0]));
    
    if (track.get()) {
      if (info.IsConstructCall()) {
        VideoSink* sink = new VideoSink();
        
        if (track->kind().compare(webrtc::MediaStreamTrackInterface::kVideoKind) == 0) {
          webrtc::VideoTrackInterface *videoTrack = static_cast<webrtc::VideoTrackInterface*>(track.get());
          
          sink->_video = videoTrack->GetSource();
          sink->_source = videoTrack->GetSource();
          sink->_source->RegisterObserver(sink->_observer.get());
          sink->_video->AddOrUpdateSink(sink, rtc::VideoSinkWants());
          
          if (sink->_video->state() == webrtc::MediaSourceInterface::kLive) {
            sink->SetReference(true);
          }
        } else {
          Nan::ThrowError("Invalid MediaStreamTrack for AudioSink");
        }
        
        sink->Wrap(info.This(), "VideoSink");
        return info.GetReturnValue().Set(info.This());
      } else {
        const int argc = 1;
        Local<Value> argv[argc] = {
          Local<Object>::Cast(info[0])
        };
        
        Local<Function> instance = Nan::New(VideoSink::constructor);
        return info.GetReturnValue().Set(instance->NewInstance(argc, argv));
      }
    } else {
      Nan::ThrowError("Invalid MediaStreamTrack");
    }
  }

  info.GetReturnValue().SetUndefined();
}

void VideoSink::Stop(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  VideoSink *self = RTCWrap::Unwrap<VideoSink>(info.This(), "VideoSink");

  self->SetReference(false);

  return info.GetReturnValue().SetUndefined();
}

void VideoSink::GetOnData(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  VideoSink *self = RTCWrap::Unwrap<VideoSink>(info.Holder(), "VideoSink");
  return info.GetReturnValue().Set(Nan::New<Function>(self->_ondata));
}

void VideoSink::SetOnData(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  VideoSink *self = RTCWrap::Unwrap<VideoSink>(info.Holder(), "VideoSink");

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_ondata.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_ondata.Reset();
  }
}

void VideoSink::OnFrame(const cricket::VideoFrame &frame) {
  printf("Got Video Frame!\n");
}

void VideoSink::On(Event *event) {
  if (_source.get()) {
    if (_source->state() != webrtc::MediaSourceInterface::kLive) {
      EventEmitter::SetReference(false);
    }
  }
}