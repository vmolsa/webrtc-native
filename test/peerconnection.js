const webrtc_native = require('../dist/webrtc-native.node');

function makePair(left, right, ls, rs) {
  ls.onnegotiationneeded = () => {
    console.log(left, ' <-> ', right, '[OnNegotiationNeeded]');

    ls.createOffer().then((offer) => {
      console.log(left, ' <-> ', right, '[CreateOffer]: Done!');

      ls.setLocalDescription(offer).then(() => {
        console.log(left, ' <-> ', right, '[SetLocalDescription]: Done!');

        rs.setRemoteDescription(offer).then(() => {
          console.log(right, ' <-> ', left, '[SetRemoteDescription]: Done!');

          rs.createAnswer().then((answer) => {
            console.log(right, ' <-> ', left, '[CreateAnswer]: Done!');

            rs.setLocalDescription(answer).then(() => {
              console.log(right, ' <-> ', left, '[SetLocalDescription]: Done!');

              ls.setRemoteDescription(answer).then(() => {
                console.log(left, ' <-> ', right, '[SetRemoteDescription]: Done!');
                console.log(left, ' <-> ', right, '[OnNegotiationNeeded]: Done!');
              }).catch((error) => {
                console.log(left, ' <-> ', right, '[SetRemoteDescription]:', error);
              });
            }).catch((error) => {
              console.log(right, ' <-> ', left, '[SetLocalDescription]:', error);
            });
          }).catch((error) => {
            console.log(right, ' <-> ', left, '[CreateAnswer]:', error);
          });
        }).catch((error) => {
          console.log(right, ' <-> ', left, '[SetRemoteDescription]:', error);
        });
      }).catch((error) => {
        console.log(left, ' <-> ', right, '[SetLocalDescription]:', error);
      });
    }).catch((error) => {
      console.log(left, ' <-> ', right, '[CreateOffer]:', error)
    });
  };

  ls.onsignalingstatechange = () => {
    console.log(left, ' <-> ', right, '[OnSignalingStateChange]:', ls.signalingState);
  };

  ls.onicegatheringstatechange = () => {
    console.log(left, ' <-> ', right, '[OnIceGatheringStateChange]:', ls.iceGatheringState);
  };

  ls.oniceconnectionstatechange = () => {
    console.log(left, ' <-> ', right, '[OnIceConnectionStateChange]:', ls.iceConnectionState);
  };

  ls.onicecandidatesremoved = () => {
    console.log(left, ' <-> ', right, '[OnIceCandidatesRemoved]');
  };
 
  ls.onaddstream = (event) => {
    console.log(left, ' <-> ', right, '[OnAddStream]:', event.stream);
  };

  ls.onremovestream = (event) => {
    console.log(left, ' <-> ', right, '[OnRemoveStream]:', event.stream);
  };

  ls.ondatachannel = (event) => {
    let dataChannel = event.channel || event;

    dataChannel.onopen = () => {
      console.log(left, ' ==> ', right, '[DataChannel:', dataChannel.id, ', Label:', dataChannel.label, ']: Opened');
      dataChannel.send('PING');
    };

    dataChannel.onclose = () => {
      console.log(left, ' ==> ', right, '[DataChannel:', dataChannel.id, ', Label:', dataChannel.label, ']: Closed');
    };

    dataChannel.onerror = (error) => {
      console.log(left, ' ==> ', right, '[DataChannel:', dataChannel.id, ', Label:', dataChannel.label, ']: ', error);
    };

    dataChannel.onmessage = (event) => {
      console.log(left, ' ==> ', right, '[DataChannel:', dataChannel.id, ', Label:', dataChannel.label, ']: Message');

      if (event.data == 'PING') {
        console.log(left, ' ==> ', right, '[DataChannel:', dataChannel.id, ', Label:', dataChannel.label, ']: PING');
        dataChannel.send('PONG');
      } else if (event.data == 'PONG') {
        console.log(left, ' ==> ', right, '[DataChannel:', dataChannel.id, ', Label:', dataChannel.label, ']: PONG');
      }
    };
    
    console.log(left, ' ==> ', right, '[DataChannel:', dataChannel.id, ', Label:', dataChannel.label, ']:', dataChannel.readyState);
  };

  ls.onicecandidate = (event) => {
    console.log(left, ' <-> ', right, '[OnIceCandidate]:', event.candidate);

    rs.addIceCandidate(event.candidate).then(() => {
      console.log(right, ' <-> ', left, '[AddIceCandidate]: Done!');
    }).catch((error) => {
      console.log(right, ' <-> ', left, '[AddIceCandidate]:', error);
    });
  };
}

let alice = new webrtc_native.RTCPeerConnection();
let bob = new webrtc_native.RTCPeerConnection();

makePair('alice', 'bob', alice, bob);
makePair('bob', 'alice', bob, alice);

alice.ondatachannel(alice.createDataChannel('test_channel'));

setTimeout(() => {
  alice.close();
  bob.close();
}, 5000);