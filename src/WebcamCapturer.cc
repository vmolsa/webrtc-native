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
  
  NanScope();
  
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(WebcamCapturer::New);
  constructor->Set(NanNew("webcam"), tpl->GetFunction());
}

NAN_METHOD(WebcamCapturer::New) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  NanScope();

  WebcamCapturer* capturer = new WebcamCapturer(Local<Object>::Cast(args[0]));
  capturer->Wrap(args.This(), "MediaSource");
  NanReturnValue(args.This());
}

void WebcamCapturer::End() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  EventEmitter::SetReference(false);
  _module->StopCapture();
}

bool WebcamCapturer::End(Local<Value> data) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  WebcamCapturer::End();
  return true;
}

bool WebcamCapturer::Write(Local<Value> data) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  return false;
}

void WebcamCapturer::OnIncomingCapturedFrame(const int32_t id, const webrtc::VideoFrame& frame) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  MediaSourceImage image;

  image.mime = std::string("image/i420");
  image.width = frame.width();
  image.height = frame.height();
  image.buffer = rtc::Buffer(frame.buffer(webrtc::kYPlane), frame.allocated_size(webrtc::kYPlane));
  
  Emit(kMediaSourceImage, image);
}

void WebcamCapturer::OnCaptureDelayChanged(const int32_t id, const int32_t delay) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}