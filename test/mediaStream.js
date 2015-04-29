var WEBRTC = require('../');

var config = {
  iceServers: [
    {
      url: 'stun:stun.l.google.com:19302',
    },
  ],
};

var constraints = {
  audio: true,
  video: true,
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

  alice.onaddstream = function (stream) {
    if (stream) {
      console.log('Alice got mediaStream');
    }
  };

  bob.onaddstream = function(stream) {
    if(stream) {
      console.log('Bob got mediaStream');
    }
  };
}

var alice = new WEBRTC.RTCPeerConnection(config, constraints);
var bob = new WEBRTC.RTCPeerConnection(config, constraints);

P2P(alice, bob);

function onSuccess(stream) {
  if (stream) {
    console.log('Alice: new mediaStream');
    alice.addStream(stream);
  }
}

function onError(error) {
  throw error;
}

WEBRTC.getUserMedia({
  audio: true,
  video: true,
}, onSuccess, onError);
