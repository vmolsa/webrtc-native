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
  class RTCDataChannel : public Nan::ObjectWrap {
    public:
      static void Init(v8::Local<v8::Object> exports);

      static v8::Local<v8::Value> New(const crtc::Let<crtc::RTCDataChannel> &dataChannel);
      static crtc::Let<crtc::RTCDataChannel> New(const v8::Local<v8::Object> &dataChannel);

      static crtc::RTCPeerConnection::RTCDataChannelInit RTCDataChannelInitFromValue(v8::Local<v8::Value> value);

    private:
      explicit RTCDataChannel();
      ~RTCDataChannel();

      void Init(const crtc::Let<crtc::RTCDataChannel> &dataChannel);

      static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);

      static void Close(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void Send(const Nan::FunctionCallbackInfo<v8::Value> &info);

      static void Id(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void Label(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void BufferedAmount(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void MaxPacketLifeTime(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void MaxRetransmits(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void Negotiated(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void Ordered(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void Protocol(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void ReadyState(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);

      static void BufferedAmountLowThreshold(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void BufferedAmountLowThreshold(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void onbufferedamountlow(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onbufferedamountlow(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void onclose(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onclose(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void onerror(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onerror(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void onmessage(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onmessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void onopen(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onopen(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);

      static void RemoveEvents(const crtc::Let<crtc::RTCDataChannel> &dataChannel, bool async = true);

      Nan::Persistent<v8::Function> _onbufferedamountlow;
      Nan::Persistent<v8::Function> _onclose;
      Nan::Persistent<v8::Function> _onerror;
      Nan::Persistent<v8::Function> _onmessage;
      Nan::Persistent<v8::Function> _onopen;

      crtc::Let<crtc::RTCDataChannel> _channel;
      
      static Nan::Persistent<v8::Function> constructor;
  };
};
