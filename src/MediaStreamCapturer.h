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

#ifndef WEBRTC_MEDIASTREAMCAPTURER_H
#define WEBRTC_MEDIASTREAMCAPTURER_H

#include "MediaSource.h"
#include "talk/app/webrtc/mediastreaminterface.h"

namespace WebRTC { 
  class MediaStreamCapturer : public MediaSource, public webrtc::VideoRendererInterface, public webrtc::AudioTrackSinkInterface  {
    public:
      static void Init(v8::Local<v8::Object> constructor);
      
      void End();
            
    private:
      explicit MediaStreamCapturer(v8::Local<v8::Object> properties);
      virtual ~MediaStreamCapturer();
  
      static NAN_METHOD(New);
      
      void Start();
      virtual bool End(v8::Local<v8::Value> data);
      virtual bool Write(v8::Local<v8::Value> data);
     
      virtual void RenderFrame(const cricket::VideoFrame* frame);
      virtual void OnData(const void* data, int bits, int rate, int channels, int frames);
  
    protected:
      webrtc::AudioTrackVector _audioTracks;
      webrtc::VideoTrackVector _videoTracks;
  };
};

#endif