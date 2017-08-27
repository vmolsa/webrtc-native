[WebRTC](http://en.wikipedia.org/wiki/WebRTC) for NodeJS

### Chromium

[![Greenkeeper badge](https://badges.greenkeeper.io/vmolsa/webrtc-native.svg)](https://greenkeeper.io/)

webrtc-native is using [WebRTC](http://webrtc.org/) from chromium project. code is compiled with branch [50](https://chromium.googlesource.com/external/webrtc/+/branch-heads/50).

### Usage

For installing or building module from source go to page [Getting Started](https://github.com/vmolsa/webrtc-native/wiki/Getting-started)

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

#### WebRTC.[getUserMedia](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/getUserMedia)

#### WebRTC.[getSources](http://simpl.info/getusermedia/sources/index.html)

- Returns array of available device inputs

#### WebRTC.RTCGarbageCollect()

- Notify V8 Engine to attempt to free memory.

#### WebRTC.setDebug(boolean)

- Enable / Disable WebRTC log messages
