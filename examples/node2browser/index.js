var express = require('express');
var app = express();
var server = require('http').Server(app);
var io = require('socket.io')(server);
var WebRTC = require('../../');

app.use(express.static(__dirname));

var io = require('socket.io')(server);

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
      },
    ],
  },
  video: true,
/*
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
*/
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

io.on('connection', function(socket) {
  console.log('Peer Connected');
  
  var peer = new WebRTC.RTCPeerConnection(config, constraints);
  
  socket.on('disconnect', function () {
    console.log('Peer Disconnected');
    peer.close();
  });
  
  socket.on('icecandidate', function(data) {
    if (data && data.candidate && data.sdpMid && data.sdpMLineIndex) {
      peer.addIceCandidate(new WebRTC.RTCIceCandidate(data));
    }
  });
  
  socket.on('offer', function(data) {
    peer.setRemoteDescription(new WebRTC.RTCSessionDescription(data), function() {
      peer.createAnswer(function(sdp) {
        peer.setLocalDescription(sdp, function() {
          socket.emit('answer', sdp);
        });
      });
    });
  });
  
  socket.on('answer', function(data) {
    peer.setRemoteDescription(new WebRTC.RTCSessionDescription(data));
  });
    
  peer.onicecandidate = function(event) {
    var candidate = event.candidate || event;
    socket.emit('icecandidate', candidate);
  };

  peer.onnegotiationneeded = function() {
    peer.createOffer(function(sdp) {
      peer.setLocalDescription(sdp, function() {
        socket.emit('offer', sdp);
      });
    });
  };
  
  peer.onaddstream = function(stream) {
    console.log('Peer: add mediaStream');
  };
  
  peer.onremovestream = function(stream) {
    console.log('Peer: remove mediaStream');
  };

  peer.ondatachannel = function(event) {
    var channel = event ? event.channel || event : null;

    channel.onopen = function() {
      console.log('Peer Channel opened!');
    };
    
    channel.onclose = function() {
      console.log('Peer Channel closed!');
    };    

    channel.onmessage = function(event) {
      var data = event.data;
      
      if (data == 'PING') {
        console.log('Peer: Sending PONG');
        channel.send('PONG');
      } else {
        console.log('Peer Message:', data);
        channel.send(data);
      }
    };
  };
  
  peer.ondatachannel(peer.createDataChannel('echo'));
  
  WebRTC.getUserMedia(constraints, function(stream) {
    console.log('Sending Stream to Peer');
    peer.addStream(stream);
  });
});

server.listen(8080, function() {
  console.log('Open in browser: http://localhost:8080/');
});