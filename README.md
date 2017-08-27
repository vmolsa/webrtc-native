## Object RTC (ORTC) API for WebRTC

Object Real-Time Communications (ORTC) provides a powerful API for the development of WebRTC based applications. ORTC does not utilize Session Description Protocol (SDP) in the API, nor does it mandate support for the Offer/Answer state machine (though an application is free to choose SDP and Offer/Answer as an on-the-wire signaling mechanism). Instead, ORTC uses "sender", "receiver" and "transport" objects, which have "capabilities" describing what they are capable of doing, as well as "parameters" which define what they are configured to do. "Tracks" are encoded by senders and sent over transports, then decoded by receivers while "data channels" are sent over transports directly.

In a Javascript application utilizing the ORTC API, the relationship between the application and the objects, as well as between the objects themselves is shown below. Horizontal or slanted arrows denote the flow of media or data, whereas vertical arrows denote interactions via methods and events.

![ORTC](http://draft.ortc.org/images/quic.svg)

### API

[Documentation](https://rawgit.com/vmolsa/webrtc-native/ortc/doc/modules/_ortc_.html)

### References

[ORTC](http://draft.ortc.org/)
[Interactive Connectivity Establishment (ICE)](https://tools.ietf.org/html/rfc5245)
[STUN - Simple Traversal of User Datagram Protocol (UDP)](https://tools.ietf.org/html/rfc3489)
[Session Traversal Utilities for NAT (STUN)](https://tools.ietf.org/html/rfc5389)
[Traversal Using Relays around NAT (TURN)](https://tools.ietf.org/html/rfc5766)
[RTP: A Transport Protocol for Real-Time Applications](https://tools.ietf.org/html/rfc1889)
[Stream Control Transmission Protocol](https://tools.ietf.org/html/rfc4960)
[WebRTC Data Channel Protocol](https://tools.ietf.org/html/draft-jesup-rtcweb-data-protocol-04#page-3)

### WIP

[RTCICEGatherer](http://draft.ortc.org/#rtcicegatherer*) :question:
[RTCIceParameters](http://draft.ortc.org/#rtciceparameters*) :+1:
[RTCIceCandidate](http://draft.ortc.org/#rtcicecandidate*) :+1:
[RTCIceProtocol](http://draft.ortc.org/#rtciceprotocol-enum) :+1:
[RTCIceTcpCandidateType](http://draft.ortc.org/#rtcicetcpcandidatetype-enum) :+1:
[RTCIceCandidateType](http://draft.ortc.org/#rtcicecandidatetype-enum) :+1:
[RTCIceCandidateComplete](http://draft.ortc.org/#rtcicecandidatecomplete*) :+1:
[RTCIceGathererState](http://draft.ortc.org/#rtcicegathererstate*) :+1:
[RTCIceGathererIceErrorEvent](http://draft.ortc.org/#rtcicegatherericeerrorevent) :question:
[RTCIceGathererEvent](http://draft.ortc.org/#rtcicegathererevent) :question:
[RTCIceGatherOptions](http://draft.ortc.org/#rtcicegatheroptions*) :+1:
[RTCIceGatherPolicy](http://draft.ortc.org/#rtcicegatherpolicy*) :+1:
[RTCIceCredentialType](http://draft.ortc.org/#rtcicecredentialtype*) :+1:
[RTCOAuthCredential](http://draft.ortc.org/#rtcoauthcredential-dictionary) :+1:
[RTCIceServer](http://draft.ortc.org/#rtciceserver*) :+1:
[RTCIceTransport](http://draft.ortc.org/#rtcicetransport*) :question:
[RTCIceComponent](http://draft.ortc.org/#rtcicecomponent*) :+1:
[RTCIceRole](http://draft.ortc.org/#rtcicerole*) :+1:
[RTCIceTransportState](http://draft.ortc.org/#rtcicetransportstate*) :+1:
[RTCIceCandidatePairChangedEvent](http://draft.ortc.org/#rtcicecandidatepairchangedevent-interface-definition*) :question:
[RTCIceCandidatePair](http://draft.ortc.org/#rtcicecandidatepair*) :+1:
[RTCDtlsTransport](http://draft.ortc.org/#rtcdtlstransport*) :question:
[RTCDtlsParameters](http://draft.ortc.org/#rtcdtlsparameters*) :+1:
[RTCDtlsFingerprint](http://draft.ortc.org/#rtcdtlsfingerprint*) :+1:
[RTCDtlsRole](http://draft.ortc.org/#rtcdtlsrole*) :+1:
[RTCDtlsTransportState](http://draft.ortc.org/#rtcdtlstransportstate*) :+1:
[RTCRtpSender](http://draft.ortc.org/#rtcrtpsender*) :question:
[RTCSsrcConflictEvent](http://draft.ortc.org/#rtcsssrcconflictevent-interface-definition*) :question:
[RTCRtpReceiver](http://draft.ortc.org/#rtcrtpreceiver*) :question:
[RTCRtpSynchronizationSource](http://draft.ortc.org/#rtcrtpsynchronizationsource*) :+1:
[RTCRtpContributingSource](http://draft.ortc.org/#rtcrtpcontributingsource*) :+1:
[RTCIceTransportController](http://draft.ortc.org/#rtcicetransportcontroller*) :question:
[RTCRtpListener](http://draft.ortc.org/#rtcrtplistener*) :question:
[RTCRtpUnhandledEvent](http://draft.ortc.org/#rtcrtpunhandledevent-interface-definition*) :question:
[RTCRtpCapabilities](http://draft.ortc.org/#rtcrtpcapabilities*) :+1:
[RTCRtcpFeedback](http://draft.ortc.org/#rtcrtcpfeedback*) :+1:
[RTCRtpCodecCapability](http://draft.ortc.org/#rtcrtpcodeccapability*) :+1:
[RTCRtpParameters](http://draft.ortc.org/#rtcrtpparameters*) :+1:
[RTCDegradationPreference](http://draft.ortc.org/#rtcdegradationpreference*) :+1:
[RTCRtcpParameters](http://draft.ortc.org/#rtcrtcpparameters*) :+1:
[RTCRtpCodecParameters](http://draft.ortc.org/#rtcrtpcodecparameters*) :+1:
[RTCRtpEncodingParameters](http://draft.ortc.org/#rtcrtpencodingparameters*) :+1:
[RTCPriorityType](http://draft.ortc.org/#rtcprioritytype*) :+1:
[RTCRtpFecParameters](http://draft.ortc.org/#rtcrtpfecparameters*) :+1:
[RTCRtpRtxParameters](http://draft.ortc.org/#rtcrtprtxparameters*) :+1:
[RTCRtpHeaderExtension](http://draft.ortc.org/#rtcrtpheaderextension*) :+1:
[RTCRtpHeaderExtensionParameters](http://draft.ortc.org/#rtcrtpheaderextensionparameters*) :+1:
[RTCDtmfSender](http://draft.ortc.org/#rtcdtmfsender*) :question:
[RTCDTMFToneChangeEvent](http://draft.ortc.org/#rtcdtmftonechangeevent) :question:
[RTCDTMFSender](http://draft.ortc.org/#rtcdtmfsender2*) :question:
[RTCDataChannel](http://draft.ortc.org/#rtcdatachannel*) :question:
[RTCDataTransport](http://draft.ortc.org/#rtcdatatransport-interface-definition*) :+1:
[RTCDataChannelState](http://draft.ortc.org/#rtcdatachannelstate*) :+1:
[RTCDataChannelParameters](http://draft.ortc.org/#rtcdatachannelparameters*) :+1:
[RTCSctpTransport](http://draft.ortc.org/#sctp-transport*) :question:
[RTCSctpTransportState](http://draft.ortc.org/#rtcsctptransportstate*) :+1:
[RTCSctpCapabilities](http://draft.ortc.org/#rtcsctpcapabilities*) :+1:
[RTCDataChannelEvent](http://draft.ortc.org/#rtcdatachannelevent) :question:
[RTCQuicTransport](http://draft.ortc.org/#quic-transport*) :question:
[RTCQuicParameters](http://draft.ortc.org/#rtcquicparameters*) :question:
[RTCQuicRole](http://draft.ortc.org/#rtcquicrole*) :question:
[RTCQuicTransportState](http://draft.ortc.org/#rtcquictransportstate*) :question:
[RTCStatsReport](http://draft.ortc.org/#rtcstatsreport-interface) :+1:
[RTCStats](http://draft.ortc.org/#rtcstats-dictionary) :+1:
[RTCIdentity](http://draft.ortc.org/#rtcidentity-interface) :question:
[RTCIdentityError](http://draft.ortc.org/#rtcidentityerror-dictionary) :question:
[RTCIdentityAssertion](http://draft.ortc.org/#rtcidentityassertion-dictionary) :question:
[RTCCertificate](http://draft.ortc.org/#rtccertificate-interface) :question: