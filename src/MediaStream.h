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

#include <v8.h>

#include "Observers.h" 
#include "EventEmitter.h"
#include "Wrap.h"

namespace WebRTC {
  class MediaStream : public RTCWrap, public EventEmitter {
   public:
    static void Init();
    static v8::Local<v8::Value> New(v8::Isolate *isolate, rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream);

    static rtc::scoped_refptr<webrtc::MediaStreamInterface> Unwrap(v8::Isolate *isolate, v8::Local<v8::Object> value);
    static rtc::scoped_refptr<webrtc::MediaStreamInterface> Unwrap(v8::Isolate *isolate, v8::Local<v8::Value> value);

   private:
    MediaStream();
    ~MediaStream() final;

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AddTrack(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Clone(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetTrackById(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetAudioTracks(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetVideoTracks(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void RemoveTrack(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void GetEnded(v8::Local<v8::String> property,
                         const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetId(v8::Local<v8::String> property,
                      const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetOnAddTrack(v8::Local<v8::String> property,
                              const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetOnRemoveTrack(v8::Local<v8::String> property,
                                 const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetOnEnded(v8::Local<v8::String> property,
                           const v8::PropertyCallbackInfo<v8::Value> &info);

    static void ReadOnly(v8::Local<v8::String> property,
                         v8::Local<v8::Value> value,
                         const v8::PropertyCallbackInfo<void> &info);

    static void SetOnAddTrack(v8::Local<v8::String> property,
                              v8::Local<v8::Value> value,
                              const v8::PropertyCallbackInfo<void> &info);

    static void SetOnRemoveTrack(v8::Local<v8::String> property,
                                 v8::Local<v8::Value> value,
                                 const v8::PropertyCallbackInfo<void> &info);

    static void SetOnEnded(v8::Local<v8::String> property,
                           v8::Local<v8::Value> value,
                           const v8::PropertyCallbackInfo<void> &info);

    void On(Event *event) final;

   protected:
    v8::Persistent<v8::Function> _onaddtrack;
    v8::Persistent<v8::Function> _onremovetrack;
    v8::Persistent<v8::Function> _onended;

    rtc::scoped_refptr<MediaStreamObserver> _observer;
    rtc::scoped_refptr<webrtc::MediaStreamInterface> _stream;

    static v8::Persistent<v8::Function> constructor;
  };
};

#endif
