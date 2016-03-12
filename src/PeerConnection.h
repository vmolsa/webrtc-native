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

#ifndef WEBRTC_PEERCONNECTION_H
#define WEBRTC_PEERCONNECTION_H

#include "Common.h"
#include "Observers.h" 
#include "EventEmitter.h"
#include "MediaConstraints.h"
#include "Wrap.h"

namespace WebRTC {
  enum PeerConnectionEvent {
    kPeerConnectionCreateClosed = 1,
    kPeerConnectionCreateOffer,
    kPeerConnectionCreateOfferError,
    kPeerConnectionCreateAnswer,
    kPeerConnectionCreateAnswerError,
    kPeerConnectionSetLocalDescription,
    kPeerConnectionSetLocalDescriptionError,
    kPeerConnectionSetRemoteDescription,
    kPeerConnectionSetRemoteDescriptionError,
    kPeerConnectionIceCandidate,
    kPeerConnectionSignalChange,
    kPeerConnectionIceChange,
    kPeerConnectionIceGathering,
    kPeerConnectionDataChannel,
    kPeerConnectionAddStream,
    kPeerConnectionRemoveStream,
    kPeerConnectionRenegotiation,
    kPeerConnectionStats
  };  
  
  class PeerConnection : public RTCWrap, public EventEmitter {
   public:
    static void Init(v8::Handle<v8::Object> exports);
    
   private:
    PeerConnection(const v8::Local<v8::Object> &configuration,
                   const v8::Local<v8::Object> &constraints);
                      
    ~PeerConnection() final;
   
    static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void CreateOffer(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void CreateAnswer(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void SetLocalDescription(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void SetRemoteDescription(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void AddIceCandidate(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void CreateDataChannel(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void AddStream(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void RemoveStream(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void GetLocalStreams(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void GetRemoteStreams(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void GetStreamById(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void GetStats(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void Close(const Nan::FunctionCallbackInfo<v8::Value> &info);
    
    static void GetSignalingState(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetIceConnectionState(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetIceGatheringState(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetOnSignalingStateChange(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetOnIceConnectionStateChange(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetOnIceCandidate(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetOnDataChannel(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetOnNegotiationNeeded(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetOnAddStream(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetOnRemoveStream(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetLocalDescription(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    static void GetRemoteDescription(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
    
    static void ReadOnly(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    static void SetOnSignalingStateChange(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    static void SetOnIceConnectionStateChange(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    static void SetOnIceCandidate(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    static void SetOnDataChannel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    static void SetOnNegotiationNeeded(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    static void SetOnAddStream(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
    static void SetOnRemoveStream(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);

    void On(Event *event) final;
    
    bool IsStable();
    
    webrtc::PeerConnectionInterface *GetSocket();
    
   protected:
    Nan::Persistent<v8::Function> _onsignalingstatechange;
    Nan::Persistent<v8::Function> _oniceconnectionstatechange;
    Nan::Persistent<v8::Function> _onicecandidate;
    Nan::Persistent<v8::Function> _ondatachannel;
    Nan::Persistent<v8::Function> _onnegotiationneeded;
    Nan::Persistent<v8::Function> _onaddstream;
    Nan::Persistent<v8::Function> _onremovestream;
    
    Nan::Persistent<v8::Function> _offerCallback;
    Nan::Persistent<v8::Function> _offerErrorCallback;
    
    Nan::Persistent<v8::Function> _answerCallback;
    Nan::Persistent<v8::Function> _answerErrorCallback;
    
    Nan::Persistent<v8::Function> _localCallback;
    Nan::Persistent<v8::Function> _localErrorCallback;
    
    Nan::Persistent<v8::Function> _remoteCallback;
    Nan::Persistent<v8::Function> _remoteErrorCallback;

    Nan::Persistent<v8::Function> _onstats;
    
    Nan::Persistent<v8::Object> _localsdp;
    Nan::Persistent<v8::Object> _remotesdp;
    
    static Nan::Persistent<v8::Function> constructor;
    
    rtc::scoped_refptr<StatsObserver> _stats;
    rtc::scoped_refptr<OfferObserver> _offer;
    rtc::scoped_refptr<AnswerObserver> _answer;
    rtc::scoped_refptr<LocalDescriptionObserver> _local;
    rtc::scoped_refptr<RemoteDescriptionObserver> _remote;
    rtc::scoped_refptr<PeerConnectionObserver> _peer;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> _socket;
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _factory;
    
    rtc::scoped_refptr<MediaConstraints> _constraints;
    webrtc::PeerConnectionInterface::RTCConfiguration _config;
  };
};

#endif