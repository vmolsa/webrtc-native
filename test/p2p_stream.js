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
      DtlsSrtpKeyAgreement: false,
    },
  ],
  mandatory: {
    OfferToReceiveAudio: true,
    OfferToReceiveVideo: true,
  },
};

var alice = new WEBRTC.RTCPeerConnection(config, constraints);
var bob = new WEBRTC.RTCPeerConnection(config, constraints);

alice.onicecandidate = function (event) {
  var candidate = event.candidate || event;
  bob.addIceCandidate(candidate);
};

bob.onicecandidate = function (event) {
  var candidate = event.candidate || event;
  alice.addIceCandidate(candidate);
};

alice.onnegotiationneeded = function () {
  alice.createOffer(function (sdp) {
    alice.setLocalDescription(sdp, function () {
      bob.setRemoteDescription(sdp, function () {
        bob.createAnswer(function (sdp) {
          bob.setLocalDescription(sdp, function () {
            alice.setRemoteDescription(sdp, function () {

            });
          });
        });
      });
    });
  });
};

bob.onnegotiationneeded = function () {
  bob.createOffer(function (sdp) {
    bob.setLocalDescription(sdp, function () {
      alice.setRemoteDescription(sdp, function () {
        alice.createAnswer(function (sdp) {
          alice.setLocalDescription(sdp, function () {
            bob.setRemoteDescription(sdp, function () {

            });
          });
        });
      });
    });
  });
};

WEBRTC.getSources(function (inputs) {
  console.log(inputs);

  WEBRTC.getUserMedia({
    audio: true,
    video: true,
  }, function (stream) {
    alice.addStream(stream);
  });
});

