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

#include <iostream>

#include "mediastream.h"
#include "mediastreamtrack.h"
#include "rtcdatachannel.h"
#include "rtcpeerconnection.h"
#include "v8-promise.h"

using namespace WebRTC;
using namespace v8;

Nan::Persistent<Function> RTCPeerConnection::constructor;

RTCPeerConnection::RTCPeerConnection(const crtc::RTCPeerConnection::RTCConfiguration &config) {
  _peer = crtc::RTCPeerConnection::New(config);

  _peer->onnegotiationneeded = [=]() {
    Nan::HandleScope scope;

    const int argc = 0;
    Local<Value> argv[0];
    Local<Function> callback = Nan::New<Function>(_onnegotiationneeded);

    if (!callback.IsEmpty() && callback->IsFunction()) {
      callback->Call(Nan::Null(), argc, argv);
    }
  };

  _peer->onsignalingstatechange = [=]() {
    Nan::HandleScope scope;

    const int argc = 0;
    Local<Value> argv[0];
    Local<Function> callback = Nan::New<Function>(_onsignalingstatechange);

    if (!callback.IsEmpty() && callback->IsFunction()) {
      callback->Call(Nan::Null(), argc, argv);
    }

    if (_peer->SignalingState() == crtc::RTCPeerConnection::kSignalingClosed) {
      Nan::ObjectWrap::Unref();
      RTCPeerConnection::RemoveEvents(_peer);
    }
  };

  _peer->onicegatheringstatechange = [=]() {
    Nan::HandleScope scope;

    const int argc = 0;
    Local<Value> argv[0];
    
    
    Local<Function> callback = Nan::New<Function>(_onicegatheringstatechange);

    if (!callback.IsEmpty() && callback->IsFunction()) {
      callback->Call(Nan::Null(), argc, argv);
    }
  };

  _peer->oniceconnectionstatechange = [=]() {
    Nan::HandleScope scope;

    const int argc = 0;
    Local<Value> argv[0];
    Local<Function> callback = Nan::New<Function>(_oniceconnectionstatechange);

    if (!callback.IsEmpty() && callback->IsFunction()) {
      callback->Call(Nan::Null(), argc, argv);
    }
  };

  _peer->onicecandidatesremoved = [=]() {
    Nan::HandleScope scope;

    const int argc = 0;
    Local<Value> argv[0];
    Local<Function> callback = Nan::New<Function>(_onicecandidatesremoved);

    if (!callback.IsEmpty() && callback->IsFunction()) {
      callback->Call(Nan::Null(), argc, argv);
    }
  };

  _peer->onaddstream = [=](const crtc::Let<crtc::MediaStream> &stream) {
    Nan::HandleScope scope;

    Local<Object> obj = Nan::New<Object>();
    obj->Set(Nan::New("stream").ToLocalChecked(), MediaStream::New(stream));

    const int argc = 1;
    Local<Value> argv[] = {
      obj
    };

    Local<Function> callback = Nan::New<Function>(_onaddstream);

    if (!callback.IsEmpty() && callback->IsFunction()) {
      callback->Call(Nan::Null(), argc, argv);
    }
  };

  _peer->onremovestream = [=](const crtc::Let<crtc::MediaStream> &stream) {
    Nan::HandleScope scope;

    Local<Object> obj = Nan::New<Object>();
    obj->Set(Nan::New("stream").ToLocalChecked(), MediaStream::New(stream));

    const int argc = 1;
    Local<Value> argv[] = {
      obj
    };

    Local<Function> callback = Nan::New<Function>(_onremovestream);
    
    if (!callback.IsEmpty() && callback->IsFunction()) {
      callback->Call(Nan::Null(), argc, argv);
    }
  };

  _peer->ondatachannel = [=](const crtc::Let<crtc::RTCDataChannel> &dataChannel) {
    Nan::HandleScope scope;

    Local<Object> obj = Nan::New<Object>();
    obj->Set(Nan::New("channel").ToLocalChecked(), RTCDataChannel::New(dataChannel));

    const int argc = 1;
    Local<Value> argv[] = {
      obj
    };

    Local<Function> callback = Nan::New<Function>(_ondatachannel);

    if (!callback.IsEmpty() && callback->IsFunction()) {
      callback->Call(Nan::Null(), argc, argv);
    }
  };

  _peer->onicecandidate = [=](const crtc::RTCPeerConnection::RTCIceCandidate &iceCandidate) {
    Nan::HandleScope scope;

    Local<Object> obj = Nan::New<Object>();
    obj->Set(Nan::New("candidate").ToLocalChecked(), RTCIceCandidateToObject(iceCandidate));
    
    int argc = 1;
    Local<Value> argv[] = {
      obj      
    };
    
    Local<Function> callback = Nan::New<Function>(_onicecandidate);

    if (!callback.IsEmpty() && callback->IsFunction()) {
      callback->Call(Nan::Null(), argc, argv);
    }
  };
}

RTCPeerConnection::~RTCPeerConnection() {
  RTCPeerConnection::RemoveEvents(_peer, false);
}

