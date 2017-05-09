const webrtc_native = require('../dist/webrtc-native.node');

let peers = [];
let channels = [];

let open_peers = 0;
let closed_peers = 0;
let open_channels = 0;
let closed_channels = 0;
let send_ping = 0;
let recv_ping = 0;
let send_pong = 0;
let recv_pong = 0;
let messages_in = 0;
let messages_out = 0;

let names = [
  'joshua',
  'alec',
  'kathy',
  //'kyle',
  //'mary',
  //'harry',
  //'candice',
  //'kelly',
  //'jesse',
  //'edward',
  //'george',
  //'albert',
  //'alice',
  //'bob',
  //'stuart',
];

let peer_count = names.length;
let expected_count = peer_count * peer_count - peer_count;

function checkExpected(msg, value, expected) {
  if (value != expected) {
    setImmediate(() => {
      throw new Error(msg + ': ' + value + ', expected: ' + expected);
    });
  }

  console.log(msg + ':', value, ' ( expected:', expected, ')');
}

function checkMessages() {
  if (messages_in != expected_count * 10 * 10 * 2 && messages_out != expected_count) {
    return false;
  }

  setTimeout(() => {
    for (let peer in peers) {
      peers[peer].close();
    }

    setTimeout(() => {
      checkExpected('Peers created', open_peers, expected_count);
      checkExpected('Peers closed', closed_peers, expected_count);
      checkExpected('Datachannels opened', open_channels, expected_count * 10);
      checkExpected('Datachannels closed', closed_channels, expected_count * 10);
      checkExpected('Messages received', messages_in, expected_count * 10 * 10 * 2);
      checkExpected('Messages sended', messages_out, expected_count * 10 * 10 * 2);
      checkExpected('Ping send count', send_ping, expected_count * 10 * 10);
      checkExpected('Ping received count', recv_ping, expected_count * 10 * 10);
      checkExpected('Pong send count', send_pong, expected_count * 10 * 10);
      checkExpected('Pong received count', recv_pong, expected_count * 10 * 10);
    }, 1000);
  }, 2000);
}

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

    if (ls.signalingState == 'closed') {
      closed_peers++;
    }
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
      open_channels++;
      console.log(left, ' ==> ', right, '[DataChannel:', dataChannel.id, ', Label:', dataChannel.label, ']: Opened');
      
      for (let index = 0; index < 10; index++) {
        dataChannel.send('PING');
        send_ping++;
        messages_out++;
      }
    };

    dataChannel.onclose = () => {
      closed_channels++;
      console.log(left, ' ==> ', right, '[DataChannel:', dataChannel.id, ', Label:', dataChannel.label, ']: Closed');
    };

    dataChannel.onerror = (error) => {
      console.log(left, ' ==> ', right, '[DataChannel:', dataChannel.id, ', Label:', dataChannel.label, ']: ', error);
    };

    dataChannel.onmessage = (event) => {
      console.log(left, ' ==> ', right, '[DataChannel:', dataChannel.id, ', Label:', dataChannel.label, ']: Message');
      messages_in++;
 
      if (event.data == 'PING') {
        recv_ping++;
        console.log(left, ' ==> ', right, '[DataChannel:', dataChannel.id, ', Label:', dataChannel.label, ']: PING');
        dataChannel.send('PONG');
        send_pong++;
        messages_out++;
      } else if (event.data == 'PONG') {
        recv_pong++;
        console.log(left, ' ==> ', right, '[DataChannel:', dataChannel.id, ', Label:', dataChannel.label, ']: PONG');
      }

      channels.push(dataChannel);
      checkMessages();
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

function CreatePeerPair(left, right) {
  let ls = new webrtc_native.RTCPeerConnection();
  let rs = new webrtc_native.RTCPeerConnection();

  makePair(left, right, ls, rs);
  makePair(right, left, rs, ls);

  open_peers += 2;

  for (let index = 0; index < 10; index++) {
    ls.ondatachannel(ls.createDataChannel(left));
  }

  peers.push(ls);
  peers.push(rs);
}

for (let x = 0; x < peer_count; x++) {
  for (let y = x + 1; y < peer_count; y++) {
    CreatePeerPair(names[x], names[y]);
  }
}