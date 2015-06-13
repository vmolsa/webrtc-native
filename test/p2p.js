var WEBRTC = require('../');

function P2P(alice, bob) {
  alice.onicecandidate = function(event) {
    var candidate = event.candidate || event;
    bob.addIceCandidate(candidate);
  };

  bob.onicecandidate = function(event) {
    var candidate = event.candidate || event;
    alice.addIceCandidate(candidate);
  };

  alice.onnegotiationneeded = function(callback) {      
    alice.createOffer(function(sdp) {        
      alice.setLocalDescription(sdp, function() {
        bob.setRemoteDescription(sdp, function() { 
          bob.createAnswer(function(sdp) {
            bob.setLocalDescription(sdp, function() {
              alice.setRemoteDescription(sdp, function() {
                console.log("Alice -> Bob: Connected!");
                
                if (callback) {
                  callback();
                }
              });
            });
          });
        });  
      });
    });
  };

  bob.onnegotiationneeded = function(callback) {
    bob.createOffer(function(sdp) {
      bob.setLocalDescription(sdp, function() {          
        alice.setRemoteDescription(sdp, function() {            
          alice.createAnswer(function(sdp) {              
            alice.setLocalDescription(sdp, function() {                
              bob.setRemoteDescription(sdp, function() {
                console.log("Bob -> Alice: Connected!");
                
                if (callback) {
                  callback();
                }
              });
            });
          });
        });  
      });
    });
  };
}

var config = {
  iceServers: [
    {
      url: 'stun:stun.l.google.com:19302',
    },
  ],
};

var constraints = {
  audio: {
    optional: [
      {
/*
        googEchoCancellation: true,
        googEchoCancellation2: true,
        googDAEchoCancellation: true,
        googAutoGainControl: true,
        googAutoGainControl2: true,
        googNoiseSuppression: true,
        googNoiseSuppression2: true,
        googHighpassFilter: true,
        googTypingNoiseDetection: true,
        googAudioMirroring: true,
*/
      },
    ],
  },
  video: {
    optional: [
      {
        minAspectRatio: 1.333,
        maxAspectRatio: 1.778,
        maxWidth: 1920,
        minWidth: 320,
        maxHeight: 1080,
        minHeight: 180,
        maxFrameRate: 60,
        minFrameRate: 30,
      },
    ],
  },
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

var alice = new WEBRTC.RTCPeerConnection(config, constraints);
var bob = new WEBRTC.RTCPeerConnection(config, constraints);

P2P(alice, bob);

alice.onnegotiationneeded(function() {
  console.log('Done! :)');
});
