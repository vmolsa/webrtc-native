#ifndef WEBRTCPEERCONNECTION_H
#define WEBRTCPEERCONNECTION_H

#include <v8.h>
#include <node.h>

#include "Observers.h"
#include "EventEmitter.h"
#include "Wrap.h"

namespace WebRTC {
  class PeerConnection : public RTCWrap, public EventEmitter {    
   public:
    static void Init(v8::Handle<v8::Object> exports);
    
   private:
    PeerConnection(const v8::Local<v8::Object> &configuration,
                   const v8::Local<v8::Object> &constraints);
                      
    virtual ~PeerConnection();
   
    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void CreateOffer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void CreateAnswer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SetLocalDescription(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SetRemoteDescription(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AddIceCandidate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void CreateDataChannel(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);
    
    static void GetSignalingState(v8::Local<v8::String> property, 
                                  const v8::PropertyCallbackInfo<v8::Value> &info);
                               
    static void GetIceConnectionState(v8::Local<v8::String> property, 
                                      const v8::PropertyCallbackInfo<v8::Value> &info);
                                      
    static void GetIceGatheringState(v8::Local<v8::String> property, 
                                     const v8::PropertyCallbackInfo<v8::Value> &info);

    static void GetOnSignalingStateChange(v8::Local<v8::String> property, 
                                          const v8::PropertyCallbackInfo<v8::Value> &info);
                                          
    static void GetOnIceCandidate(v8::Local<v8::String> property, 
                                  const v8::PropertyCallbackInfo<v8::Value> &info);
                                  
    static void GetOnDataChannel(v8::Local<v8::String> property, 
                                 const v8::PropertyCallbackInfo<v8::Value> &info);
                                 
    static void GetOnNegotiationNeeded(v8::Local<v8::String> property, 
                                       const v8::PropertyCallbackInfo<v8::Value> &info);
                                       
    static void GetOnAddStream(v8::Local<v8::String> property, 
                               const v8::PropertyCallbackInfo<v8::Value> &info);
    
    static void ReadOnly(v8::Local<v8::String> property, 
                         v8::Local<v8::Value> value, 
                         const v8::PropertyCallbackInfo<void> &info);
                         
    static void SetOnSignalingStateChange(v8::Local<v8::String> property, 
                                          v8::Local<v8::Value> value, 
                                          const v8::PropertyCallbackInfo<void> &info);
                                          
    static void SetOnIceCandidate(v8::Local<v8::String> property, 
                                  v8::Local<v8::Value> value, 
                                  const v8::PropertyCallbackInfo<void> &info);
                                  
    static void SetOnDataChannel(v8::Local<v8::String> property, 
                                 v8::Local<v8::Value> value, 
                                 const v8::PropertyCallbackInfo<void> &info);
                                 
    static void SetOnNegotiationNeeded(v8::Local<v8::String> property, 
                                       v8::Local<v8::Value> value, 
                                       const v8::PropertyCallbackInfo<void> &info);
                                       
    static void SetOnAddStream(v8::Local<v8::String> property, 
                               v8::Local<v8::Value> value, 
                               const v8::PropertyCallbackInfo<void> &info);
                               
    virtual void On(Event *event) final;
    
    webrtc::PeerConnectionInterface *GetSocket();
    
   protected:
    bool _useDTLS;
    bool _useAudio;
    bool _useVideo;
    
    v8::Persistent<v8::Function> _onsignalingstatechange;
    v8::Persistent<v8::Function> _onicecandidate;
    v8::Persistent<v8::Function> _ondatachannel;
    v8::Persistent<v8::Function> _onnegotiationneeded;
    v8::Persistent<v8::Function> _onaddstream;
    
    v8::Persistent<v8::Function> _offerCallback;
    v8::Persistent<v8::Function> _offerErrorCallback;
    
    v8::Persistent<v8::Function> _answerCallback;
    v8::Persistent<v8::Function> _answerErrorCallback;
    
    v8::Persistent<v8::Function> _localCallback;
    v8::Persistent<v8::Function> _localErrorCallback;
    
    v8::Persistent<v8::Function> _remoteCallback;
    v8::Persistent<v8::Function> _remoteErrorCallback;
    
    static v8::Persistent<v8::Function> constructor;
    
    rtc::scoped_refptr<OfferObserver> _offer;
    rtc::scoped_refptr<AnswerObserver> _answer;
    rtc::scoped_refptr<LocalDescriptionObserver> _local;
    rtc::scoped_refptr<RemoteDescriptionObserver> _remote;
    rtc::scoped_refptr<PeerConnectionObserver> _peer;   
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> _socket;
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _factory;
    
    webrtc::PeerConnectionInterface::IceServers _servers;
  };
};

#endif