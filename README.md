[WebRTC](http://en.wikipedia.org/wiki/WebRTC) for NodeJS

### Chromium

webrtc-native is using [WebRTC](http://webrtc.org/) from chromium project. code is compiled with branch [master](https://chromium.googlesource.com/external/webrtc/+/master).

# Install

````
npm install webrtc-native --save
````

### Compatibility x64

|  NodeJS / Io.JS  | Windows 8.1 | OSX | Ubuntu 14.04 |
|------------------|-------------|-----|--------------|
| IoJS-v2.3.2      | Old Release |  x  | Old Release  |
| IoJS-v3.3.1      | Old Release |  x  | Old Release  |
| NodeJS-v0.10.38  | Old Release |  x  | Old Release  |
| NodeJS-v0.12.5   | Old Release |  x  | Old Release  |
| NodeJS-v4.2.1    |      -      |  x  |      -       |
| NodeJS-v4.3.2    |      -      |  x  |      -       |
| NodeJS-v4.4.0    |      -      |  x  |      -       |
| NodeJS-v5.0.0    |      -      |  x  |      -       |
| NodeJS-v5.1.1    |      -      |  x  |      -       |
| NodeJS-v5.2.0    |      -      |  x  |      -       |
| NodeJS-v5.3.0    |      -      |  x  |      -       |
| NodeJS-v5.4.1    |      -      |  x  |      -       |
| NodeJS-v5.5.0    |      -      |  x  |      -       |
| NodeJS-v5.6.0    |      -      |  x  |      -       |
| NodeJS-v5.7.1    |      -      |  x  |      -       |
| NodeJS-v5.8.0    |      -      |  x  |      -       |

|             Features             |  Windows 8.1  |      OSX      | Ubuntu 14.04  |
|----------------------------------|---------------|---------------|---------------|
| Audio Capture                    |   Untested    |       x       |   Untested    |
| Camera Capture                   |   Untested    |       -       |   Untested    |
| Dtls-Srtp DataChannels           |   Untested    |       x       |   Untested    |
| RTP DataChannels                 |   Untested    |       x       |   Untested    |
| Video Streaming                  |   Untested    |       -       |   Untested    |
| Audio Streaming                  |   Untested    |       x       |   Untested    |
| Video Recording from MediaStream | Unimplemented | Unimplemented | Unimplemented |
| Audio Recording from MediaStream | Unimplemented | Unimplemented | Unimplemented |

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