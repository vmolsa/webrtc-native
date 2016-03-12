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

#ifndef WEBRTC_OBSERVERS_H
#define WEBRTC_OBSERVERS_H

#include "EventEmitter.h"

namespace WebRTC {  
  class OfferObserver : public webrtc::CreateSessionDescriptionObserver, public NotifyEmitter {
   public:
    OfferObserver(EventEmitter *listener = 0);
    
    void OnSuccess(webrtc::SessionDescriptionInterface* sdp) final;
    void OnFailure(const std::string &error) final;
  };
  
  class AnswerObserver : public webrtc::CreateSessionDescriptionObserver, public NotifyEmitter {    
   public:
    AnswerObserver(EventEmitter *listener = 0);
    
    void OnSuccess(webrtc::SessionDescriptionInterface* sdp) final;
    void OnFailure(const std::string &error) final;
  };

  class LocalDescriptionObserver : public webrtc::SetSessionDescriptionObserver, public NotifyEmitter {
   public:
    LocalDescriptionObserver(EventEmitter *listener = 0);

    void OnSuccess() final;
    void OnFailure(const std::string &error) final;
  };
    
  class RemoteDescriptionObserver : public webrtc::SetSessionDescriptionObserver, public NotifyEmitter {    
   public:
    RemoteDescriptionObserver(EventEmitter *listener = 0);
    
    void OnSuccess() final;
    void OnFailure(const std::string &error) final;
  };
  
  class PeerConnectionObserver : 
    public webrtc::PeerConnectionObserver, 
    public rtc::RefCountInterface,
    public NotifyEmitter
  {
   public:
    PeerConnectionObserver(EventEmitter *listener = 0);
    
    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState state) final;
    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState state) final;
    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState state) final;
    void OnStateChange(webrtc::PeerConnectionObserver::StateType state);
    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) final;
    void OnDataChannel(webrtc::DataChannelInterface* channel) final;
    void OnRenegotiationNeeded() final;

    void OnAddStream(webrtc::MediaStreamInterface* stream) final;
    void OnRemoveStream(webrtc::MediaStreamInterface* stream) final;
  };
  
  class DataChannelObserver : 
    public webrtc::DataChannelObserver, 
    public rtc::RefCountInterface,
    public NotifyEmitter 
  {
   public:
    DataChannelObserver(EventEmitter *listener = 0);

    void OnStateChange() final;
    void OnMessage(const webrtc::DataBuffer& buffer) final;
  };

  class MediaStreamObserver :
    public webrtc::ObserverInterface,
    public rtc::RefCountInterface,
    public NotifyEmitter 
  {
   public:
     MediaStreamObserver(EventEmitter *listener = 0);
     
     void OnChanged() final;
  };

  class MediaStreamTrackObserver :
    public webrtc::ObserverInterface,
    public rtc::RefCountInterface,
    public NotifyEmitter
  {
   public:
    MediaStreamTrackObserver(EventEmitter *listener = 0);
    
    void OnChanged() final;
  };

  class StatsObserver : public webrtc::StatsObserver, public NotifyEmitter {
   public:
    StatsObserver(EventEmitter *listener = 0);

    void OnComplete(const webrtc::StatsReports& reports) final;
  };
};

#endif
