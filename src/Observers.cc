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
#include "PeerConnection.h"
#include "DataChannel.h"
#include "MediaStream.h"
#include "MediaStreamTrack.h"

using namespace WebRTC;

OfferObserver::OfferObserver(EventEmitter *listener) : 
  NotifyEmitter(listener) { }

void OfferObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Json::StyledWriter writer;
  Json::Value msg;
  std::string sdp;
  
  if (desc->ToString(&sdp)) {
    msg["type"] = desc->type();
    msg["sdp"] = sdp;

    Emit(kPeerConnectionCreateOffer, writer.write(msg));
  }
}

void OfferObserver::OnFailure(const std::string &error) {
  LOG(LS_ERROR) << __PRETTY_FUNCTION__;
  
  Emit(kPeerConnectionCreateOfferError, error);
}

AnswerObserver::AnswerObserver(EventEmitter *listener) : 
  NotifyEmitter(listener) { }
  
void AnswerObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Json::StyledWriter writer;
  Json::Value msg;
  std::string sdp;
  
  if (desc->ToString(&sdp)) { 
    msg["type"] = desc->type();
    msg["sdp"] = sdp;
    
    Emit(kPeerConnectionCreateAnswer, writer.write(msg));
  }
}

void AnswerObserver::OnFailure(const std::string &error) {
  LOG(LS_ERROR) << __PRETTY_FUNCTION__;
  
  Emit(kPeerConnectionCreateAnswerError, error);
}

LocalDescriptionObserver::LocalDescriptionObserver(EventEmitter *listener) : 
  NotifyEmitter(listener) { }

void LocalDescriptionObserver::OnSuccess() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Emit(kPeerConnectionSetLocalDescription);
}

void LocalDescriptionObserver::OnFailure(const std::string &error) {
  LOG(LS_ERROR) << __PRETTY_FUNCTION__;
  
  Emit(kPeerConnectionSetLocalDescriptionError, error);
}

RemoteDescriptionObserver::RemoteDescriptionObserver(EventEmitter *listener) : 
  NotifyEmitter(listener) { }

void RemoteDescriptionObserver::OnSuccess() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Emit(kPeerConnectionSetRemoteDescription);
}

void RemoteDescriptionObserver::OnFailure(const std::string &error) {
  LOG(LS_ERROR) << __PRETTY_FUNCTION__;
  
  Emit(kPeerConnectionSetRemoteDescriptionError, error);
}

PeerConnectionObserver::PeerConnectionObserver(EventEmitter *listener) : 
  NotifyEmitter(listener) { }

void PeerConnectionObserver::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState state) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Emit(kPeerConnectionSignalChange);
  
  if (state == webrtc::PeerConnectionInterface::kClosed) {
    Emit(kPeerConnectionCreateClosed);
  }
}

void PeerConnectionObserver::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState state) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Emit(kPeerConnectionIceChange);
}

void PeerConnectionObserver::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState state) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Emit(kPeerConnectionIceGathering);
  
  if (state == webrtc::PeerConnectionInterface::kIceGatheringComplete) {
    Emit(kPeerConnectionIceCandidate, std::string());
  }
}

void PeerConnectionObserver::OnStateChange(webrtc::PeerConnectionObserver::StateType state) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

void PeerConnectionObserver::OnDataChannel(webrtc::DataChannelInterface *channel) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel = channel;
  
  if (dataChannel.get()) {
    Emit(kPeerConnectionDataChannel, dataChannel);
  }
}

void PeerConnectionObserver::OnAddStream(webrtc::MediaStreamInterface *stream) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = stream;

  if (mediaStream.get()) {
    Emit(kPeerConnectionAddStream, mediaStream);
  }
}

void PeerConnectionObserver::OnRemoveStream(webrtc::MediaStreamInterface *stream) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = stream;

  if (mediaStream.get()) {
    Emit(kPeerConnectionRemoveStream, mediaStream);
  }
}

void PeerConnectionObserver::OnRenegotiationNeeded() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Emit(kPeerConnectionRenegotiation);
}

void PeerConnectionObserver::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Json::StyledWriter writer;
  Json::Value msg;
  std::string sdp;
  
  if (candidate->ToString(&sdp)) {
    msg["sdpMid"] = candidate->sdp_mid();
    msg["sdpMLineIndex"] = candidate->sdp_mline_index();
    msg["candidate"] = sdp;
    
    Emit(kPeerConnectionIceCandidate, writer.write(msg));
  }
}

DataChannelObserver::DataChannelObserver(EventEmitter *listener) : 
  NotifyEmitter(listener) { }

void DataChannelObserver::OnStateChange() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;

  Emit(kDataChannelStateChange);
}

void DataChannelObserver::OnMessage(const webrtc::DataBuffer& buffer) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (buffer.binary) {
    Emit(kDataChannelBinary, buffer.data);
  } else {
    Emit(kDataChannelData, buffer.data);
  }
}

MediaStreamObserver::MediaStreamObserver(EventEmitter *listener) :
  NotifyEmitter(listener) { }

void MediaStreamObserver::OnChanged() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Emit(kMediaStreamChanged);
}

MediaStreamTrackObserver::MediaStreamTrackObserver(EventEmitter *listener) :
  NotifyEmitter(listener) { }

void MediaStreamTrackObserver::OnChanged() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Emit(kMediaStreamTrackChanged);
}

StatsObserver::StatsObserver(EventEmitter *listener) :
  NotifyEmitter(listener) { }

void StatsObserver::OnComplete(const webrtc::StatsReports &reports) {
  LOG(LS_INFO) << "StatsObserver::OnComplete()";
  
  Emit(kPeerConnectionStats, reports);
}
