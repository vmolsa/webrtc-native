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

#ifndef WEBRTC_MEDIASOURCE_H
#define WEBRTC_MEDIASOURCE_H

#include <nan.h>
#include "Thread.h"
#include "MediaStream.h"
#include "MediaStreamTrack.h"

#include "talk/media/base/videoframe.h"
#include "webrtc/video_frame.h"
#include "webrtc/base/buffer.h"
#include "webrtc/modules/video_capture/include/video_capture.h"
#include "webrtc/modules/video_capture/include/video_capture_factory.h"
#include "webrtc/modules/utility/interface/process_thread.h"

namespace WebRTC {
  class WebcamCapturer : public webrtc::VideoCaptureDataCallback, public rtc::RefCountInterface {
      friend class rtc::RefCountedObject<WebcamCapturer>;
    public:
      static rtc::scoped_refptr<WebcamCapturer> New(Thread *worker = 0);
      
      void End();
    private:
      explicit WebcamCapturer(Thread *worker = 0);
      virtual ~WebcamCapturer();
     
      virtual void OnIncomingCapturedFrame(const int32_t id, const webrtc::VideoFrame& frame);
      virtual void OnCaptureDelayChanged(const int32_t id, const int32_t delay);
      
    protected:
      rtc::scoped_ptr<webrtc::VideoCaptureModule::DeviceInfo> _deviceInfo;
      rtc::scoped_refptr<webrtc::VideoCaptureModule> _module;
      Thread* _worker;
  };
  
  class VideoRenderer : public webrtc::VideoRendererInterface, public rtc::RefCountInterface {
      friend class rtc::RefCountedObject<VideoRenderer>;
    public:
      static rtc::scoped_refptr<VideoRenderer> New(webrtc::MediaStreamTrackInterface *track = 0, Thread *worker = 0);
      
      void End();
    
    private:
      explicit VideoRenderer(webrtc::MediaStreamTrackInterface *track = 0, Thread *worker = 0);
      virtual ~VideoRenderer();
     
      virtual void RenderFrame(const cricket::VideoFrame* frame);
            
    protected:
      rtc::scoped_refptr<webrtc::VideoTrackInterface> _track;
      Thread* _worker;
  };
  
  class MediaSource : public RTCWrap, public EventEmitter { 
    public:
      static void Init(v8::Handle<v8::Object> exports);
      
    private:
      explicit MediaSource(const std::string &fmt, v8::Local<v8::Function> callback);
      MediaSource(const std::string &fmt, rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream);
      MediaSource(const std::string &fmt, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> mediaStreamTrack);
      ~MediaSource() final;
      
      Thread *Format2Worker(const std::string &fmt);
      
      static NAN_METHOD(New);
      static NAN_METHOD(Write);
      static NAN_METHOD(End);

      static NAN_GETTER(OnData);
      static NAN_GETTER(OnError);
      
      static NAN_SETTER(ReadOnly);
      static NAN_SETTER(OnData);
      static NAN_SETTER(OnError);
      
      void On(Event *event) final;
      
    protected:   
      v8::Persistent<v8::Function> _callback;
      v8::Persistent<v8::Function> _ondata;
      v8::Persistent<v8::Function> _onerror;
      
      rtc::scoped_refptr<VideoRenderer> _renderer;
      rtc::scoped_refptr<WebcamCapturer> _capturer;
      
      static v8::Persistent<v8::Function> constructor;
  };
};

#endif