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
   
    static NAN_METHOD(New);
    static NAN_METHOD(CreateOffer);
    static NAN_METHOD(CreateAnswer);
    static NAN_METHOD(SetLocalDescription);
    static NAN_METHOD(SetRemoteDescription);
    static NAN_METHOD(AddIceCandidate);
    static NAN_METHOD(CreateDataChannel);
    static NAN_METHOD(AddStream);
    static NAN_METHOD(RemoveStream);
    static NAN_METHOD(GetLocalStreams);
    static NAN_METHOD(GetRemoteStreams);
    static NAN_METHOD(GetStreamById);
    static NAN_METHOD(GetStats);
    static NAN_METHOD(Close);
    
    static NAN_GETTER(GetSignalingState);
    static NAN_GETTER(GetIceConnectionState);
    static NAN_GETTER(GetIceGatheringState);
    static NAN_GETTER(GetOnSignalingStateChange);
    static NAN_GETTER(GetOnIceConnectionStateChange);
    static NAN_GETTER(GetOnIceCandidate);
    static NAN_GETTER(GetOnDataChannel);
    static NAN_GETTER(GetOnNegotiationNeeded);
    static NAN_GETTER(GetOnAddStream);
    static NAN_GETTER(GetOnRemoveStream);
    static NAN_GETTER(GetLocalDescription);
    static NAN_GETTER(GetRemoteDescription);
    
    static NAN_SETTER(ReadOnly);
    static NAN_SETTER(SetOnSignalingStateChange);
    static NAN_SETTER(SetOnIceConnectionStateChange);
    static NAN_SETTER(SetOnIceCandidate);
    static NAN_SETTER(SetOnDataChannel);
    static NAN_SETTER(SetOnNegotiationNeeded);
    static NAN_SETTER(SetOnAddStream);
    static NAN_SETTER(SetOnRemoveStream);
                               
    void On(Event *event) final;
    
    bool IsStable();
    
    webrtc::PeerConnectionInterface *GetSocket();
    
   protected:
    v8::Persistent<v8::Function> _onsignalingstatechange;
    v8::Persistent<v8::Function> _oniceconnectionstatechange;
    v8::Persistent<v8::Function> _onicecandidate;
    v8::Persistent<v8::Function> _ondatachannel;
    v8::Persistent<v8::Function> _onnegotiationneeded;
    v8::Persistent<v8::Function> _onaddstream;
    v8::Persistent<v8::Function> _onremovestream;
    
    v8::Persistent<v8::Function> _offerCallback;
    v8::Persistent<v8::Function> _offerErrorCallback;
    
    v8::Persistent<v8::Function> _answerCallback;
    v8::Persistent<v8::Function> _answerErrorCallback;
    
    v8::Persistent<v8::Function> _localCallback;
    v8::Persistent<v8::Function> _localErrorCallback;
    
    v8::Persistent<v8::Function> _remoteCallback;
    v8::Persistent<v8::Function> _remoteErrorCallback;

    v8::Persistent<v8::Function> _onstats;
    
    v8::Persistent<v8::Object> _localsdp;
    v8::Persistent<v8::Object> _remotesdp;   
    
    static v8::Persistent<v8::Function> constructor;
    
    rtc::scoped_refptr<StatsObserver> _stats;
    rtc::scoped_refptr<OfferObserver> _offer;
    rtc::scoped_refptr<AnswerObserver> _answer;
    rtc::scoped_refptr<LocalDescriptionObserver> _local;
    rtc::scoped_refptr<RemoteDescriptionObserver> _remote;
    rtc::scoped_refptr<PeerConnectionObserver> _peer;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> _socket;
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _factory;
    
    rtc::scoped_refptr<MediaConstraints> _constraints;
    webrtc::PeerConnectionInterface::IceServers _servers;
  };
};

#endif