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

#ifndef WEBRTC_MEDIASTREAMTRACK_H
#define WEBRTC_MEDIASTREAMTRACK_H

#include <v8.h>

#include "Observers.h" 
#include "EventEmitter.h"
#include "Wrap.h"

namespace WebRTC {
  class MediaStreamTrack : public RTCWrap, public EventEmitter {
   public:
    static void Init();

    static v8::Local<v8::Value> New(v8::Isolate *isolate, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> mediaStreamTrack);

    static rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> Unwrap(v8::Isolate *isolate, v8::Local<v8::Object> value);
    static rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> Unwrap(v8::Isolate *isolate, v8::Local<v8::Value> value);

   private:
    MediaStreamTrack();
    ~MediaStreamTrack() final;

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetConstraints(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ApplyConstraints(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetSettings(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetCapabilities(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Clone(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Stop(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void GetEnabled(v8::Local<v8::String> property,
                           const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetId(v8::Local<v8::String> property,
                      const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetKind(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetLabel(v8::Local<v8::String> property,
                         const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetMuted(v8::Local<v8::String> property,
                         const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetReadOnly(v8::Local<v8::String> property,
                            const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetReadyState(v8::Local<v8::String> property,
                              const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetRemote(v8::Local<v8::String> property,
                          const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetOnStarted(v8::Local<v8::String> property,
                             const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetOnMute(v8::Local<v8::String> property,
                          const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetOnUnMute(v8::Local<v8::String> property,
                            const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetOnOverConstrained(v8::Local<v8::String> property,
                                     const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetOnEnded(v8::Local<v8::String> property,
                           const v8::PropertyCallbackInfo<v8::Value> &info);

    static void ReadOnly(v8::Local<v8::String> property,
                         v8::Local<v8::Value> value,
                         const v8::PropertyCallbackInfo<void> &info);

    static void SetEnabled(v8::Local<v8::String> property,
                           v8::Local<v8::Value> value,
                           const v8::PropertyCallbackInfo<void> &info);

    static void SetOnStarted(v8::Local<v8::String> property,
                             v8::Local<v8::Value> value,
                             const v8::PropertyCallbackInfo<void> &info);

    static void SetOnMute(v8::Local<v8::String> property,
                          v8::Local<v8::Value> value,
                          const v8::PropertyCallbackInfo<void> &info);

    static void SetOnUnMute(v8::Local<v8::String> property,
                            v8::Local<v8::Value> value,
                            const v8::PropertyCallbackInfo<void> &info);

    static void SetOnOverConstrained(v8::Local<v8::String> property,
                                     v8::Local<v8::Value> value,
                                     const v8::PropertyCallbackInfo<void> &info);

    static void SetOnEnded(v8::Local<v8::String> property,
                           v8::Local<v8::Value> value,
                           const v8::PropertyCallbackInfo<void> &info);

    void On(Event *event) final;

   protected:
    rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> _track;
    rtc::scoped_refptr<MediaStreamTrackObserver> _observer;

    v8::Persistent<v8::Function> _onstarted;
    v8::Persistent<v8::Function> _onmute;
    v8::Persistent<v8::Function> _onunmute;
    v8::Persistent<v8::Function> _onoverconstrained;
    v8::Persistent<v8::Function> _onended;

    static v8::Persistent<v8::Function> constructor;
  };
};

#endif