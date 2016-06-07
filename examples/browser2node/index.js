var fs = require('fs');
var express = require('express');
var app = express();
var WebRTC = require('../../');
var server = require('http').Server(app);

app.use(express.static(__dirname));

var io = require('socket.io')(server);

var config = {
  iceServers: [
    {
      url: 'stun:stun.l.google.com:19302',
    },
  ],
};

io.on('connection', function(socket) {
  console.log('Peer Connected');
  
  var peer = new WebRTC.RTCPeerConnection(config);
  
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
  
  var capturers = [];
  
  peer.onaddstream = function(event) {
    var stream = event.stream || event;
    
    if (stream && stream.active) {
      console.log('Peer: add mediaStream');
      
      var audio_list = stream.getAudioTracks();
      
      audio_list.forEach(function(track) {
        capturers.push(new WebRTC.MediaCapturer(track));
      });

      var video_list = stream.getVideoTracks();
      
      video_list.forEach(function(track) {
        capturers.push(new WebRTC.MediaCapturer(track));
      });
    }
  };
  
  peer.onremovestream = function(stream) {
    console.log('Peer: remove mediaStream');
  };
});

server.listen(8080, function() {
  console.log('Open in browser: http://localhost:8080/');
});