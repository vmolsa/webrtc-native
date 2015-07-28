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
    
    static NAN_METHOD(New);
    static NAN_METHOD(Close);
    static NAN_METHOD(Send);

    static NAN_GETTER(GetId);
    static NAN_GETTER(GetLabel);
    static NAN_GETTER(GetOrdered);
    static NAN_GETTER(GetProtocol);
    static NAN_GETTER(GetReadyState);
    static NAN_GETTER(GetBufferedAmount);
    static NAN_GETTER(GetBinaryType);
    static NAN_GETTER(GetMaxPacketLifeType);
    static NAN_GETTER(GetMaxRetransmits);
    static NAN_GETTER(GetNegotiated);
    static NAN_GETTER(GetReliable);
    static NAN_GETTER(GetOnOpen);
    static NAN_GETTER(GetOnMessage);
    static NAN_GETTER(GetOnClose);
    static NAN_GETTER(GetOnError);

    static NAN_SETTER(ReadOnly);
    static NAN_SETTER(SetBinaryType);
    static NAN_SETTER(SetOnOpen);
    static NAN_SETTER(SetOnMessage);
    static NAN_SETTER(SetOnClose);
    static NAN_SETTER(SetOnError);

    void On(Event *event) final;
    
    webrtc::DataChannelInterface *GetSocket() const;
    
   protected:
    rtc::scoped_refptr<DataChannelObserver> _observer;
    rtc::scoped_refptr<webrtc::DataChannelInterface> _socket;
    
    v8::Persistent<v8::String> _binaryType;
    
    v8::Persistent<v8::Function> _onopen;
    v8::Persistent<v8::Function> _onmessage;
    v8::Persistent<v8::Function> _onclose;
    v8::Persistent<v8::Function> _onerror;
    
    static v8::Persistent<v8::Function> constructor;
  };
};

#endif
    