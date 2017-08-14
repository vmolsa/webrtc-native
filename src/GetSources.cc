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

#include <ctype.h>
#include "Platform.h"
#include "MediaStreamTrack.h"
#include "GetSources.h"
using namespace v8;
using namespace WebRTC;

void GetSources::Init(Handle<Object> exports) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  exports->Set(Nan::New("getSources").ToLocalChecked(), Nan::New<FunctionTemplate>(GetSources::GetDevices)->GetFunction());
  exports->Set(Nan::New("getVideoSource").ToLocalChecked(), Nan::New<FunctionTemplate>(GetSources::GetVideoSource2)->GetFunction());
}

rtc::scoped_refptr<webrtc::AudioTrackInterface> GetSources::GetAudioSource(const rtc::scoped_refptr<MediaConstraints> &constraints) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  rtc::scoped_refptr<webrtc::AudioTrackInterface> track;  
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory = webrtc::CreatePeerConnectionFactory(rtc::Thread::Current(), Platform::GetWorker(), 0, 0, 0);
  
  if (factory.get()) {
    track = factory->CreateAudioTrack("audio", factory->CreateAudioSource(constraints->ToConstraints()));
  }

  return track;
}

rtc::scoped_refptr<webrtc::AudioTrackInterface> GetSources::GetAudioSource(const std::string id, const rtc::scoped_refptr<MediaConstraints> &constraints) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  return GetSources::GetAudioSource(constraints);
}

rtc::scoped_refptr<webrtc::VideoTrackInterface> GetSources::GetVideoSource(const rtc::scoped_refptr<MediaConstraints> &constraints) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  cricket::VideoCapturer* capturer = nullptr;
  rtc::scoped_refptr<webrtc::VideoTrackInterface> track;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory = webrtc::CreatePeerConnectionFactory(rtc::Thread::Current(), Platform::GetWorker(), 0, 0, 0);
  std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> video_info(webrtc::VideoCaptureFactory::CreateDeviceInfo(0));
  cricket::WebRtcVideoDeviceCapturerFactory device_factory;

  if (factory.get()) {
    if (video_info) {
      int num_devices = video_info->NumberOfDevices();
      
      for (int i = 0; i < num_devices; ++i) {
        const uint32_t kSize = 256;
        char name[kSize] = {0};
        char id[kSize] = {0};
        
        if (video_info->GetDeviceName(i, name, kSize, id, kSize) != -1) {
          capturer = device_factory.Create(cricket::Device(name, 0));
          
          if (capturer) {
            
            for(int i = 0; name[i]; i++){
              if (name[i]==' ') name[i] = '_';
              name[i] = tolower(name[i]);
            }
            track = factory->CreateVideoTrack(name, factory->CreateVideoSource(capturer, constraints->ToConstraints()));
            return track;
          }
        }
      }
    }
  }
  
  return track;
}

rtc::scoped_refptr<webrtc::VideoTrackInterface> GetSources::GetVideoSource(const std::string id_name, const rtc::scoped_refptr<MediaConstraints> &constraints) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  cricket::VideoCapturer* capturer = nullptr;
  rtc::scoped_refptr<webrtc::VideoTrackInterface> track;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory = webrtc::CreatePeerConnectionFactory(rtc::Thread::Current(), Platform::GetWorker(), 0, 0, 0);
  std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> video_info(webrtc::VideoCaptureFactory::CreateDeviceInfo(0));
  cricket::WebRtcVideoDeviceCapturerFactory device_factory;

  if (factory.get()) {
    if (video_info) {
      int num_devices = video_info->NumberOfDevices();
      
      for (int i = 0; i < num_devices; ++i) {
        const uint32_t kSize = 256;
        char name[kSize] = {0};
        char id[kSize] = {0};
        
        if (video_info->GetDeviceName(i, name, kSize, id, kSize) != -1) {
          if (id_name.empty() || id_name.compare(name) == 0) {
            capturer = device_factory.Create(cricket::Device(name, 0));
            
            if (capturer) {
              for(int i = 0; name[i]; i++){
                if (name[i]==' ') name[i] = '_';
                name[i] = tolower(name[i]);
              }
              track = factory->CreateVideoTrack(name, factory->CreateVideoSource(capturer, constraints->ToConstraints()));
              return track;
            }
          }
        }
      }
    }
  }

  return track;
}

Local<Value> GetSources::GetDevices() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::EscapableHandleScope scope;
  Local<Array> list = Nan::New<Array>();
  uint32_t index = 0;

  std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> video_info(webrtc::VideoCaptureFactory::CreateDeviceInfo(0));

  if (video_info) {
    int num_devices = video_info->NumberOfDevices();
    
    for (int i = 0; i < num_devices; ++i) {
      const uint32_t kSize = 256;
      char name[kSize] = {0};
      char id[kSize] = {0};
      
      if (video_info->GetDeviceName(i, name, kSize, id, kSize) != -1) {
        Local<Object> device = Nan::New<Object>();

        device->Set(Nan::New("kind").ToLocalChecked(), Nan::New("video").ToLocalChecked());
        device->Set(Nan::New("label").ToLocalChecked(), Nan::New(name).ToLocalChecked());
        device->Set(Nan::New("id").ToLocalChecked(), Nan::New(id).ToLocalChecked());

        list->Set(index, device);
        index++;
      }
    }
  }

  return scope.Escape(list);
}


void GetSources::GetVideoSource2(const Nan::FunctionCallbackInfo<Value> &info) {
  Nan::Utf8String  name_id(info[0]->ToString());
  
  
  rtc::scoped_refptr<MediaConstraints> constraints = MediaConstraints::New(info[1]);
  rtc::scoped_refptr<webrtc::VideoTrackInterface> track = GetSources::GetVideoSource(*name_id,constraints);
  
  
  info.GetReturnValue().Set(MediaStreamTrack::New(track));
}


void GetSources::GetDevices(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (info.Length() == 1 && info[0]->IsFunction()) {
    Local<Function> callback = Local<Function>::Cast(info[0]);

    Local<Value> argv[1] = { 
      GetSources::GetDevices()
    };

    if (!callback.IsEmpty()) {
      callback->Call(info.This(), 1, argv);
    }
  }
  
  info.GetReturnValue().SetUndefined();
}
