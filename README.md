WebRTC for NodeJS

This module is working only with latest NodeJS version. 
If you are using another version of NodeJS then you should use [node-webrtc](https://github.com/js-platform/node-webrtc) project.

# API

[RTCPeerConnection](https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection)

[RTCDataChannel](https://developer.mozilla.org/en-US/docs/Web/API/RTCDataChannel)

[MediaStream](https://developer.mozilla.org/en-US/docs/Web/API/MediaStream)

[MediaStreamTrack](https://developer.mozilla.org/en-US/docs/Web/API/MediaStreamTrack)

[getUserMedia](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/getUserMedia)

````
var WebRTC = require('webrtc-native');

var RTCPeerConnection = WebRTC.RTCPeerConnection          // Partial Support
var RTCIceCandidate = WebRTC.RTCIceCandidate              
var RTCSessionDescription = WebRTC.RTCSessionDescription
var MediaStream = WebRTC.MediaStream                      // Not Implemented
var MediaStreamTrack = WebRTC.MediaStreamTrack            // Not Implemented
var getUserMedia = WebRTC.getUserMedia                    // Not Implemented
var getSources = WebRTC.getSources                        // Not Implemented
````

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
[Git](http://git-scm.com/)

[Python 2.7.x](https://www.python.org/downloads/)

[Visual Studio Community](https://www.visualstudio.com/products/visual-studio-community-vs)

[DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)

## OS X
````
XCode 3.0 or higher
````
