var WEBRTC = require('../');

var config = {
  iceServers: [
    {
      url: 'stun:stun.l.google.com:19302',
    },
  ],
};

var constraints = {
  optional: [
    {
      DtlsSrtpKeyAgreement: true,
    },
  ],
  mandatory: {
    OfferToReceiveAudio: true,
    OfferToReceiveVideo: true,
  },
};

var totalChannels = 0;

function RunTest(done) {
  function P2P(alice, bob) {
    function showState() {
      console.log('Alice State:', alice.signalingState);
      console.log('Bob State:', bob.signalingState);
    }
  
    alice.onicecandidate = function(event) {
      var candidate = event.candidate || event;
      bob.addIceCandidate(candidate);
    };

    bob.onicecandidate = function(event) {
      var candidate = event.candidate || event;
      alice.addIceCandidate(candidate);
    };

    alice.onnegotiationneeded = function() {
      showState();
      
      alice.createOffer(function(sdp) {
        showState();
        
        alice.setLocalDescription(sdp, function() {
          showState();
          
          bob.setRemoteDescription(sdp, function() { 
            showState();
            
            bob.createAnswer(function(sdp) {
              showState();
              
              bob.setLocalDescription(sdp, function() {
                showState();
                
                alice.setRemoteDescription(sdp, function() {
                  showState();
                });
              });
            });
          });  
        });
      });
    };

    bob.onnegotiationneeded = function() {
      showState();
      
      bob.createOffer(function(sdp) {
        showState();
        
        bob.setLocalDescription(sdp, function() {
          showState();
          
          alice.setRemoteDescription(sdp, function() {
            showState();
            
            alice.createAnswer(function(sdp) {
              showState();
              
              alice.setLocalDescription(sdp, function() {
                showState();
                
                bob.setRemoteDescription(sdp, function() {
                  showState();
                });
              });
            });
          });  
        });
      });
    };

    alice.ondatachannel = function(event, callback) {
      var channel = event.channel || event;

      channel.onopen = function() {
        totalChannels += 1;
        console.log('Alice: DataChannel Open!');

        if (callback) {
          callback(channel);
        }
      };

      channel.onmessage = function(data) {
        console.log('Alice:', data);

        WEBRTC.RTCGarbageCollect();
      };

      channel.onclose = function() {
        console.log('Alice: DataChannel Closed!');
        channel = null;
      };
    };

    bob.ondatachannel = function(event, callback) {
      var channel = event.channel || event;

      channel.onopen = function() {
        totalChannels += 1;
        console.log('Bob: DataChannel Open!');

        if (callback) {
          callback(channel);
        }
      };

      channel.onmessage = function(data) {
        if (typeof(data) == 'string') {
          console.log('Bob:', data);
        } else {
          var buf = new Buffer(new Uint8Array(data));
          if (buf.toString('utf8') === 'HELLO') {
            console.log('TEST SUCCESS');
          } else {
            console.log('TEST FAILED');
          }
        }

        channel.send('Hello World!');

        WEBRTC.RTCGarbageCollect();
      };

      channel.onclose = function() {
        console.log('Bob: DataChannel Closed!');
        channel = null;
      };
    };
  }

  var peers = [];

  function createPeers() {
    for (var instances = 0; instances < 1; instances++) {
      var alice = new WEBRTC.RTCPeerConnection(config, constraints);
      var bob = new WEBRTC.RTCPeerConnection(config, constraints);

      P2P(alice, bob);
      
      peers.push(alice);
      peers.push(bob);

      for (var channels = 0; channels < 1; channels++) {
        alice.ondatachannel(alice.createDataChannel('TestChannel'), function(channel) {
          var buf = new Buffer(5);
          buf.write('HELLO');

          channel.send(new Uint8Array(buf));
        });
      }
    }
  }

  createPeers();

  setTimeout(function() {
    while (peers.length) {
      var peer = peers[0];
      peers.splice(0, 1);
      peer.close();
    }    
    setTimeout(function() {
      console.log('Total count of channels:', totalChannels);
    }, 5000);    
  }, 1000);
}

RunTest(function() {
  WEBRTC.RTCGarbageCollect();
});
