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

#ifndef WEBRTC_MEDIASTREAM_H
#define WEBRTC_MEDIASTREAM_H

#include "Common.h"
#include "Observers.h" 
#include "EventEmitter.h"
#include "Wrap.h"

namespace WebRTC {
  enum MediaStreamEvent {
    kMediaStreamChanged
  };
  
  class MediaStream : public RTCWrap, public EventEmitter {
   public:
    static void Init();    
    static v8::Local<v8::Value> New(rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream);

    static rtc::scoped_refptr<webrtc::MediaStreamInterface> Unwrap(v8::Local<v8::Object> value);
    static rtc::scoped_refptr<webrtc::MediaStreamInterface> Unwrap(v8::Local<v8::Value> value);

   private:
    MediaStream();
    ~MediaStream() final;
    
    static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void AddTrack(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void Clone(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void GetTrackById(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void GetAudioTracks(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void GetVideoTracks(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void GetTracks(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void RemoveTrack(const Nan::FunctionCallbackInfo<v8::Value> &info);
    
    static void GetActive(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetEnded(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetId(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetOnAddTrack(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetOnRemoveTrack(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);

    static void ReadOnly(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    static void SetOnAddTrack(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    static void SetOnRemoveTrack(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);

    void CheckState();
    void On(Event *event) final;

   protected:
    bool _active;
    bool _ended;
    
    Nan::Persistent<v8::Function> _onaddtrack;
    Nan::Persistent<v8::Function> _onremovetrack;

    rtc::scoped_refptr<MediaStreamObserver> _observer;
    rtc::scoped_refptr<webrtc::MediaStreamInterface> _stream;

    webrtc::AudioTrackVector _audio_tracks;
    webrtc::VideoTrackVector _video_tracks;

    static Nan::Persistent<v8::Function> constructor;
  };
};

#endif
