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

#ifndef WEBRTC_WEBCAMCAPTURER_H
#define WEBRTC_WEBCAMCAPTURER_H

#include "MediaSource.h"

namespace WebRTC {
  class WebcamCapturer : public MediaSource, public webrtc::VideoCaptureDataCallback  {
    public:
      static void Init(v8::Local<v8::Object> constructor);
      
      virtual void End();
            
    private:
      explicit WebcamCapturer(v8::Local<v8::Object> properties);
      virtual ~WebcamCapturer();
  
      static NAN_METHOD(New);

      virtual void OnIncomingCapturedFrame(const int32_t id, const webrtc::VideoFrame& frame);
      virtual void OnCaptureDelayChanged(const int32_t id, const int32_t delay);
  
    protected:
      rtc::scoped_ptr<webrtc::VideoCaptureModule::DeviceInfo> _deviceInfo;
      rtc::scoped_refptr<webrtc::VideoCaptureModule> _module;
      webrtc::VideoCaptureCapability _capability;
  };
};

#endif