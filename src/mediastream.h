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
  class MediaStream : public Nan::ObjectWrap {
    public:
      static void Init(v8::Local<v8::Object> exports);

      static v8::Local<v8::Value> New(const crtc::Let<crtc::MediaStream> &mediaStream);
      static crtc::Let<crtc::MediaStream> New(const v8::Local<v8::Object> &mediaStream);

    private:
      explicit MediaStream();
      ~MediaStream();

      void Init(const crtc::Let<crtc::MediaStream> &mediaStream);

      static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);

      static void Clone(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void AddTrack(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void RemoveTrack(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void GetTrackById(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void GetAudioTracks(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void GetVideoTracks(const Nan::FunctionCallbackInfo<v8::Value> &info);

      static void Id(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);

      static void onaddtrack(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onaddtrack(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void onremovetrack(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onremovetrack(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);

      crtc::Let<crtc::MediaStream> _stream;

      Nan::Persistent<v8::Function> _onaddtrack;
      Nan::Persistent<v8::Function> _onremovetrack;

      static Nan::Persistent<v8::Function> constructor;
  };
};
