## Object RTC (ORTC) API for WebRTC

Object Real-Time Communications (ORTC) provides a powerful API for the development of WebRTC based applications. ORTC does not utilize Session Description Protocol (SDP) in the API, nor does it mandate support for the Offer/Answer state machine (though an application is free to choose SDP and Offer/Answer as an on-the-wire signaling mechanism). Instead, ORTC uses "sender", "receiver" and "transport" objects, which have "capabilities" describing what they are capable of doing, as well as "parameters" which define what they are configured to do. "Tracks" are encoded by senders and sent over transports, then decoded by receivers while "data channels" are sent over transports directly.

In a Javascript application utilizing the ORTC API, the relationship between the application and the objects, as well as between the objects themselves is shown below. Horizontal or slanted arrows denote the flow of media or data, whereas vertical arrows denote interactions via methods and events.

![ORTC](http://draft.ortc.org/images/quic.svg)

### API

[Documentation](https://rawgit.com/vmolsa/webrtc-native/ortc/doc/modules/_ortc_.html)