void RTCPeerConnection::Init(Local<Object> exports) {
  Nan::HandleScope scope;
  
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(RTCPeerConnection::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("RTCPeerConnection").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "createDataChannel", RTCPeerConnection::CreateDataChannel);
  Nan::SetPrototypeMethod(tpl, "addIceCandidate", RTCPeerConnection::AddIceCandidate);
  Nan::SetPrototypeMethod(tpl, "addStream", RTCPeerConnection::AddStream);
  Nan::SetPrototypeMethod(tpl, "createAnswer", RTCPeerConnection::CreateAnswer);
  Nan::SetPrototypeMethod(tpl, "createOffer", RTCPeerConnection::CreateOffer);
  Nan::SetPrototypeMethod(tpl, "getLocalStreams", RTCPeerConnection::GetLocalStreams);
  Nan::SetPrototypeMethod(tpl, "getRemoteStreams", RTCPeerConnection::GetRemoteStreams);
  Nan::SetPrototypeMethod(tpl, "removeStream", RTCPeerConnection::RemoveStream);
  Nan::SetPrototypeMethod(tpl, "setConfiguration", RTCPeerConnection::SetConfiguration);
  Nan::SetPrototypeMethod(tpl, "setLocalDescription", RTCPeerConnection::SetLocalDescription);
  Nan::SetPrototypeMethod(tpl, "setRemoteDescription", RTCPeerConnection::SetRemoteDescription);
  Nan::SetPrototypeMethod(tpl, "close", RTCPeerConnection::Close);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("currentLocalDescription").ToLocalChecked(), 
                   RTCPeerConnection::CurrentLocalDescription);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("currentRemoteDescription").ToLocalChecked(), 
                   RTCPeerConnection::CurrentRemoteDescription);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("localDescription").ToLocalChecked(), 
                   RTCPeerConnection::LocalDescription);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("pendingLocalDescription").ToLocalChecked(), 
                   RTCPeerConnection::PendingLocalDescription);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("pendingRemoteDescription").ToLocalChecked(), 
                   RTCPeerConnection::PendingRemoteDescription);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("remoteDescription").ToLocalChecked(), 
                   RTCPeerConnection::RemoteDescription);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("iceConnectionState").ToLocalChecked(), 
                   RTCPeerConnection::IceConnectionState);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("iceGatheringState").ToLocalChecked(), 
                   RTCPeerConnection::IceGatheringState);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("signalingState").ToLocalChecked(), 
                   RTCPeerConnection::SignalingState);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("onnegotiationneeded").ToLocalChecked(),
                   RTCPeerConnection::onnegotiationneeded,
                   RTCPeerConnection::onnegotiationneeded);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("onsignalingstatechange").ToLocalChecked(),
                   RTCPeerConnection::onsignalingstatechange,
                   RTCPeerConnection::onsignalingstatechange);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("onicegatheringstatechange").ToLocalChecked(),
                   RTCPeerConnection::onicegatheringstatechange,
                   RTCPeerConnection::onicegatheringstatechange);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("oniceconnectionstatechange").ToLocalChecked(),
                   RTCPeerConnection::oniceconnectionstatechange,
                   RTCPeerConnection::oniceconnectionstatechange);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("onicecandidatesremoved").ToLocalChecked(),
                   RTCPeerConnection::onicecandidatesremoved,
                   RTCPeerConnection::onicecandidatesremoved);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("onaddstream").ToLocalChecked(),
                   RTCPeerConnection::onaddstream,
                   RTCPeerConnection::onaddstream);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("onremovestream").ToLocalChecked(),
                   RTCPeerConnection::onremovestream,
                   RTCPeerConnection::onremovestream);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("ondatachannel").ToLocalChecked(),
                   RTCPeerConnection::ondatachannel,
                   RTCPeerConnection::ondatachannel);

  Nan::SetAccessor(tpl->InstanceTemplate(), 
                   Nan::New("onicecandidate").ToLocalChecked(),
                   RTCPeerConnection::onicecandidate,
                   RTCPeerConnection::onicecandidate);

  constructor.Reset<Function>(tpl->GetFunction());
  exports->Set(Nan::New("RTCPeerConnection").ToLocalChecked(), tpl->GetFunction());
}

void RTCPeerConnection::Dispose() {

}

crtc::RTCPeerConnection::RTCSessionDescription RTCPeerConnection::RTCSessionDescriptionFromValue(Local<Value> value) {
  crtc::RTCPeerConnection::RTCSessionDescription rtcsdp;

  if (!value.IsEmpty() && value->IsObject()) {
    Local<Object> obj = Local<Object>::Cast(value);
    Local<Value> type = obj->Get(Nan::New("type").ToLocalChecked());
    Local<Value> sdp = obj->Get(Nan::New("sdp").ToLocalChecked());
      
    if (!type.IsEmpty() && type->IsString()) {
      if (!sdp.IsEmpty() && sdp->IsString()) {
        if (!std::strcmp(*v8::String::Utf8Value(type), "answer")) {
          rtcsdp.type = crtc::RTCPeerConnection::RTCSessionDescription::kAnswer;
          rtcsdp.sdp = std::string(*v8::String::Utf8Value(sdp));
        } else if (!std::strcmp(*v8::String::Utf8Value(type), "offer")) {
          rtcsdp.type = crtc::RTCPeerConnection::RTCSessionDescription::kOffer;
          rtcsdp.sdp = std::string(*v8::String::Utf8Value(sdp));
        } else if (!std::strcmp(*v8::String::Utf8Value(type), "pranswer")) {
          rtcsdp.type = crtc::RTCPeerConnection::RTCSessionDescription::kPranswer;
          rtcsdp.sdp = std::string(*v8::String::Utf8Value(sdp));
        } else if (!std::strcmp(*v8::String::Utf8Value(type), "rollback")) {
          rtcsdp.type = crtc::RTCPeerConnection::RTCSessionDescription::kRollback;
          rtcsdp.sdp = std::string(*v8::String::Utf8Value(sdp));
        }
      }
    }
  }

  return rtcsdp;
}

