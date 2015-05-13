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

#include "Observers.h"

using namespace WebRTC;

OfferObserver::OfferObserver(EventEmitter *parent) : 
  _parent(parent) { }

void OfferObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
  LOG(LS_INFO) << "OfferObserver::OnSuccess()";
  
  Json::StyledWriter writer;
  Json::Value msg;
  std::string sdp;
  
  if (desc->ToString(&sdp)) {
    msg["type"] = desc->type();
    msg["sdp"] = sdp;

    _parent->Emit(kPeerConnectionCreateOffer, writer.write(msg));
  }
}

void OfferObserver::OnFailure(const std::string &error) {
  LOG(LS_ERROR) << "OfferObserver::OnFailure(" << error.c_str() << ")";
  _parent->Emit(kPeerConnectionCreateOfferError, error);
}

AnswerObserver::AnswerObserver(EventEmitter *parent) : 
  _parent(parent) { }
   
void AnswerObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
  LOG(LS_INFO) << "AnswerObserver::OnSuccess()";
  
  Json::StyledWriter writer;
  Json::Value msg;
  std::string sdp;
  
  if (desc->ToString(&sdp)) { 
    msg["type"] = desc->type();
    msg["sdp"] = sdp;

    _parent->Emit(kPeerConnectionCreateAnswer, writer.write(msg));
  }
}

void AnswerObserver::OnFailure(const std::string &error) {
  LOG(LS_ERROR) << "AnswerObserver::OnFailure(" << error.c_str() << ")";
  _parent->Emit(kPeerConnectionCreateAnswerError, error);
}

LocalDescriptionObserver::LocalDescriptionObserver(EventEmitter *parent) : 
  _parent(parent) { }
    
void LocalDescriptionObserver::OnSuccess() {
  LOG(LS_INFO) << "LocalDescriptionObserver::OnSuccess()";
  _parent->Emit(kPeerConnectionSetLocalDescription);
}

void LocalDescriptionObserver::OnFailure(const std::string &error) {
  LOG(LS_ERROR) << "LocalDescriptionObserver::OnFailure(" << error.c_str() << ")";
  _parent->Emit(kPeerConnectionSetLocalDescriptionError, error);
}

RemoteDescriptionObserver::RemoteDescriptionObserver(EventEmitter *parent) : 
  _parent(parent) { }
    
void RemoteDescriptionObserver::OnSuccess() {
  LOG(LS_INFO) << "RemoteDescriptionObserver::OnSuccess()";
  _parent->Emit(kPeerConnectionSetRemoteDescription);
}

void RemoteDescriptionObserver::OnFailure(const std::string &error) {
  LOG(LS_ERROR) << "RemoteDescriptionObserver::OnFailure(" << error.c_str() << ")";
  _parent->Emit(kPeerConnectionSetRemoteDescriptionError, error);
}

PeerConnectionObserver::PeerConnectionObserver(EventEmitter *parent) : 
  _parent(parent) { }

void PeerConnectionObserver::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState state) {
  LOG(LS_INFO) << "PeerConnectionObserver::OnSignalingChange()";
  _parent->Emit(kPeerConnectionSignalChange);
  
  if (state == webrtc::PeerConnectionInterface::kClosed) {
    _parent->Stop();
  }
}

void PeerConnectionObserver::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState state) {
  LOG(LS_INFO) << "PeerConnectionObserver::OnIceConnectionChange()";
  _parent->Emit(kPeerConnectionIceChange);
}

void PeerConnectionObserver::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState state) {
  LOG(LS_INFO) << "PeerConnectionObserver::OnIceGatheringChange()";
  _parent->Emit(kPeerConnectionIceGathering);
}

void PeerConnectionObserver::OnStateChange(webrtc::PeerConnectionObserver::StateType state) {
  LOG(LS_INFO) << "PeerConnectionObserver::OnStateChange()";
}

void PeerConnectionObserver::OnDataChannel(webrtc::DataChannelInterface *channel) {
  LOG(LS_INFO) << "PeerConnectionObserver::OnDataChannel()";
  rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel = channel;
  
  if (dataChannel.get()) {
    _parent->Emit(kPeerConnectionDataChannel, dataChannel);
  }
}

void PeerConnectionObserver::OnAddStream(webrtc::MediaStreamInterface *stream) {
  LOG(LS_INFO) << "PeerConnectionObserver::OnAddStream()";
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = stream;

  if (mediaStream.get()) {
    _parent->Emit(kPeerConnectionAddStream, mediaStream);
  }
}

void PeerConnectionObserver::OnRemoveStream(webrtc::MediaStreamInterface *stream) {
  LOG(LS_INFO) << "PeerConnectionObserver::OnRemoveStream()";
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = stream;

  if (mediaStream.get()) {
    _parent->Emit(kPeerConnectionRemoveStream, mediaStream);
  }
}

void PeerConnectionObserver::OnRenegotiationNeeded() {
  LOG(LS_INFO) << "PeerConnectionObserver::OnRenegotiationNeeded()";
  _parent->Emit(kPeerConnectionRenegotiation);
}

void PeerConnectionObserver::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
  LOG(LS_INFO) << "PeerConnectionObserver::OnIceCandidate()";
  
  Json::StyledWriter writer;
  Json::Value msg;
  std::string sdp;
  
  if (candidate->ToString(&sdp)) {
    msg["sdpMid"] = candidate->sdp_mid();
    msg["sdpMLineIndex"] = candidate->sdp_mline_index();
    msg["candidate"] = sdp;
    
    _parent->Emit(kPeerConnectionIceCandidate, writer.write(msg));
  }
}

DataChannelObserver::DataChannelObserver(EventEmitter *parent) : 
  _parent(parent) { }
  
void DataChannelObserver::OnStateChange() {
  LOG(LS_INFO) << "DataChannelObserver::OnStateChange()";
  _parent->Emit(kDataChannelStateChange);
}

void DataChannelObserver::OnMessage(const webrtc::DataBuffer& buffer) {
  LOG(LS_INFO) << "DataChannelObserver::OnMessage()";
  
  if (buffer.binary) {
    _parent->Emit(kDataChannelBinary, buffer.data);
  } else {
    _parent->Emit(kDataChannelData, buffer.data);
  }
}

MediaStreamObserver::MediaStreamObserver(EventEmitter *parent) :
  _parent(parent) { }

void MediaStreamObserver::OnChanged() {
  LOG(LS_INFO) << "MediaStreamObserver::OnChanged()";
  _parent->Emit(kMediaStreamChanged);
}

MediaStreamTrackObserver::MediaStreamTrackObserver(EventEmitter *parent) :
  _parent(parent) { }

void MediaStreamTrackObserver::OnChanged() {
  LOG(LS_INFO) << "MediaStreamTrackObserver::OnChanged()";
  _parent->Emit(kMediaStreamTrackChanged);
}
