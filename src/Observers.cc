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

OfferObserver::OfferObserver(EventEmitter *listener) : 
  _listener(listener) { }

void OfferObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Json::StyledWriter writer;
  Json::Value msg;
  std::string sdp;
  
  if (desc->ToString(&sdp)) {
    msg["type"] = desc->type();
    msg["sdp"] = sdp;

    if (_listener) {
      _listener->Emit(kPeerConnectionCreateOffer, writer.write(msg));
    }
  }
}

void OfferObserver::SetEmitter(EventEmitter *listener) {
  _listener = listener;
}

void OfferObserver::OnFailure(const std::string &error) {
  LOG(LS_ERROR) << __PRETTY_FUNCTION__;
  
  if (_listener) {
    _listener->Emit(kPeerConnectionCreateOfferError, error);
  }
}

AnswerObserver::AnswerObserver(EventEmitter *listener) : 
  _listener(listener) { }
 
void AnswerObserver::SetEmitter(EventEmitter *listener) {
  _listener = listener;
}
   
void AnswerObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  Json::StyledWriter writer;
  Json::Value msg;
  std::string sdp;
  
  if (desc->ToString(&sdp)) { 
    msg["type"] = desc->type();
    msg["sdp"] = sdp;
    
    if (_listener) {
      _listener->Emit(kPeerConnectionCreateAnswer, writer.write(msg));
    }
  }
}

void AnswerObserver::OnFailure(const std::string &error) {
  LOG(LS_ERROR) << __PRETTY_FUNCTION__;
  
  if (_listener) {
    _listener->Emit(kPeerConnectionCreateAnswerError, error);
  }
}

LocalDescriptionObserver::LocalDescriptionObserver(EventEmitter *listener) : 
  _listener(listener) { }

void LocalDescriptionObserver::SetEmitter(EventEmitter *listener) {
  _listener = listener;
}
    
void LocalDescriptionObserver::OnSuccess() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_listener) {
    _listener->Emit(kPeerConnectionSetLocalDescription);
  }
}

void LocalDescriptionObserver::OnFailure(const std::string &error) {
  LOG(LS_ERROR) << __PRETTY_FUNCTION__;
  
  if (_listener) {
    _listener->Emit(kPeerConnectionSetLocalDescriptionError, error);
  }
}

RemoteDescriptionObserver::RemoteDescriptionObserver(EventEmitter *listener) : 
  _listener(listener) { }

void RemoteDescriptionObserver::SetEmitter(EventEmitter *listener) {
  _listener = listener;
}
    
void RemoteDescriptionObserver::OnSuccess() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_listener) {
    _listener->Emit(kPeerConnectionSetRemoteDescription);
  }
}

void RemoteDescriptionObserver::OnFailure(const std::string &error) {
  LOG(LS_ERROR) << __PRETTY_FUNCTION__;
  
  if (_listener) {
    _listener->Emit(kPeerConnectionSetRemoteDescriptionError, error);
  }
}

PeerConnectionObserver::PeerConnectionObserver(EventEmitter *listener) : 
  _listener(listener) { }

void PeerConnectionObserver::SetEmitter(EventEmitter *listener) {
  _listener = listener;
}

void PeerConnectionObserver::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState state) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_listener) {
    _listener->Emit(kPeerConnectionSignalChange);
  
    if (state == webrtc::PeerConnectionInterface::kClosed) {
      _listener->SetReference(false);
    }
  }
}

void PeerConnectionObserver::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState state) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_listener) {
    _listener->Emit(kPeerConnectionIceChange);
  }
}

void PeerConnectionObserver::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState state) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_listener) {
    _listener->Emit(kPeerConnectionIceGathering);
  }
}

void PeerConnectionObserver::OnStateChange(webrtc::PeerConnectionObserver::StateType state) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

void PeerConnectionObserver::OnDataChannel(webrtc::DataChannelInterface *channel) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel = channel;
  
  if (dataChannel.get() && _listener) {
    _listener->Emit(kPeerConnectionDataChannel, dataChannel);
  }
}

void PeerConnectionObserver::OnAddStream(webrtc::MediaStreamInterface *stream) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = stream;

  if (mediaStream.get() && _listener) {
    _listener->Emit(kPeerConnectionAddStream, mediaStream);
  }
}

void PeerConnectionObserver::OnRemoveStream(webrtc::MediaStreamInterface *stream) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = stream;

  if (mediaStream.get() && _listener) {
    _listener->Emit(kPeerConnectionRemoveStream, mediaStream);
  }
}

void PeerConnectionObserver::OnRenegotiationNeeded() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_listener) {
    _listener->Emit(kPeerConnectionRenegotiation);
  }
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
    
    if (_listener) {
      _listener->Emit(kPeerConnectionIceCandidate, writer.write(msg));
    }
  }
}

DataChannelObserver::DataChannelObserver(EventEmitter *listener) : 
  _listener(listener) { }

void DataChannelObserver::SetEmitter(EventEmitter *listener) {
  _listener = listener;
}
  
void DataChannelObserver::OnStateChange() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_listener) {
    _listener->Emit(kDataChannelStateChange);
  }
}

void DataChannelObserver::OnMessage(const webrtc::DataBuffer& buffer) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_listener) {
    if (buffer.binary) {
      _listener->Emit(kDataChannelBinary, buffer.data);
    } else {
      _listener->Emit(kDataChannelData, buffer.data);
    }
  }
}

MediaStreamObserver::MediaStreamObserver(EventEmitter *listener) :
  _listener(listener) { }

void MediaStreamObserver::OnChanged() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_listener) {
    _listener->Emit(kMediaStreamChanged);
  }
}

void MediaStreamObserver::SetEmitter(EventEmitter *listener) {
  _listener = listener;
}

MediaStreamTrackObserver::MediaStreamTrackObserver(EventEmitter *listener) :
  _listener(listener) { }

void MediaStreamTrackObserver::SetEmitter(EventEmitter *listener) {
  _listener = listener;
}

void MediaStreamTrackObserver::OnChanged() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  
  if (_listener) {
    _listener->Emit(kMediaStreamTrackChanged);
  }
}

StatsObserver::StatsObserver(EventEmitter *listener) :
  _listener(listener) { }

void StatsObserver::SetEmitter(EventEmitter *listener) {
  _listener = listener;
}

void StatsObserver::OnComplete(const webrtc::StatsReports &reports) {
  LOG(LS_INFO) << "StatsObserver::OnComplete()";
  
  if (_listener) {
    _listener->Emit(kPeerConnectionStats, reports);
  }
}