Local<Object> RTCPeerConnection::RTCSessionDescriptionToObject(const crtc::RTCPeerConnection::RTCSessionDescription &sdp) {
  Nan::EscapableHandleScope scope;
  Local<Object> obj = Nan::New<Object>();

  obj->Set(Nan::New("type").ToLocalChecked(), RTCSdpTypeToString(sdp.type));
  obj->Set(Nan::New("sdp").ToLocalChecked(), Nan::New<v8::String>(sdp.sdp).ToLocalChecked());

  return scope.Escape(obj);
}

crtc::RTCPeerConnection::RTCIceCandidate RTCPeerConnection::RTCIceCandidateFromValue(Local<Value> value) {
  crtc::RTCPeerConnection::RTCIceCandidate iceCandidate;

  if (!value.IsEmpty() && value->IsObject()) {
    Local<Object> obj = Local<Object>::Cast(value);
    Local<Value> sdpMid = obj->Get(Nan::New("sdpMid").ToLocalChecked());
    Local<Value> sdpMLineIndex = obj->Get(Nan::New("sdpMLineIndex").ToLocalChecked());
    Local<Value> candidate = obj->Get(Nan::New("candidate").ToLocalChecked());

    if (!sdpMid.IsEmpty() && sdpMid->IsString()) {
      if (!sdpMLineIndex.IsEmpty() && sdpMLineIndex->IsUint32()) {
        if (!candidate.IsEmpty() && candidate->IsString()) {
          iceCandidate.candidate = std::string(*v8::String::Utf8Value(candidate));
          iceCandidate.sdpMid = std::string(*v8::String::Utf8Value(sdpMid));
          iceCandidate.sdpMLineIndex = Nan::To<uint32_t>(sdpMLineIndex).FromJust();
        }
      }
    }
  }

  return iceCandidate;
}

Local<Object> RTCPeerConnection::RTCIceCandidateToObject(const crtc::RTCPeerConnection::RTCIceCandidate &iceCandidate) {
  Nan::EscapableHandleScope scope;
  Local<Object> obj = Nan::New<Object>();

  obj->Set(Nan::New("sdpMid").ToLocalChecked(), Nan::New<v8::String>(iceCandidate.sdpMid).ToLocalChecked());
  obj->Set(Nan::New("sdpMLineIndex").ToLocalChecked(), Nan::New(iceCandidate.sdpMLineIndex));
  obj->Set(Nan::New("candidate").ToLocalChecked(), Nan::New<v8::String>(iceCandidate.candidate).ToLocalChecked());

  return scope.Escape(obj);
}

crtc::RTCPeerConnection::RTCIceServer RTCPeerConnection::RTCIceServerFromValue(Local<Value> value) {
  crtc::RTCPeerConnection::RTCIceServer iceServer;

  if (!value.IsEmpty() && value->IsObject()) {
    Local<Object> obj = Local<Object>::Cast(value);
    Local<Value> credential = obj->Get(Nan::New("credential").ToLocalChecked());
    Local<Value> credentialType = obj->Get(Nan::New("credentialType").ToLocalChecked());
    Local<Value> username = obj->Get(Nan::New("username").ToLocalChecked());
    Local<Value> urls = obj->Get(Nan::New("urls").ToLocalChecked());

    if (!credential.IsEmpty() && credential->IsString()) {
      iceServer.credential = std::string(*v8::String::Utf8Value(credential));
    }

    if (!credentialType.IsEmpty() && credentialType->IsString()) {
      if (!std::strcmp(*v8::String::Utf8Value(credentialType), "password")) {
        iceServer.credentialType = std::string("password");
      } else if (!std::strcmp(*v8::String::Utf8Value(credentialType), "token")) {
        iceServer.credentialType = std::string("token");
      }
    }

    if (!username.IsEmpty() && username->IsString()) {
      iceServer.username = std::string(*v8::String::Utf8Value(username));
    }

    if (!urls.IsEmpty()) {
      if (urls->IsString()) {
        iceServer.urls.push_back(std::string(*v8::String::Utf8Value(urls)));
      } else if (urls->IsArray()) {
        Local<Array> list = Local<Array>::Cast(urls);

        for (uint32_t index = 0; index < list->Length(); index++) {
          Local<Value> url = list->Get(index);

          if (!url.IsEmpty() && url->IsString()) {
            iceServer.urls.push_back(std::string(*v8::String::Utf8Value(url)));
          }
        }
      }
    }
  }

  return iceServer;
}

