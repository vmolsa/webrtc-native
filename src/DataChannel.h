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

#ifndef WEBRTC_DATACHANNEL_H
#define WEBRTC_DATACHANNEL_H

#include "Common.h"
#include "Observers.h"
#include "EventEmitter.h"
#include "Wrap.h"
#include "ArrayBuffer.h"

namespace WebRTC {
  enum DataChannelEvent {
    kDataChannelStateChange,
    kDataChannelBinary,
    kDataChannelData,
  };  
  
  class DataChannel : public RTCWrap, public EventEmitter {    
   public:    
    static void Init();
    static v8::Local<v8::Value> New(rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel);
    
   private:
    DataChannel();
    ~DataChannel() final;
    
    static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void Close(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void Send(const Nan::FunctionCallbackInfo<v8::Value> &info);

    static void GetId(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetLabel(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetOrdered(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetProtocol(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetReadyState(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetBufferedAmount(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetBinaryType(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetMaxPacketLifeType(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetMaxRetransmits(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetNegotiated(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetReliable(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetOnOpen(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetOnMessage(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetOnClose(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetOnError(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);

    static void ReadOnly(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    static void SetBinaryType(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    static void SetOnOpen(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    static void SetOnMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    static void SetOnClose(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    static void SetOnError(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);

    void On(Event *event) final;
    
    webrtc::DataChannelInterface *GetSocket() const;
    
   protected:
    rtc::scoped_refptr<DataChannelObserver> _observer;
    rtc::scoped_refptr<webrtc::DataChannelInterface> _socket;
    
    Nan::Persistent<v8::String> _binaryType;
    
    Nan::Persistent<v8::Function> _onopen;
    Nan::Persistent<v8::Function> _onmessage;
    Nan::Persistent<v8::Function> _onclose;
    Nan::Persistent<v8::Function> _onerror;
    
    static Nan::Persistent<v8::Function> constructor;
  };
};

#endif
    