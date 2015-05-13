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

#include "PeerConnectionTest.h"
#include "talk/app/webrtc/test/fakedtlsidentityservice.h"

using namespace WebRTC;

PeerConnection::PeerConnection(const char *id) :
  OnIceCandidate(0),
  OnNegotiationNeeded(0),
  OnDataChannel(0),
  _id(id),
  _onoffer(0),
  _onanswer(0),
  _onlocal(0),
  _onremote(0),
  _offer(new rtc::RefCountedObject<OfferObserver>(this)),
  _answer(new rtc::RefCountedObject<AnswerObserver>(this)),
  _local(new rtc::RefCountedObject<LocalDescriptionObserver>(this)),
  _remote(new rtc::RefCountedObject<RemoteDescriptionObserver>(this)),
  _peer(new rtc::RefCountedObject<PeerConnectionObserver>(this)),
  _factory(webrtc::CreatePeerConnectionFactory(rtc::Thread::Current(), rtc::Thread::Current(), NULL, NULL, NULL)),
  _port(webrtc::FakePortAllocatorFactory::Create())
{
  LOG(LS_INFO) << _id << "::New()";

  _constraints.SetMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveAudio, webrtc::MediaConstraintsInterface::kValueTrue);
  _constraints.SetMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveVideo, webrtc::MediaConstraintsInterface::kValueTrue);
  _constraints.SetMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, webrtc::MediaConstraintsInterface::kValueTrue);
  //_constraints.SetAllowRtpDataChannels();
}

PeerConnection::~PeerConnection() {
  LOG(LS_INFO) << _id << "::Destroy()";

  if (_socket.get()) {
    webrtc::PeerConnectionInterface::SignalingState state(_socket->signaling_state());

    if (state != webrtc::PeerConnectionInterface::kClosed) {
      _socket->Close();
    }

    _socket.release();
  }

  EventEmitter::End();
}

webrtc::PeerConnectionInterface *PeerConnection::GetSocket() {
  LOG(LS_INFO) << _id << "::GetSocket()";

  if (!_socket.get()) {
    EventEmitter::Start();

    if (_factory.get()) {
      _socket = _factory->CreatePeerConnection(_servers, &_constraints, _port.get(), new FakeIdentityService(), _peer.get());
    }
  }

  return _socket.get();
}

rtc::scoped_refptr<PeerConnection> PeerConnection::New(const char *id) {
  return new rtc::RefCountedObject<PeerConnection>(id);
}

void PeerConnection::CreateOffer(SdpCallback callback) {
  LOG(LS_INFO) << _id << "::CreateOffer()";
  
  webrtc::PeerConnectionInterface *socket = PeerConnection::GetSocket();
  _onoffer = callback;
  
  socket->CreateOffer(_offer.get(), NULL);
}

void PeerConnection::CreateAnswer(SdpCallback callback) {
  LOG(LS_INFO) << _id << "::CreateAnswer()";
  
  webrtc::PeerConnectionInterface *socket = PeerConnection::GetSocket();
  _onanswer = callback;

  socket->CreateAnswer(_answer.get(), &_constraints);
}

void PeerConnection::SetLocalDescription(const std::string &data, Callback callback) {
  LOG(LS_INFO) << _id << "::SetLocalDescription()";
  
  webrtc::PeerConnectionInterface *socket = PeerConnection::GetSocket();
  _onlocal = callback;

  std::string type;
  std::string sdp;
  Json::Reader reader;
  Json::Value jsonData;

  reader.parse(data, jsonData);
  rtc::GetStringFromJsonObject(jsonData, "type", &type);
  rtc::GetStringFromJsonObject(jsonData, "sdp", &sdp);

  webrtc::SessionDescriptionInterface *desc(webrtc::CreateSessionDescription(type, sdp));
  socket->SetLocalDescription(_local.get(), desc);
}

void PeerConnection::SetRemoteDescription(const std::string &data, Callback callback) {
  LOG(LS_INFO) << _id << "::SetRemoteDescription()";
  
  webrtc::PeerConnectionInterface *socket = PeerConnection::GetSocket();
  _onremote = callback;

  std::string type;
  std::string sdp;
  Json::Reader reader;
  Json::Value jsonData;

  reader.parse(data, jsonData);
  rtc::GetStringFromJsonObject(jsonData, "type", &type);
  rtc::GetStringFromJsonObject(jsonData, "sdp", &sdp);

  webrtc::SessionDescriptionInterface *desc(webrtc::CreateSessionDescription(type, sdp));
  socket->SetRemoteDescription(_remote.get(), desc);
}