Local<Object> RTCPeerConnection::RTCIceServerToObject(const crtc::RTCPeerConnection::RTCIceServer &server) {
  Nan::EscapableHandleScope scope;
  Local<Object> obj = Nan::New<Object>();

  if (!server.credential.empty()) {
    obj->Set(Nan::New("credential").ToLocalChecked(), Nan::New<v8::String>(server.credential).ToLocalChecked());
  }

  if (!server.credentialType.empty()) {
    obj->Set(Nan::New("credentialType").ToLocalChecked(), Nan::New<v8::String>(server.credentialType).ToLocalChecked());
  }

  if (!server.username.empty()) {
    obj->Set(Nan::New("username").ToLocalChecked(), Nan::New<v8::String>(server.username).ToLocalChecked());
  }

  if (!server.urls.empty()) {
    Local<Array> list = Nan::New<Array>();
    
    for (uint32_t index = 0; index < server.urls.size(); index++) {
      list->Set(index, Nan::New<v8::String>(server.urls[index]).ToLocalChecked());
    }

    obj->Set(Nan::New("urls").ToLocalChecked(), list);
  }

  return scope.Escape(obj);
}

crtc::RTCPeerConnection::RTCConfiguration RTCPeerConnection::RTCConfigurationFromValue(Local<Value> value) {
  crtc::RTCPeerConnection::RTCConfiguration config;

  if (!value.IsEmpty() && value->IsObject()) {
    Local<Object> obj = Local<Object>::Cast(value);
    Local<Value> iceCandidatePoolSize = obj->Get(Nan::New("iceCandidatePoolSize").ToLocalChecked());
    Local<Value> bundlePolicy = obj->Get(Nan::New("bundlePolicy").ToLocalChecked());
    Local<Value> iceServers = obj->Get(Nan::New("iceServers").ToLocalChecked());
    Local<Value> iceTransportPolicy = obj->Get(Nan::New("iceTransportPolicy").ToLocalChecked());
    Local<Value> rtcpMuxPolicy = obj->Get(Nan::New("rtcpMuxPolicy").ToLocalChecked());

    if (!iceCandidatePoolSize.IsEmpty() && iceCandidatePoolSize->IsUint32()) {
      config.iceCandidatePoolSize = static_cast<uint16_t>(Nan::To<uint32_t>(iceCandidatePoolSize).FromJust());
    }

    config.bundlePolicy = RTCBundlePolicyFromValue(bundlePolicy);    
    config.iceTransportPolicy = RTCIceTransportPolicyFromValue(iceTransportPolicy);
    config.rtcpMuxPolicy = RTCRtcpMuxPolicyFromValue(rtcpMuxPolicy);

    if (!iceServers.IsEmpty() && iceServers->IsArray()) {
      Local<Array> list = Local<Array>::Cast(iceServers);

      for (uint32_t index = 0; index < list->Length(); index++) {
        Local<Value> server = list->Get(index);

        if (!server.IsEmpty() && server->IsObject()) {
          config.iceServers.push_back(RTCIceServerFromValue(server));
        }
      }
    }
  }

  return config;
}

Local<Object> RTCPeerConnection::RTCConfigurationToObject(const crtc::RTCPeerConnection::RTCConfiguration &config) {
  Nan::EscapableHandleScope scope;
  Local<Object> obj = Nan::New<Object>();

  obj->Set(Nan::New("iceCandidatePoolSize").ToLocalChecked(), Nan::New(config.iceCandidatePoolSize));
  obj->Set(Nan::New("bundlePolicy").ToLocalChecked(), RTCBundlePolicyToString(config.bundlePolicy));
  obj->Set(Nan::New("iceTransportPolicy").ToLocalChecked(), RTCIceTransportPolicyToString(config.iceTransportPolicy));
  obj->Set(Nan::New("rtcpMuxPolicy").ToLocalChecked(), RTCRtcpMuxPolicyToString(config.rtcpMuxPolicy));

  if (!config.iceServers.empty()) {
    Local<Array> list = Nan::New<Array>();
    
    for (uint32_t index = 0; index < config.iceServers.size(); index++) {
      list->Set(index, RTCIceServerToObject(config.iceServers[index]));
    }

    obj->Set(Nan::New("iceServers").ToLocalChecked(), list);
  }

  return scope.Escape(obj);
}

crtc::RTCPeerConnection::RTCOfferOptions RTCPeerConnection::RTCOfferOptionsFromValue(Local<Value> value) {
  crtc::RTCPeerConnection::RTCOfferOptions offer;

  if (!value.IsEmpty() && value->IsObject()) {
    Local<Object> obj = Local<Object>::Cast(value);
    Local<Value> voiceActivityDetection = obj->Get(Nan::New("voiceActivityDetection").ToLocalChecked());
    Local<Value> iceRestart = obj->Get(Nan::New("iceRestart").ToLocalChecked());
    
    if (!voiceActivityDetection.IsEmpty() && voiceActivityDetection->IsBoolean()) {
      offer.voiceActivityDetection = Nan::To<bool>(voiceActivityDetection).FromJust();
    }

    if (!iceRestart.IsEmpty() && iceRestart->IsBoolean()) {
      offer.iceRestart = Nan::To<bool>(iceRestart).FromJust();
    }
  }

  return offer;
}

