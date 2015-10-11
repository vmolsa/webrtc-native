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

#include "WebcamCapturer.h"
 
using namespace v8;
using namespace WebRTC;

WebcamCapturer::WebcamCapturer(v8::Local<v8::Object> properties) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  EventEmitter::SetReference(true);
  
  unsigned int device = 0;
  char device_name[256];
  char unique_name[256];
  
  // TODO(): get values from properties
  
  _capability.width = 1024;
  _capability.height = 720;
  _capability.rawType = webrtc::kVideoYV12;
  _capability.maxFPS = 30;
  
  _deviceInfo.reset(webrtc::VideoCaptureFactory::CreateDeviceInfo(0));
  _deviceInfo->GetDeviceName(device, device_name, 256, unique_name, 256);
  
  _module = webrtc::VideoCaptureFactory::Create(device, unique_name);
  _module->RegisterCaptureDataCallback(*(this));
  _module->StartCapture(_capability);
}

WebcamCapturer::~WebcamCapturer() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  WebcamCapturer::End();
}

void WebcamCapturer::Init(v8::Local<v8::Object> constructor) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Nan::HandleScope scope;
  
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(WebcamCapturer::New);
  constructor->Set(Nan::New("webcam").ToLocalChecked(), tpl->GetFunction());
}

void WebcamCapturer::New(const Nan::FunctionCallbackInfo<Value> &info) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  WebcamCapturer* capturer = new WebcamCapturer(Local<Object>::Cast(info[0]));
  capturer->Wrap(info.This(), "MediaSource");
  return info.GetReturnValue().Set(info.This());
}

void WebcamCapturer::End() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_module.get()) {
    _module->StopCapture();
    
    _module.release();
    _deviceInfo.release();
  } 
  
  MediaSource::End();
}

void WebcamCapturer::OnIncomingCapturedFrame(const int32_t id, const webrtc::VideoFrame &frame) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer = frame.video_frame_buffer();
  Emit(kMediaSourceFrame, buffer);
}

void WebcamCapturer::OnCaptureDelayChanged(const int32_t id, const int32_t delay) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}