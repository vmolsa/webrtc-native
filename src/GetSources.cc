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
#include "Platform.h"
#include "GetSources.h"

using namespace v8;
using namespace WebRTC;

void GetSources::Init(Handle<Object> exports) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  exports->Set(Nan::New("getSources").ToLocalChecked(), Nan::New<FunctionTemplate>(GetSources::GetDevices)->GetFunction());
}

rtc::scoped_refptr<webrtc::AudioTrackInterface> GetSources::GetAudioSource(const rtc::scoped_refptr<MediaConstraints> &constraints) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::AudioTrackInterface> track;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory = Platform::GetFactory();
  
  if (factory.get()) {
    rtc::scoped_refptr<webrtc::AudioSourceInterface> src = factory->CreateAudioSource(constraints->ToConstraints());
    track = factory->CreateAudioTrack("audio", src);
  }

  return track;
}

rtc::scoped_refptr<webrtc::AudioTrackInterface> GetSources::GetAudioSource(const std::string id, const rtc::scoped_refptr<MediaConstraints> &constraints) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  // TODO(): CreateAudioSource(cricket::AudioCapturer*, MediaConstraintsInterface) Missing?
  return GetSources::GetAudioSource(constraints);
}

rtc::scoped_refptr<webrtc::VideoTrackInterface> GetSources::GetVideoSource(const rtc::scoped_refptr<MediaConstraints> &constraints) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  //cricket::VideoCapturer* capturer = NULL;
  rtc::scoped_refptr<webrtc::VideoTrackInterface> track;
  /*
  cricket::DeviceManagerInterface *manager = Core::GetManager();
  
  if (manager) {
    std::vector<cricket::Device> video_devs;
    
    if (manager->GetVideoCaptureDevices(&video_devs)) {
      std::vector<cricket::Device>::iterator video_it;
      
      for (video_it = video_devs.begin(); video_it != video_devs.end(); ++video_it) {
        capturer = manager->CreateVideoCapturer(*video_it);
        
        if (capturer != NULL) {
          break;
        }
      }
    } else {
      LOG(LS_ERROR) << "Can't enumerate video devices";
    }
  } else {
    LOG(LS_ERROR) << "Can't create device manager";
  }
  
  if (capturer) {
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory = Core::CreateFactory();

    if (factory.get()) {
      track = factory->CreateVideoTrack("video", factory->CreateVideoSource(capturer, constraints->ToConstraints()));
        
      if (!track.get()) {
        LOG(LS_ERROR) << "Can't create video track";
      }
    } else {
      LOG(LS_ERROR) << "Can't get Factory";
    }
  }
  */
  
  return track;
}

rtc::scoped_refptr<webrtc::VideoTrackInterface> GetSources::GetVideoSource(const std::string id, const rtc::scoped_refptr<MediaConstraints> &constraints) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  //cricket::DeviceManagerInterface *manager = Core::GetManager();
  rtc::scoped_refptr<webrtc::VideoTrackInterface> track;
  /*
  cricket::VideoCapturer *cap = 0;

  if (manager) {
    cricket::Device video_dev;

    if (!manager->GetVideoCaptureDevice(id, &video_dev)) {
      cap = manager->CreateVideoCapturer(video_dev);
    }
  }

  if (cap) {
    //webrtc::PeerConnectionFactoryInterface *factory = Core::GetFactory();
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory = Core::CreateFactory();

    if (factory.get()) {
      track = factory->CreateVideoTrack("video", factory->CreateVideoSource(cap, constraints->ToConstraints()));
    }
  }
  */

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

/*
  cricket::DeviceManagerInterface *manager = Core::GetManager();

  if (manager) {
    std::vector<cricket::Device> audio_devs;
    std::vector<cricket::Device> video_devs;

    if (manager->GetAudioInputDevices(&audio_devs)) {
      std::vector<cricket::Device>::iterator audio_it;

      for (audio_it = audio_devs.begin(); audio_it != audio_devs.end(); audio_it++) {
        const cricket::Device &dev = *audio_it;
        Local<Object> dev_obj = Nan::New<Object>();

        dev_obj->Set(Nan::New("kind").ToLocalChecked(), Nan::New("audio").ToLocalChecked());
        dev_obj->Set(Nan::New("label").ToLocalChecked(), Nan::New(dev.name.c_str()).ToLocalChecked());
        dev_obj->Set(Nan::New("id").ToLocalChecked(), Nan::New(dev.id.c_str()).ToLocalChecked());

        list->Set(index, dev_obj);
        index++;
      }
    }

    if (manager->GetVideoCaptureDevices(&video_devs)) {
      std::vector<cricket::Device>::iterator video_it = video_devs.begin();

      for (video_it = video_devs.begin(); video_it != video_devs.end(); video_it++) {
        const cricket::Device &dev = *video_it;
        Local<Object> dev_obj = Nan::New<Object>();

        dev_obj->Set(Nan::New("kind").ToLocalChecked(), Nan::New("video").ToLocalChecked());
        dev_obj->Set(Nan::New("label").ToLocalChecked(), Nan::New(dev.name.c_str()).ToLocalChecked());
        dev_obj->Set(Nan::New("id").ToLocalChecked(), Nan::New(dev.id.c_str()).ToLocalChecked());

        list->Set(index, dev_obj);
        index++;
      }
    }
  }
*/

  return scope.Escape(list);
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