Local<Object> RTCPeerConnection::RTCOfferOptionsToObject(const crtc::RTCPeerConnection::RTCOfferOptions &offer) {
  Nan::EscapableHandleScope scope;
  Local<Object> obj = Nan::New<Object>();

  obj->Set(Nan::New("voiceActivityDetection").ToLocalChecked(), Nan::New(offer.voiceActivityDetection));
  obj->Set(Nan::New("iceRestart").ToLocalChecked(), Nan::New(offer.iceRestart));

  return scope.Escape(obj);
}

crtc::RTCPeerConnection::RTCAnswerOptions RTCPeerConnection::RTCAnswerOptionsFromValue(Local<Value> value) {
  crtc::RTCPeerConnection::RTCAnswerOptions answer;

  if (!value.IsEmpty() && value->IsObject()) {
    Local<Object> obj = Local<Object>::Cast(value);
    Local<Value> voiceActivityDetection = obj->Get(Nan::New("voiceActivityDetection").ToLocalChecked());

    if (!voiceActivityDetection.IsEmpty() && voiceActivityDetection->IsBoolean()) {
      answer.voiceActivityDetection = Nan::To<bool>(voiceActivityDetection).FromJust();
    }
  }

  return answer;
}

Local<Object> RTCPeerConnection::RTCAnswerOptionsToObject(const crtc::RTCPeerConnection::RTCAnswerOptions &answer) {
  Nan::EscapableHandleScope scope;
  Local<Object> obj = Nan::New<Object>();

  obj->Set(Nan::New("voiceActivityDetection").ToLocalChecked(), Nan::New(answer.voiceActivityDetection));

  return scope.Escape(obj);
}

Local<String> RTCPeerConnection::RTCSdpTypeToString(const crtc::RTCPeerConnection::RTCSessionDescription::RTCSdpType &type) {
  Nan::EscapableHandleScope scope;

  switch (type) {
    case crtc::RTCPeerConnection::RTCSessionDescription::kAnswer:
      return scope.Escape(Nan::New("answer").ToLocalChecked());
    case crtc::RTCPeerConnection::RTCSessionDescription::kOffer:
      return scope.Escape(Nan::New("offer").ToLocalChecked());
    case crtc::RTCPeerConnection::RTCSessionDescription::kPranswer:
      return scope.Escape(Nan::New("pranswer").ToLocalChecked());
    case crtc::RTCPeerConnection::RTCSessionDescription::kRollback:
      return scope.Escape(Nan::New("rollback").ToLocalChecked());
  }

  return scope.Escape(Nan::New("").ToLocalChecked());
}

Local<String> RTCPeerConnection::RTCSignalingStateToString(const crtc::RTCPeerConnection::RTCSignalingState &state) {
  Nan::EscapableHandleScope scope;

  switch (state) {
    case crtc::RTCPeerConnection::kStable:
      return scope.Escape(Nan::New("stable").ToLocalChecked());
    case crtc::RTCPeerConnection::kHaveLocalOffer:
      return scope.Escape(Nan::New("have-local-offer").ToLocalChecked());
    case crtc::RTCPeerConnection::kHaveLocalPrAnswer:
      return scope.Escape(Nan::New("have-local-pranswer").ToLocalChecked());
    case crtc::RTCPeerConnection::kHaveRemoteOffer:
      return scope.Escape(Nan::New("have-remote-offer").ToLocalChecked());
    case crtc::RTCPeerConnection::kHaveRemotePrAnswer:
      return scope.Escape(Nan::New("have-remote-pranswer").ToLocalChecked());
    case crtc::RTCPeerConnection::kSignalingClosed:
      return scope.Escape(Nan::New("closed").ToLocalChecked());
  }

  return scope.Escape(Nan::New("").ToLocalChecked());
}

Local<String> RTCPeerConnection::RTCIceGatheringStateToString(const crtc::RTCPeerConnection::RTCIceGatheringState &state) {
  Nan::EscapableHandleScope scope;

  switch (state) {
    case crtc::RTCPeerConnection::kNewGathering:
      return scope.Escape(Nan::New("new").ToLocalChecked());
    case crtc::RTCPeerConnection::kGathering:
      return scope.Escape(Nan::New("gathering").ToLocalChecked());
    case crtc::RTCPeerConnection::kComplete:
      return scope.Escape(Nan::New("complete").ToLocalChecked());
  }

  return scope.Escape(Nan::New("").ToLocalChecked());
}

