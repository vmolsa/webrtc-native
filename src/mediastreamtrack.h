/*
* The MIT License (MIT)
*
* Copyright (c) 2017 vmolsa <ville.molsa@gmail.com> (http://github.com/vmolsa)
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

#include <nan.h>
#include <crtc.h>

namespace WebRTC {
  class MediaStreamTrack : public Nan::ObjectWrap {
    public:
      static void Init(v8::Local<v8::Object> exports);
      static v8::Local<v8::Value> New(const crtc::Let<crtc::MediaStreamTrack> &mediaStreamTrack);
      static crtc::Let<crtc::MediaStreamTrack> New(const v8::Local<v8::Object> &mediaStreamTrack);

    private:
      explicit MediaStreamTrack();
      ~MediaStreamTrack();

      void Init(const crtc::Let<crtc::MediaStreamTrack> &mediaStreamTrack);

      static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);

      static void Clone(const Nan::FunctionCallbackInfo<v8::Value> &info);

      static void Enabled(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void Muted(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void Remote(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void Id(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void Kind(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void ReadyState(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);

      static void onstarted(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onstarted(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void onended(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onended(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void onmute(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onmute(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);     
      static void onunmute(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onunmute(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);     

      crtc::Let<crtc::MediaStreamTrack> _track;

      Nan::Persistent<v8::Function> _onstarted;
      Nan::Persistent<v8::Function> _onended;
      Nan::Persistent<v8::Function> _onmute;
      Nan::Persistent<v8::Function> _onunmute;

      static Nan::Persistent<v8::Function> constructor;
  };
};
