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

#include "Core.h"
#include "GetSources.h"

using namespace v8;
using namespace WebRTC;

void GetSources::Init(Handle<Object> exports) {
  LOG(LS_INFO) << __FUNCTION__;
  
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  exports->Set(String::NewFromUtf8(isolate, "getSources"), FunctionTemplate::New(isolate, GetSources::GetDevices)->GetFunction());
}

rtc::scoped_refptr<webrtc::AudioTrackInterface> GetSources::GetAudioSource(const rtc::scoped_refptr<MediaConstraints> &constraints) {
  LOG(LS_INFO) << __FUNCTION__;
  
  webrtc::PeerConnectionFactoryInterface *factory = Core::GetFactory();
  rtc::scoped_refptr<webrtc::AudioTrackInterface> track;

  if (factory) {
    rtc::scoped_refptr<webrtc::AudioSourceInterface> src = factory->CreateAudioSource(constraints->ToConstraints());
    track = factory->CreateAudioTrack("audio", src);
  }

  return track;
}

rtc::scoped_refptr<webrtc::AudioTrackInterface> GetSources::GetAudioSource(const std::string id, const rtc::scoped_refptr<MediaConstraints> &constraints) {
  LOG(LS_INFO) << __FUNCTION__;
  // TODO(): CreateAudioSource(cricket::AudioCapturer*, MediaConstraintsInterface) Missing?
  return GetSources::GetAudioSource(constraints);
}

rtc::scoped_refptr<webrtc::VideoTrackInterface> GetSources::GetVideoSource(const rtc::scoped_refptr<MediaConstraints> &constraints) {
  LOG(LS_INFO) << __FUNCTION__;
  
  cricket::DeviceManagerInterface *manager = Core::GetManager();
  rtc::scoped_refptr<webrtc::VideoTrackInterface> track;
  cricket::VideoCapturer *cap = 0;

  if (manager) {
    std::vector<cricket::Device> video_devs;

    if (manager->GetVideoCaptureDevices(&video_devs)) {
      std::vector<cricket::Device>::iterator video_it = video_devs.begin();
      
      for (video_it = video_devs.begin(); !cap && video_it != video_devs.end(); video_it++) {
        cap = manager->CreateVideoCapturer(*video_it);
      }
    }
  }

  if (cap) {
    webrtc::PeerConnectionFactoryInterface *factory = Core::GetFactory();

    if (factory) {
      rtc::scoped_refptr<webrtc::VideoSourceInterface> src = factory->CreateVideoSource(cap, constraints->ToConstraints());
      track = factory->CreateVideoTrack("video", src);
    }
  }

  return track;
}

rtc::scoped_refptr<webrtc::VideoTrackInterface> GetSources::GetVideoSource(const std::string id, const rtc::scoped_refptr<MediaConstraints> &constraints) {
  LOG(LS_INFO) << __FUNCTION__;
  
  cricket::DeviceManagerInterface *manager = Core::GetManager();
  rtc::scoped_refptr<webrtc::VideoTrackInterface> track;
  cricket::VideoCapturer *cap = 0;

  if (manager) {
    cricket::Device video_dev;

    if (!manager->GetVideoCaptureDevice(id, &video_dev)) {
      cap = manager->CreateVideoCapturer(video_dev);
    }
  }

  if (cap) {
    webrtc::PeerConnectionFactoryInterface *factory = Core::GetFactory();

    if (factory) {
      rtc::scoped_refptr<webrtc::VideoSourceInterface> src = factory->CreateVideoSource(cap, constraints->ToConstraints());
      track = factory->CreateVideoTrack("video", src);
    }
  }

  return track;
}

Local<Value> GetSources::GetDevices(Isolate *isolate) {
  LOG(LS_INFO) << __FUNCTION__;
  
  EscapableHandleScope scope(isolate);
  Local<Array> list = Array::New(isolate);
  uint32_t index = 0;

  cricket::DeviceManagerInterface *manager = Core::GetManager();

  if (manager) {
    std::vector<cricket::Device> audio_devs;
    std::vector<cricket::Device> video_devs;

    if (manager->GetAudioInputDevices(&audio_devs)) {
      std::vector<cricket::Device>::iterator audio_it;

      for (audio_it = audio_devs.begin(); audio_it != audio_devs.end(); audio_it++) {
        const cricket::Device &dev = *audio_it;
        Local<Object> dev_obj = Object::New(isolate);

        dev_obj->Set(String::NewFromUtf8(isolate, "kind"), String::NewFromUtf8(isolate, "audio"));
        dev_obj->Set(String::NewFromUtf8(isolate, "label"), String::NewFromUtf8(isolate, dev.name.c_str()));
        dev_obj->Set(String::NewFromUtf8(isolate, "id"), String::NewFromUtf8(isolate, dev.id.c_str()));

        list->Set(index, dev_obj);
        index++;
      }
    }

    if (manager->GetVideoCaptureDevices(&video_devs)) {
      std::vector<cricket::Device>::iterator video_it = video_devs.begin();

      for (video_it = video_devs.begin(); video_it != video_devs.end(); video_it++) {
        const cricket::Device &dev = *video_it;
        Local<Object> dev_obj = Object::New(isolate);

        dev_obj->Set(String::NewFromUtf8(isolate, "kind"), String::NewFromUtf8(isolate, "video"));
        dev_obj->Set(String::NewFromUtf8(isolate, "label"), String::NewFromUtf8(isolate, dev.name.c_str()));
        dev_obj->Set(String::NewFromUtf8(isolate, "id"), String::NewFromUtf8(isolate, dev.id.c_str()));

        list->Set(index, dev_obj);
        index++;
      }
    }
  }

  return scope.Escape(list);
}

void GetSources::GetDevices(const FunctionCallbackInfo<Value>& args) {
  LOG(LS_INFO) << __FUNCTION__;
  
  Isolate *isolate = args.GetIsolate();
  HandleScope scope(isolate);

  if (args.Length() == 1 && args[0]->IsFunction()) {
    Local<Function> callback = Local<Function>::Cast(args[0]);

    Local<Value> argv[1] = { 
      GetSources::GetDevices(isolate)
    };

    if (!callback.IsEmpty()) {
      callback->Call(args.This(), 1, argv);
    }
  }
}