Local<String> RTCPeerConnection::RTCIceConnectionStateToString(const crtc::RTCPeerConnection::RTCIceConnectionState &state) {
  Nan::EscapableHandleScope scope;

  switch (state) {
    case crtc::RTCPeerConnection::kNew:
      return scope.Escape(Nan::New("new").ToLocalChecked());
    case crtc::RTCPeerConnection::kChecking:
      return scope.Escape(Nan::New("checking").ToLocalChecked());
    case crtc::RTCPeerConnection::kConnected:
      return scope.Escape(Nan::New("connected").ToLocalChecked());
    case crtc::RTCPeerConnection::kCompleted:
      return scope.Escape(Nan::New("completed").ToLocalChecked());
    case crtc::RTCPeerConnection::kFailed:
      return scope.Escape(Nan::New("failed").ToLocalChecked());
    case crtc::RTCPeerConnection::kDisconnected:
      return scope.Escape(Nan::New("disconnected").ToLocalChecked());
    case crtc::RTCPeerConnection::kClosed:
      return scope.Escape(Nan::New("closed").ToLocalChecked());
  }

  return scope.Escape(Nan::New("").ToLocalChecked());
}

crtc::RTCPeerConnection::RTCBundlePolicy RTCPeerConnection::RTCBundlePolicyFromValue(Local<Value> value) {
  if (!value.IsEmpty() && value->IsString()) {
    if (!std::strcmp(*v8::String::Utf8Value(value), "balanced")) {
      return crtc::RTCPeerConnection::kBalanced;
    } else if (!std::strcmp(*v8::String::Utf8Value(value), "max-compat")) {
      return crtc::RTCPeerConnection::kMaxBundle;
    } else if (!std::strcmp(*v8::String::Utf8Value(value), "max-bundle")) {
      return crtc::RTCPeerConnection::kMaxCompat;
    }
  }

  return crtc::RTCPeerConnection::kMaxBundle;
}

Local<String> RTCPeerConnection::RTCBundlePolicyToString(const crtc::RTCPeerConnection::RTCBundlePolicy &policy) {
  Nan::EscapableHandleScope scope;

  switch (policy) {
    case crtc::RTCPeerConnection::kBalanced:
      return scope.Escape(Nan::New("balanced").ToLocalChecked());
    case crtc::RTCPeerConnection::kMaxBundle:
      return scope.Escape(Nan::New("max-compat").ToLocalChecked());
    case crtc::RTCPeerConnection::kMaxCompat:
      return scope.Escape(Nan::New("max-bundle").ToLocalChecked());
  }

  return scope.Escape(Nan::New("").ToLocalChecked());
}

crtc::RTCPeerConnection::RTCIceTransportPolicy RTCPeerConnection::RTCIceTransportPolicyFromValue(Local<Value> value) {
  if (!value.IsEmpty() && value->IsString()) {
    if (!std::strcmp(*v8::String::Utf8Value(value), "relay")) {
      return crtc::RTCPeerConnection::kRelay;
    } else if (!std::strcmp(*v8::String::Utf8Value(value), "public")) {
      return crtc::RTCPeerConnection::kPublic;
    } else if (!std::strcmp(*v8::String::Utf8Value(value), "all")) {
      return crtc::RTCPeerConnection::kAll;
    }
  }

  return crtc::RTCPeerConnection::kAll;
}

Local<String> RTCPeerConnection::RTCIceTransportPolicyToString(const crtc::RTCPeerConnection::RTCIceTransportPolicy &policy) {
  Nan::EscapableHandleScope scope;

  switch (policy) {
    case crtc::RTCPeerConnection::kRelay:
      return scope.Escape(Nan::New("relay").ToLocalChecked());
    case crtc::RTCPeerConnection::kPublic:
      return scope.Escape(Nan::New("public").ToLocalChecked());
    case crtc::RTCPeerConnection::kAll:
      return scope.Escape(Nan::New("all").ToLocalChecked());
  }

  return scope.Escape(Nan::New("").ToLocalChecked());
}

crtc::RTCPeerConnection::RTCRtcpMuxPolicy RTCPeerConnection::RTCRtcpMuxPolicyFromValue(Local<Value> value) {
  if (!value.IsEmpty() && value->IsString()) {
    if (!std::strcmp(*v8::String::Utf8Value(value), "negotiate")) {
      return crtc::RTCPeerConnection::kNegotiate;
    } else if (!std::strcmp(*v8::String::Utf8Value(value), "relay")) {
      return crtc::RTCPeerConnection::kRequire;
    }
  }

  return crtc::RTCPeerConnection::kRequire;
}

Local<String> RTCPeerConnection::RTCRtcpMuxPolicyToString(const crtc::RTCPeerConnection::RTCRtcpMuxPolicy &policy) {
  Nan::EscapableHandleScope scope;
  
  switch (policy) {
    case crtc::RTCPeerConnection::kNegotiate:
      return scope.Escape(Nan::New("negotiate").ToLocalChecked());
    case crtc::RTCPeerConnection::kRequire:
      return scope.Escape(Nan::New("relay").ToLocalChecked());
  }

  return scope.Escape(Nan::New("").ToLocalChecked());
}

void RTCPeerConnection::New(const Nan::FunctionCallbackInfo<Value> &info) {
  if (info.IsConstructCall()) {
    RTCPeerConnection* peer = new RTCPeerConnection(RTCConfigurationFromValue(info[0]));
    peer->Wrap(info.This());
    peer->Ref();

    return info.GetReturnValue().Set(info.This());
  } else {
    const int argc = 1;
    Local<Value> argv[argc] = {
      info[0]
    };
    
    Local<Function> cons = Nan::New(RTCPeerConnection::constructor);
    return info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
  }
}

