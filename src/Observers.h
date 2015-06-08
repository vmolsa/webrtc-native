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

#include "webrtc/base/logging.h"
#include "webrtc/base/json.h"
#include "webrtc/base/basictypes.h"
#include "webrtc/base/common.h"
#include "webrtc/base/scoped_ptr.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/base/sslstreamadapter.h"
#include "webrtc/base/stringutils.h"
#include "webrtc/base/thread.h"
#include "webrtc/base/buffer.h"
#include "webrtc/base/scoped_ref_ptr.h"
#include "webrtc/base/refcount.h"

#include "talk/app/webrtc/jsep.h"
#include "talk/app/webrtc/jsepsessiondescription.h"
#include "talk/app/webrtc/mediaconstraintsinterface.h"
#include "talk/app/webrtc/mediastreaminterface.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/app/webrtc/test/fakeconstraints.h"
#include "talk/app/webrtc/fakeportallocatorfactory.h"
#include "talk/app/webrtc/datachannelinterface.h"
#include "talk/app/webrtc/videosourceinterface.h"
#include "talk/app/webrtc/videosource.h"

#include "talk/media/sctp/sctpdataengine.h"
#include "talk/media/devices/devicemanager.h"
#include "talk/session/media/mediasession.h"

namespace webrtc {
  class VideoCaptureModule;
};

namespace cricket {
  class VideoRenderer;
};

#include "EventEmitter.h" // NOLINT(build/include)

namespace WebRTC {
  enum PeerConnectionEvent {
    kPeerConnectionCreateOffer = 1,
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
  
  enum DataChannelEvent {
    kDataChannelStateChange,
    kDataChannelBinary,
    kDataChannelData,
  };

  enum MediaStreamEvent {
    kMediaStreamChanged
  };

  enum MediaStreamTrackEvent {
    kMediaStreamTrackChanged
  };
  
  class OfferObserver : public webrtc::CreateSessionDescriptionObserver {
   public:
    OfferObserver(EventEmitter *parent = 0);
    
    void OnSuccess(webrtc::SessionDescriptionInterface* sdp) final;
    void OnFailure(const std::string &error) final;
    
   protected:
    EventEmitter* _parent;
  };
  
  class AnswerObserver : public webrtc::CreateSessionDescriptionObserver {    
   public:
    AnswerObserver(EventEmitter *parent = 0);
   
    void OnSuccess(webrtc::SessionDescriptionInterface* sdp) final;
    void OnFailure(const std::string &error) final;
    
   protected:
    EventEmitter* _parent;
  };

  class LocalDescriptionObserver : public webrtc::SetSessionDescriptionObserver {
   public:
    LocalDescriptionObserver(EventEmitter *parent = 0);
    
    void OnSuccess() final;
    void OnFailure(const std::string &error) final;

   protected:
    EventEmitter* _parent;
  };
    
  class RemoteDescriptionObserver : public webrtc::SetSessionDescriptionObserver {    
   public:
    RemoteDescriptionObserver(EventEmitter *parent = 0);
    
    void OnSuccess() final;
    void OnFailure(const std::string &error) final;

   protected:
    EventEmitter* _parent;
  };
  
  class PeerConnectionObserver : 
    public webrtc::PeerConnectionObserver, 
    public rtc::RefCountInterface 
  {
   public:
    PeerConnectionObserver(EventEmitter *parent = 0);
    
    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState state) final;
    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState state) final;
    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState state) final;
    void OnStateChange(webrtc::PeerConnectionObserver::StateType state) final;
    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) final;
    void OnDataChannel(webrtc::DataChannelInterface* channel) final;
    void OnRenegotiationNeeded() final;

    void OnAddStream(webrtc::MediaStreamInterface* stream) final;
    void OnRemoveStream(webrtc::MediaStreamInterface* stream) final;

   protected:
    EventEmitter* _parent;
  };
  
  class DataChannelObserver : 
    public webrtc::DataChannelObserver, 
    public rtc::RefCountInterface 
  {
   public:
    DataChannelObserver(EventEmitter *parent = 0);
    
    void OnStateChange() final;
    void OnMessage(const webrtc::DataBuffer& buffer) final;
    
   protected:
    EventEmitter* _parent;
  };

  class MediaStreamObserver :
    public webrtc::ObserverInterface,
    public rtc::RefCountInterface
  {
   public:
     MediaStreamObserver(EventEmitter *parent = 0);

     void OnChanged() final;

   protected:
    EventEmitter* _parent;
  };

  class MediaStreamTrackObserver :
    public webrtc::ObserverInterface,
    public rtc::RefCountInterface
  {
   public:
    MediaStreamTrackObserver(EventEmitter *parent = 0);

    void OnChanged() final;

   protected:
    EventEmitter* _parent;
  };

  class StatsObserver : public webrtc::StatsObserver {
   public:
    StatsObserver(EventEmitter *parent = 0);

    void OnComplete(const webrtc::StatsReports& reports) final;

   protected:
    EventEmitter* _parent;
  };
};

#endif
