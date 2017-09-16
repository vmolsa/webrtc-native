## Object RTC (ORTC) API for WebRTC

Object Real-Time Communications (ORTC) provides a powerful API for the development of WebRTC based applications. ORTC does not utilize Session Description Protocol (SDP) in the API, nor does it mandate support for the Offer/Answer state machine (though an application is free to choose SDP and Offer/Answer as an on-the-wire signaling mechanism). Instead, ORTC uses "sender", "receiver" and "transport" objects, which have "capabilities" describing what they are capable of doing, as well as "parameters" which define what they are configured to do. "Tracks" are encoded by senders and sent over transports, then decoded by receivers while "data channels" are sent over transports directly.

In a Javascript application utilizing the ORTC API, the relationship between the application and the objects, as well as between the objects themselves is shown below. Horizontal or slanted arrows denote the flow of media or data, whereas vertical arrows denote interactions via methods and events.

![ORTC](http://draft.ortc.org/images/quic.svg)

### API

[Documentation](https://rawgit.com/vmolsa/webrtc-native/ortc/doc/modules/_ortc_.html)

### References

 - [ORTC](http://draft.ortc.org/)
 - [Interactive Connectivity Establishment (ICE)](https://tools.ietf.org/html/rfc5245)
 - [STUN - Simple Traversal of User Datagram Protocol (UDP)](https://tools.ietf.org/html/rfc3489)
 - [Session Traversal Utilities for NAT (STUN)](https://tools.ietf.org/html/rfc5389)
 - [Traversal Using Relays around NAT (TURN)](https://tools.ietf.org/html/rfc5766)
 - [RTP: A Transport Protocol for Real-Time Applications](https://tools.ietf.org/html/rfc1889)
 - [Stream Control Transmission Protocol](https://tools.ietf.org/html/rfc4960)
 - [WebRTC Data Channel Protocol](https://tools.ietf.org/html/draft-jesup-rtcweb-data-protocol-04#page-3)
 - [NAT Behavior Discovery Using Session Traversal Utilities for NAT (STUN)](https://tools.ietf.org/html/rfc5780)
 - [Datagram Transport Layer Security (DTLS) Extension to Establish Keys for the Secure Real-time Transport Protocol (SRTP)](https://tools.ietf.org/html/rfc5764)
 - [TCP Candidates with Interactive Connectivity Establishment (ICE)](https://tools.ietf.org/html/rfc6544)
 - [Default Address Selection for Internet Protocol version 6 (IPv6)](https://tools.ietf.org/html/rfc3484)
 - [Test Vectors for Session Traversal Utilities for NAT](https://tools.ietf.org/html/rfc5769)

### WIP

 - [RTCICEGatherer](http://draft.ortc.org/#rtcicegatherer*) 
 - [RTCIceTransport](http://draft.ortc.org/#rtcicetransport*) 
 - [RTCDtlsTransport](http://draft.ortc.org/#rtcdtlstransport*) 
 - [RTCRtpSender](http://draft.ortc.org/#rtcrtpsender*) 
 - [RTCRtpReceiver](http://draft.ortc.org/#rtcrtpreceiver*) 
 - [RTCIceTransportController](http://draft.ortc.org/#rtcicetransportcontroller*) 
 - [RTCRtpListener](http://draft.ortc.org/#rtcrtplistener*) 
 - [RTCDtmfSender](http://draft.ortc.org/#rtcdtmfsender*) 
 - [RTCDTMFToneChangeEvent](http://draft.ortc.org/#rtcdtmftonechangeevent) 
 - [RTCDTMFSender](http://draft.ortc.org/#rtcdtmfsender2*) 
 - [RTCDataChannel](http://draft.ortc.org/#rtcdatachannel*) 
 - [RTCSctpTransport](http://draft.ortc.org/#sctp-transport*) 
 - [RTCQuicTransport](http://draft.ortc.org/#quic-transport*) 
 - [RTCQuicParameters](http://draft.ortc.org/#rtcquicparameters*) 
 - [RTCQuicRole](http://draft.ortc.org/#rtcquicrole*) 
 - [RTCQuicTransportState](http://draft.ortc.org/#rtcquictransportstate*) 
 - [RTCIdentity](http://draft.ortc.org/#rtcidentity-interface) 
 - [RTCIdentityError](http://draft.ortc.org/#rtcidentityerror-dictionary) 
 - [RTCIdentityAssertion](http://draft.ortc.org/#rtcidentityassertion-dictionary) 
 - [RTCCertificate](http://draft.ortc.org/#rtccertificate-interface) 
