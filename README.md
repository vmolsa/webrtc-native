# WebRTC C++ library
WebRTC (Web Real-Time Communication) is a collection of communications protocols and application programming interfaces that enable real-time communication over peer-to-peer connections. This allows web browsers to not only request resources from backend servers, but also real-time information from browsers of other users.

This enables applications such as video conferencing, file transfer, chat, or desktop sharing without the need of either internal or external plugins.

WebRTC is being standardized by the World Wide Web Consortium (W3C) and the Internet Engineering Task Force (IETF). The reference implementation is released as free software under the terms of a BSD license.

WebRTC uses Real-Time Protocol to transfer audio and video.

**See also**
 - https://webrtc.org/

[![Build Status](https://travis-ci.org/vmolsa/webrtc-native.svg?branch=v2.x)](https://travis-ci.org/vmolsa/webrtc-native)

### API

````
var WebRTC = require('webrtc-native');
````

#### WebRTC.[RTCPeerConnection](https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection)

#### WebRTC.[RTCIceCandidate](https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnectionIceEvent)

#### WebRTC.[RTCSessionDescription](https://developer.mozilla.org/en-US/docs/Web/API/RTCSessionDescription)

#### WebRTC.[RTCDataChannel](https://developer.mozilla.org/en-US/docs/Web/API/RTCDataChannel)

#### WebRTC.[MediaStream](https://developer.mozilla.org/en-US/docs/Web/API/MediaStream)

#### WebRTC.[MediaStreamTrack](https://developer.mozilla.org/en-US/docs/Web/API/MediaStreamTrack)

## Licence & copyright

webrtc-native is licensed under an MIT license.
webrtc-native has built on top of libcrtc. [for more libcrtc license](https://github.com/vmolsa/libcrtc/blob/master/LICENSE)