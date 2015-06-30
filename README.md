[WebRTC](http://en.wikipedia.org/wiki/WebRTC) for NodeJS

# Install

````
npm install webrtc-native --save
````

### Compatibility x64

| npm install           | Windows 8.1 | OSX | Ubuntu 14.04 |
|-----------------------|-------------|-----|--------------|
| NodeJS v0.10.x        | Yes         | Yes | Yes          |
| NodeJS v0.12.x        | Yes         | Yes | Yes          | 
| Io.js v2.3.1          | Yes         | Yes | Yes          |

### Features

| Feature               | Windows 8.1 | OSX | Ubuntu 14.04 |
|-----------------------|-------------|-----|--------------|
| RTP DataChannels      | Yes         | Yes | Yes          |
| SCTP DataChannels     | Yes         | Yes | Yes          |
| Audio Capture         | Yes         | Yes | Untested     |
| Video Capture         | Yes         | No  | Untested     |
| Screen Capture        | No          | No  | No           |
| Window Capture        | No          | No  | No           |

# API
### Usage

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

# Build

````
node-gyp rebuild
````

# Prerequisite software
## Windows

````
You must build with Visual Studio 2013 Update 4, no other versions are supported.
You must have Windows 7 x64 or later. x86 OSs are unsupported.
You must run node-gyp with administrator privileges.
````
- [Git](http://git-scm.com/)

- [Python 2.7.x](https://www.python.org/downloads/)

- [Visual Studio Community](https://www.visualstudio.com/products/visual-studio-community-vs)

- [DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)

## OS X
````
XCode 3.0 or higher
````