void RTCPeerConnection::CreateDataChannel(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  if (!info[0].IsEmpty() && info[0]->IsString()) {
    crtc::Let<crtc::RTCDataChannel> dataChannel(
      self->_peer->CreateDataChannel(*v8::String::Utf8Value(info[0]), 
                                      RTCDataChannel::RTCDataChannelInitFromValue(info[1])));
    
    if (!dataChannel.IsEmpty()) {
      return info.GetReturnValue().Set(RTCDataChannel::New(dataChannel));
    }
  }
}

void RTCPeerConnection::AddIceCandidate(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  crtc::RTCPeerConnection::RTCIceCandidate iceCandidate = RTCIceCandidateFromValue(info[0]);

  auto promise = CreatePromise(info.GetIsolate(), [=](const WebRTC::V8ResolveCallback &resolve, 
                                                      const crtc::ErrorCallback &reject) 
  {
    self->_peer->AddIceCandidate(iceCandidate)->Then([=]() {
      resolve(Nan::Undefined());
    })->Catch([=](const crtc::Let<crtc::Error> &error) {
      reject(error);
    });
  }, info.Holder());

  return info.GetReturnValue().Set(promise);
}

void RTCPeerConnection::AddStream(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  
  if (!info[0].IsEmpty() && info[0]->IsObject()) {
    Local<Object> obj = Local<Object>::Cast(info[0]);
    crtc::Let<crtc::MediaStream> stream = MediaStream::New(obj);

    if (!stream.IsEmpty()) {
      self->_peer->AddStream(stream);
    }
  }
}

void RTCPeerConnection::CreateAnswer(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  crtc::RTCPeerConnection::RTCAnswerOptions options = RTCAnswerOptionsFromValue(info[0]);

  auto promise = CreatePromise(info.GetIsolate(), [=](const WebRTC::V8ResolveCallback &resolve, 
                                                      const crtc::ErrorCallback &reject) 
  {
    self->_peer->CreateAnswer(options)->Then([=](const crtc::RTCPeerConnection::RTCSessionDescription &sdp) {
      Nan::HandleScope scope;
      resolve(RTCSessionDescriptionToObject(sdp));
    })->Catch([=](const crtc::Let<crtc::Error> &error) {
      reject(error);
    });
  }, info.Holder());

  return info.GetReturnValue().Set(promise);
}

void RTCPeerConnection::CreateOffer(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  crtc::RTCPeerConnection::RTCOfferOptions options = RTCOfferOptionsFromValue(info[0]);

  auto promise = CreatePromise(info.GetIsolate(), [=](const WebRTC::V8ResolveCallback &resolve, 
                                                      const crtc::ErrorCallback &reject) 
  {
    self->_peer->CreateOffer(options)->Then([=](const crtc::RTCPeerConnection::RTCSessionDescription &sdp) {
      Nan::HandleScope scope;
      resolve(RTCSessionDescriptionToObject(sdp));
    })->Catch([=](const crtc::Let<crtc::Error> &error) {
      reject(error);
    });
  }, info.Holder());

  return info.GetReturnValue().Set(promise);
}

void RTCPeerConnection::GetLocalStreams(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  crtc::MediaStreams streams = self->_peer->GetLocalStreams();
  Local<Array> list = Nan::New<Array>();
    
  for (uint32_t index = 0; index < streams.size(); index++) {
    list->Set(index, MediaStream::New(streams[index]));
  }

  return info.GetReturnValue().Set(list);
}

void RTCPeerConnection::GetRemoteStreams(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  crtc::MediaStreams streams = self->_peer->GetRemoteStreams();
  Local<Array> list = Nan::New<Array>();
    
  for (uint32_t index = 0; index < streams.size(); index++) {
    list->Set(index, MediaStream::New(streams[index]));
  }

  return info.GetReturnValue().Set(list);
}

void RTCPeerConnection::RemoveStream(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  if (!info[0].IsEmpty() && info[0]->IsObject()) {
    Local<Object> obj = Local<Object>::Cast(info[0]);
    crtc::Let<crtc::MediaStream> stream = MediaStream::New(obj);

    if (!stream.IsEmpty()) {
      self->_peer->RemoveStream(stream);
    }
  }
}

void RTCPeerConnection::SetConfiguration(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  if (!info[0].IsEmpty() && info[0]->IsObject()) {
    self->_peer->SetConfiguration(RTCConfigurationFromValue(info[0]));
  }
}

void RTCPeerConnection::SetLocalDescription(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  crtc::RTCPeerConnection::RTCSessionDescription sdp = RTCSessionDescriptionFromValue(info[0]);

  auto promise = CreatePromise(info.GetIsolate(), [=](const WebRTC::V8ResolveCallback &resolve, 
                                                      const crtc::ErrorCallback &reject) 
  {
    self->_peer->SetLocalDescription(sdp)->Then([=]() {
      Nan::HandleScope scope;
      resolve(Nan::Undefined());
    })->Catch([=](const crtc::Let<crtc::Error> &error) {
      reject(error);
    });
  }, info.Holder());

  return info.GetReturnValue().Set(promise);
}

