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
  class RTCPeerConnection : public Nan::ObjectWrap {
    public:
      static void Init(v8::Local<v8::Object> exports);
      static void Dispose();

      static crtc::RTCPeerConnection::RTCSessionDescription RTCSessionDescriptionFromValue(v8::Local<v8::Value> value);
      static v8::Local<v8::Object> RTCSessionDescriptionToObject(const crtc::RTCPeerConnection::RTCSessionDescription &sdp);

      static crtc::RTCPeerConnection::RTCIceCandidate RTCIceCandidateFromValue(v8::Local<v8::Value> value);
      static v8::Local<v8::Object> RTCIceCandidateToObject(const crtc::RTCPeerConnection::RTCIceCandidate &iceCandidate);

      static crtc::RTCPeerConnection::RTCIceServer RTCIceServerFromValue(v8::Local<v8::Value> value);
      static v8::Local<v8::Object> RTCIceServerToObject(const crtc::RTCPeerConnection::RTCIceServer &server);

      static crtc::RTCPeerConnection::RTCConfiguration RTCConfigurationFromValue(v8::Local<v8::Value> value);
      static v8::Local<v8::Object> RTCConfigurationToObject(const crtc::RTCPeerConnection::RTCConfiguration &config);

      static crtc::RTCPeerConnection::RTCOfferOptions RTCOfferOptionsFromValue(v8::Local<v8::Value> value);
      static v8::Local<v8::Object> RTCOfferOptionsToObject(const crtc::RTCPeerConnection::RTCOfferOptions &offer);

      static crtc::RTCPeerConnection::RTCAnswerOptions RTCAnswerOptionsFromValue(v8::Local<v8::Value> value);
      static v8::Local<v8::Object> RTCAnswerOptionsToObject(const crtc::RTCPeerConnection::RTCAnswerOptions &answer);

      static v8::Local<v8::String> RTCSdpTypeToString(const crtc::RTCPeerConnection::RTCSessionDescription::RTCSdpType &type);
      static v8::Local<v8::String> RTCSignalingStateToString(const crtc::RTCPeerConnection::RTCSignalingState &state);
      static v8::Local<v8::String> RTCIceGatheringStateToString(const crtc::RTCPeerConnection::RTCIceGatheringState &state);
      static v8::Local<v8::String> RTCIceConnectionStateToString(const crtc::RTCPeerConnection::RTCIceConnectionState &state);

      static crtc::RTCPeerConnection::RTCBundlePolicy RTCBundlePolicyFromValue(v8::Local<v8::Value> value);
      static v8::Local<v8::String> RTCBundlePolicyToString(const crtc::RTCPeerConnection::RTCBundlePolicy &policy);
      static crtc::RTCPeerConnection::RTCIceTransportPolicy RTCIceTransportPolicyFromValue(v8::Local<v8::Value> value);
      static v8::Local<v8::String> RTCIceTransportPolicyToString(const crtc::RTCPeerConnection::RTCIceTransportPolicy &policy);
      static crtc::RTCPeerConnection::RTCRtcpMuxPolicy RTCRtcpMuxPolicyFromValue(v8::Local<v8::Value> value);
      static v8::Local<v8::String> RTCRtcpMuxPolicyToString(const crtc::RTCPeerConnection::RTCRtcpMuxPolicy &policy);

    private:
      explicit RTCPeerConnection(const crtc::RTCPeerConnection::RTCConfiguration &config);
      ~RTCPeerConnection();

      static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);

      static void CreateDataChannel(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void AddIceCandidate(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void AddStream(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void CreateAnswer(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void CreateOffer(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void GetLocalStreams(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void GetRemoteStreams(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void RemoveStream(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void SetConfiguration(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void SetLocalDescription(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void SetRemoteDescription(const Nan::FunctionCallbackInfo<v8::Value> &info);
      static void Close(const Nan::FunctionCallbackInfo<v8::Value> &info);
  
      static void CurrentLocalDescription(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void CurrentRemoteDescription(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void LocalDescription(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void PendingLocalDescription(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void PendingRemoteDescription(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void RemoteDescription(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void IceConnectionState(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void IceGatheringState(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void SignalingState(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);

      static void onnegotiationneeded(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onnegotiationneeded(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void onsignalingstatechange(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onsignalingstatechange(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void onicegatheringstatechange(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onicegatheringstatechange(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void oniceconnectionstatechange(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void oniceconnectionstatechange(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void onicecandidatesremoved(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onicecandidatesremoved(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void onaddstream(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onaddstream(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void onremovestream(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onremovestream(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void ondatachannel(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void ondatachannel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      static void onicecandidate(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
      static void onicecandidate(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
      
      static void RemoveEvents(const crtc::Let<crtc::RTCPeerConnection> &peer, bool async = true);

      Nan::Persistent<v8::Function> _onnegotiationneeded;
      Nan::Persistent<v8::Function> _onsignalingstatechange;
      Nan::Persistent<v8::Function> _onicegatheringstatechange;
      Nan::Persistent<v8::Function> _oniceconnectionstatechange;
      Nan::Persistent<v8::Function> _onicecandidatesremoved;
      Nan::Persistent<v8::Function> _onaddstream;
      Nan::Persistent<v8::Function> _onremovestream;
      Nan::Persistent<v8::Function> _ondatachannel;
      Nan::Persistent<v8::Function> _onicecandidate;
      
      crtc::Let<crtc::RTCPeerConnection> _peer;

      static Nan::Persistent<v8::Function> constructor;
  };
};
