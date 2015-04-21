#ifndef WEBRTCOBSERVERS_H
#define WEBRTCOBSERVERS_H

#include "webrtc/base/logging.h"
#include "webrtc/system_wrappers/interface/ref_count.h"
#include "webrtc/base/json.h"
#include "webrtc/base/basictypes.h"
#include "webrtc/base/common.h"
#include "webrtc/base/thread.h"
#include "webrtc/base/scoped_ptr.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/base/buffer.h"
#include "webrtc/base/scoped_ref_ptr.h"
#include "webrtc/base/refcount.h"

#include "talk/app/webrtc/jsep.h"
#include "talk/media/devices/devicemanager.h"
#include "talk/app/webrtc/mediaconstraintsinterface.h"
#include "talk/app/webrtc/mediastreaminterface.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/app/webrtc/videosourceinterface.h"
#include "talk/app/webrtc/test/fakeconstraints.h"
#include "talk/app/webrtc/datachannelinterface.h"

#include "EventEmitter.h"

namespace webrtc {
class VideoCaptureModule;
}  // namespace webrtc

namespace cricket {
class VideoRenderer;
}  // namespace cricket

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
    kPeerConnectionRenegotiation
  };
  
  enum DataChannelEvent {
    kDataChannelStateChange,
    kDataChannelBinary,
    kDataChannelData,
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
    void OnAddStream(webrtc::MediaStreamInterface* stream) final;
    void OnRemoveStream(webrtc::MediaStreamInterface* stream) final;
    void OnRenegotiationNeeded() final;
    
   protected:
    EventEmitter* _parent;
  };
  
  class DataChannelObserver : 
    public webrtc::DataChannelObserver, 
    public rtc::RefCountInterface 
  {
   public:
    DataChannelObserver(EventEmitter *parent = 0);
    
    virtual void OnStateChange() final;
    virtual void OnMessage(const webrtc::DataBuffer& buffer) final;
    
   protected:
    EventEmitter* _parent;
  };
};

#endif
