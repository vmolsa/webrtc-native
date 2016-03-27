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

#ifndef WEBRTC_MEDIACAPTURER_H
#define WEBRTC_MEDIACAPTURER_H

#include "Common.h"
#include "Observers.h" 
#include "EventEmitter.h"
#include "Wrap.h"

namespace WebRTC {  
  class MediaCapturer : 
    public RTCWrap, 
    public EventEmitter, 
    public rtc::VideoSourceInterface<cricket::VideoFrame>, 
    public webrtc::AudioTrackSinkInterface 
  {
   public:
    static void Init();
      
    static v8::Local<v8::Value> New(webrtc::AudioSourceInterface *track = 0);
    static v8::Local<v8::Value> New(webrtc::VideoSourceInterface *track = 0);

   private:
    MediaCapturer();
    ~MediaCapturer() final;
    
    static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void Stop(const Nan::FunctionCallbackInfo<v8::Value> &info);

    static void GetOnData(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void SetOnData(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    
    void OnFrame(const cricket::VideoFrame &frame) final;
    void OnData(const void* audio_data, int bits_per_sample, int sample_rate, size_t number_of_channels, size_t number_of_frames) final;
    void On(Event *event) final;

   protected:
     rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> _video;
     rtc::scoped_refptr<webrtc::AudioSourceInterface> _audio;
     rtc::scoped_refptr<webrtc::MediaSourceInterface> _source;
     rtc::scoped_refptr<MediaStreamTrackObserver> _observer;
     
     Nan::Persistent<v8::Function> _ondata;
     
     static Nan::Persistent<v8::Function> constructor;
  };
};

#endif