void RTCPeerConnection::SetRemoteDescription(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  crtc::RTCPeerConnection::RTCSessionDescription sdp = RTCSessionDescriptionFromValue(info[0]);

  auto promise = CreatePromise(info.GetIsolate(), [=](const WebRTC::V8ResolveCallback &resolve, 
                                                      const crtc::ErrorCallback &reject) 
  {
    self->_peer->SetRemoteDescription(sdp)->Then([=]() {
      Nan::HandleScope scope;
      resolve(Nan::Undefined());
    })->Catch([=](const crtc::Let<crtc::Error> &error) {
      reject(error);
    });
  }, info.Holder());

  return info.GetReturnValue().Set(promise);
}

void RTCPeerConnection::Close(const Nan::FunctionCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  self->_peer->Close();
}
  
void RTCPeerConnection::CurrentLocalDescription(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(RTCSessionDescriptionToObject(self->_peer->CurrentLocalDescription()));
}

void RTCPeerConnection::CurrentRemoteDescription(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(RTCSessionDescriptionToObject(self->_peer->CurrentRemoteDescription()));
}

void RTCPeerConnection::LocalDescription(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(RTCSessionDescriptionToObject(self->_peer->LocalDescription()));
}

void RTCPeerConnection::PendingLocalDescription(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(RTCSessionDescriptionToObject(self->_peer->PendingLocalDescription()));
}

void RTCPeerConnection::PendingRemoteDescription(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(RTCSessionDescriptionToObject(self->_peer->PendingRemoteDescription()));
}

void RTCPeerConnection::RemoteDescription(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(RTCSessionDescriptionToObject(self->_peer->RemoteDescription()));
}

void RTCPeerConnection::IceConnectionState(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(RTCIceConnectionStateToString(self->_peer->IceConnectionState()));
}

void RTCPeerConnection::IceGatheringState(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(RTCIceGatheringStateToString(self->_peer->IceGatheringState()));
}

void RTCPeerConnection::SignalingState(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(RTCSignalingStateToString(self->_peer->SignalingState()));
}

void RTCPeerConnection::onnegotiationneeded(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onnegotiationneeded));
}

void RTCPeerConnection::onnegotiationneeded(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onnegotiationneeded.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onnegotiationneeded.Reset();
  }
}

void RTCPeerConnection::onsignalingstatechange(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onsignalingstatechange));
}

void RTCPeerConnection::onsignalingstatechange(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onsignalingstatechange.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onsignalingstatechange.Reset();
  }
}

void RTCPeerConnection::onicegatheringstatechange(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onicegatheringstatechange));
}

void RTCPeerConnection::onicegatheringstatechange(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onicegatheringstatechange.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onicegatheringstatechange.Reset();
  }
}

void RTCPeerConnection::oniceconnectionstatechange(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_oniceconnectionstatechange));
}

void RTCPeerConnection::oniceconnectionstatechange(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_oniceconnectionstatechange.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_oniceconnectionstatechange.Reset();
  }
}

void RTCPeerConnection::onicecandidatesremoved(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onicecandidatesremoved));
}

void RTCPeerConnection::onicecandidatesremoved(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onicecandidatesremoved.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onicecandidatesremoved.Reset();
  }
}

void RTCPeerConnection::onaddstream(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onaddstream));
}

void RTCPeerConnection::onaddstream(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onaddstream.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onaddstream.Reset();
  }
}

void RTCPeerConnection::onremovestream(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onremovestream));
}

void RTCPeerConnection::onremovestream(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onremovestream.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onremovestream.Reset();
  }
}

void RTCPeerConnection::ondatachannel(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_ondatachannel));
}

void RTCPeerConnection::ondatachannel(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_ondatachannel.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_ondatachannel.Reset();
  }
}

void RTCPeerConnection::onicecandidate(Local<String> property, const Nan::PropertyCallbackInfo<Value> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<Function>(self->_onicecandidate));
}

void RTCPeerConnection::onicecandidate(Local<String> property, Local<Value> value, const Nan::PropertyCallbackInfo<void> &info) {
  RTCPeerConnection *self = Nan::ObjectWrap::Unwrap<RTCPeerConnection>(info.Holder());

  if (!value.IsEmpty() && value->IsFunction()) {
    self->_onicecandidate.Reset<Function>(Local<Function>::Cast(value));
  } else {
    self->_onicecandidate.Reset();
  }
}

void RTCPeerConnection::RemoveEvents(const crtc::Let<crtc::RTCPeerConnection> &peer, bool async) {
  crtc::Callback callback = [=]() {
    peer->onsignalingstatechange.Dispose();
    peer->onicegatheringstatechange.Dispose();
    peer->oniceconnectionstatechange.Dispose();
    peer->onicecandidatesremoved.Dispose();
    peer->onaddstream.Dispose();
    peer->onremovestream.Dispose();
    peer->ondatachannel.Dispose();
    peer->onicecandidate.Dispose();
  };

  if (async) {
    return crtc::SetImmediate(callback);
  }
  
  callback();
}
