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

  alice.onnegotiationneeded = function() {      
    alice.createOffer(function(sdp) {        
      alice.setLocalDescription(sdp, function() {
        bob.setRemoteDescription(sdp, function() { 
          bob.createAnswer(function(sdp) {
            bob.setLocalDescription(sdp, function() {
              alice.setRemoteDescription(sdp, function() {
                console.log("Alice -> Bob: Connected!");
              });
            });
          });
        });  
      });
    });
  };

  bob.onnegotiationneeded = function() {
    bob.createOffer(function(sdp) {
      bob.setLocalDescription(sdp, function() {          
        alice.setRemoteDescription(sdp, function() {            
          alice.createAnswer(function(sdp) {              
            alice.setLocalDescription(sdp, function() {                
              bob.setRemoteDescription(sdp, function() {
                console.log("Bob -> Alice: Connected!");
              });
            });
          });
        });  
      });
    });
  };

  alice.onaddstream = function(stream) {
    if (stream) {
      console.log('Alice got mediaStream');
    }
  };

  bob.onaddstream = function(stream) {
    if (stream) {
      console.log('Bob got mediaStream');
    }
  };
  
  alice.ondatachannel = function(event, callback) {
    var channel = event ? event.channel || event : null;

    if (!channel) {
      return false;
    }
    
    console.log('Alice: Got DataChannel!');

    channel.onopen = function() {
      console.log('Alice: DataChannel Open!');

      if (callback) {
        callback(channel);
      }
    };

    channel.onmessage = function(data) {
      console.log('Alice:', data);
    };

    channel.onclose = function() {
      console.log('Alice: DataChannel Closed!');
    };
  };
  
  bob.ondatachannel = function(event, callback) {
    var channel = event ? event.channel || event : null;

    if (!channel) {
      return false;
    }
    
    console.log('Bob: Got DataChannel!');
    
    channel.onopen = function() {
      console.log('Bob: DataChannel Open!');

      if (callback) {
        callback(channel);
      }
    };

    channel.onmessage = function(data) {
      console.log('Bob:', data);
      channel.send('Hello Alice!');
    };

    channel.onclose = function() {
      console.log('Bob: DataChannel Closed!');
    };
  };  
}

var config = {
  iceServers: [
    {
      url: 'stun:stun.l.google.com:19302',
    },
  ],
};

function sctpTest() {
  console.log('Running SCTP DataChannel Test');
  
  var sctpDataChannelConfig = {
    reliable: true,
    ordered: true,
  };

  var sctpDataChannelConstraints = {
    audio: false,
    video: false,
    optional: [
      {
        RtpDataChannels: false,
        DtlsSrtpKeyAgreement: true,
      },
    ],
    mandatory: {
      OfferToReceiveAudio: false,
      OfferToReceiveVideo: false,
    },
  };

  var alice = new WEBRTC.RTCPeerConnection(config, sctpDataChannelConstraints);
  var bob = new WEBRTC.RTCPeerConnection(config, sctpDataChannelConstraints);

  P2P(alice, bob);

  alice.ondatachannel(alice.createDataChannel('TestChannel', sctpDataChannelConfig), function(channel) {  
    channel.send('Hello Bob!');

    setTimeout(function() {
      alice.close();
      bob.close();
    }, 1000);
  });
}

function rtpTest() {
  console.log('Running RTP DataChannel Test');
  
  var rtpDataChannelConfig = {
    reliable: false,
    ordered: false,
  };

  var rtpDataChannelConstraints = {
    audio: false,
    video: false,
    optional: [
      {
        RtpDataChannels: true,
        DtlsSrtpKeyAgreement: false,
      },
    ],
    mandatory: {
      OfferToReceiveAudio: false,
      OfferToReceiveVideo: false,
    },
  };

  var alice = new WEBRTC.RTCPeerConnection(config, rtpDataChannelConstraints);
  var bob = new WEBRTC.RTCPeerConnection(config, rtpDataChannelConstraints);

  P2P(alice, bob);

  alice.ondatachannel(alice.createDataChannel('TestChannel', rtpDataChannelConfig), function(channel) {  
    channel.send('Hello Bob!');

    setTimeout(function() {
      alice.close();
      bob.close();
      
      setTimeout(function() {
        sctpTest();
      }, 1000);
    }, 1000);
  });
}

rtpTest();