void PeerConnection::AddIceCandidate(const std::string &data) {
  LOG(LS_INFO) << _id << "::AddIceCandidate()";
  
  webrtc::PeerConnectionInterface *socket = PeerConnection::GetSocket();

  int sdpMLineIndex = 0;
  std::string sdp;
  std::string sdpMid;
  Json::Reader reader;
  Json::Value jsonData;

  reader.parse(data, jsonData);
  rtc::GetStringFromJsonObject(jsonData, "sdpMid", &sdpMid);
  rtc::GetIntFromJsonObject(jsonData, "sdpMLineIndex", &sdpMLineIndex);
  rtc::GetStringFromJsonObject(jsonData, "candidate", &sdp);

  rtc::scoped_ptr<webrtc::IceCandidateInterface> candidate(webrtc::CreateIceCandidate(sdpMid, sdpMLineIndex, sdp));
  socket->AddIceCandidate(candidate.get());
}

rtc::scoped_refptr<webrtc::DataChannelInterface> PeerConnection::CreateDataChannel() {
  LOG(LS_INFO) << _id << "::CreateDataChannel()";
  
  webrtc::PeerConnectionInterface *socket = PeerConnection::GetSocket();
  webrtc::DataChannelInit config;
  
  config.reliable = true;
  config.ordered = true;

  rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel = socket->CreateDataChannel("TestChannel", &config);

  return dataChannel;
}

void PeerConnection::AddStream() {
  LOG(LS_INFO) << _id << "::AddStream()";
  //webrtc::PeerConnectionInterface *socket = PeerConnection::GetSocket();
}

void PeerConnection::RemoveStream() {
  LOG(LS_INFO) << _id << "::RemoveStream()";
  //webrtc::PeerConnectionInterface *socket = PeerConnection::GetSocket();
}

void PeerConnection::Close() {
  LOG(LS_INFO) << _id << "::Close()";
  
  if (_socket.get()) {
    webrtc::PeerConnectionInterface::SignalingState state(_socket->signaling_state());

    if (state != webrtc::PeerConnectionInterface::kClosed) {
      _socket->Close();
    }
  }

  EventEmitter::End();
}

void PeerConnection::On(Event *event) {
  PeerConnectionEvent type = event->Type<PeerConnectionEvent>();
  rtc::scoped_refptr<PeerConnection> self(this);

  LOG(LS_INFO) << _id << "::On(" << type << ")";

  std::string empty;

  switch (type) {
    case kPeerConnectionCreateOffer:
      if (_onoffer) {
        _onoffer(self, empty, event->Unwrap<std::string>());
      }

      break;
    case kPeerConnectionCreateOfferError:
      if (_onoffer) {
        _onoffer(self, event->Unwrap<std::string>(), empty);
      }

      break;
    case kPeerConnectionCreateAnswer:
      if (_onanswer) {
        _onanswer(self, empty, event->Unwrap<std::string>());
      }

      break;
    case kPeerConnectionCreateAnswerError:
      if (_onanswer) {
        _onanswer(self, event->Unwrap<std::string>(), empty);
      }

      break;
    case kPeerConnectionSetLocalDescription:
      if (_onlocal) {
        _onlocal(self, empty);
      }

      break;
    case kPeerConnectionSetLocalDescriptionError:
      if (_onlocal) {
        _onlocal(self, event->Unwrap<std::string>());
      }

      break;
    case kPeerConnectionSetRemoteDescription:
      if (_onremote) {
        _onremote(self, empty);
      }

      break;
    case kPeerConnectionSetRemoteDescriptionError:
      if (_onremote) {
        _onremote(self, event->Unwrap<std::string>());
      }

      break;
    case kPeerConnectionIceCandidate:
      if (OnIceCandidate) {
        OnIceCandidate(self, empty, event->Unwrap<std::string>());
      }

      break;
    case kPeerConnectionSignalChange:

      break;
    case kPeerConnectionIceChange:

      break;
    case kPeerConnectionIceGathering:

      break;
    case kPeerConnectionDataChannel:
      if (OnDataChannel) {
        OnDataChannel(self, event->Unwrap<rtc::scoped_refptr<webrtc::DataChannelInterface> >());
      }

      break;
    case kPeerConnectionAddStream:

      break;
    case kPeerConnectionRemoveStream:

      break;
    case kPeerConnectionRenegotiation:
      if (OnNegotiationNeeded) {
        OnNegotiationNeeded(self, empty);
      }

      break;
  }
}
