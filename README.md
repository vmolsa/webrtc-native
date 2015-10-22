[WebRTC](http://en.wikipedia.org/wiki/WebRTC) for NodeJS

# Install

````
npm install webrtc-native --save
````

### Compatibility x64

| nodejs / io.js | Windows 8.1 | OSX | Ubuntu 14.04 |
|----------------|-------------|-----|--------------|
| nodejs-v0.10.x |      x      |  x  |      x       |
| nodejs-v0.12.x |      x      |  x  |      x       |
| iojs-v2.3.x",  |      x      |  x  |      x       |
| iojs-v3.3.x",  |      x      |  x  |      x       |
| iojs-v4.2.1"   |      x      |  x  |      x       |

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

# Build from source

````
git clone https://github.com/vmolsa/webrtc-native
cd webrtc-native
export BUILD_WEBRTC=true
npm install
````

# Prerequisite software
## Windows

````
You must build with Visual Studio 2013 Update 4, no other versions are supported.
You must have Windows 7 x64 or later. x86 OSs are unsupported.
You must run "npm install" with administrator privileges.
````
- [Git](http://git-scm.com/)

- [Python 2.7.x](https://www.python.org/downloads/)

- [Visual Studio Community](https://www.visualstudio.com/products/visual-studio-community-vs)

- [DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)

## OS X
````
XCode 3.0 or higher
````

## Ubuntu 14.04 LTS
````
sudo apt-get install --yes build-essential python2.7 git pkg-config libnss3-dev libasound2-dev libpulse-dev libjpeg62-dev libxv-dev libgtk2.0-dev libexpat1-dev default-jdk libxtst-dev libxss-dev libpci-dev libgconf2-dev libgnome-keyring-dev libudev-dev
````

## Fedora 22
````
sudo dnf update -y
sudo dnf groupinstall -y "Development tools"
sudo dnf install -y git gcc-c++ nss-devel SAASound-devel alsa-lib-devel systemd-devel pulseaudio-libs-devel turbojpeg-devel libXv-devel gtk+-devel expat-devel java-1.8.0-openjdk-devel gtk2-devel libXtst-devel libXScrnSaver-devel redhat-lsb pciutils-devel dbus-devel pkgconfig GConf2-devel libgnome-keyring-devel
````