var WebRTC = require('../../');
var WebSocketServer = require('ws').Server;
var express = require('express');
var app = express();
var server = require('http').Server(app);
WebRTC.setDebug(false);
app.use(express.static(__dirname));

var _sources = [];
var localstream  =undefined;
var initialized = false;

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
};



function sourceSelected(audioSource, videoSource) {
  
  _sources.push(videoSource);

}


WebRTC.getSources(function(sourceInfos) {
  

  for (var i = 0; i != sourceInfos.length; ++i) {
    var sourceInfo = sourceInfos[i];
    if (sourceInfo.kind === 'audio') {
      console.log(sourceInfo.id, sourceInfo.label || 'microphone');

      audioSource = sourceInfo.id;
    } else if (sourceInfo.kind === 'video') {
      console.log(sourceInfo.id, sourceInfo.label || 'camera');

      _sources.push (sourceInfo.id);
    } else {
      console.log('Some other kind of source: ', sourceInfo);
    }
  }

  WebRTC.getUserMedia(constraints, getUserMedia_successCallback, getUserMedia_errorCallback);

});


function getUserMedia_successCallback(stream) {

      localstream = stream;
      var video_list = localstream.getVideoTracks();
      
    video_list.forEach(function (track) {
      console.log('Video Track %s', JSON.stringify(track));
      track.readyState = "alive";
    });
    console.log('Sending Stream to Peer');
    initialized = true;
    init();
    }

  function getUserMedia_errorCallback(error) {
    console.log ('error %s',error);
  }

function setLocalSDP(p, callback){
    if(p.iceGatheringState != 'complete'){
        setTimeout(function(){
            setLocalSDP(p, callback);
        }, 10);
    }else{
        callback && callback();
    }
}


function init()
{
  ws = new WebSocketServer(
  {
          host: '127.0.0.1',
          port: 8181
  }); // start websocket server

  ws.on('connection', onConnect_Handler);

    
  function onConnect_Handler(connection)
  {

    var peer = new WebRTC.RTCPeerConnection(null, {audio: true, video: true});
    peer.addStream(localstream);
    peer.onicecandidate = function(event) {
      var candidate = event.candidate || event;
      connection.send(JSON.stringify({'type':'icecandidate', 'data':candidate}));
    };
    peer.createOffer(function(offer){
          peer.setLocalDescription(new WebRTC.RTCSessionDescription(offer), function(){
            setLocalSDP(peer, function(){
              var offerSDP = JSON.stringify(peer.localDescription);
              connection.send(offerSDP);
            });
          }, function(e){
              console.log (e);
          })
        });

    connection.on('message', function onWsMessage(message, flags)
    {
      console.log ("INCOMMING %s:", message);
      var jmsg = undefined;
      try
      {
        jmsg = JSON.parse(message);
      }
      catch (e)
      {
        return ;
      }
      
      if (jmsg.type=='answer')
      {
        console.log ('answer');
         var clientRemoteSDP = new WebRTC.RTCSessionDescription(jmsg);
        peer.setRemoteDescription(clientRemoteSDP, function(){
          console.log('setRemoteDescription OK');
        }, function(err){
          console.log('setRemoteDescription error', err);
        })
      }
      if (jmsg.hasOwnProperty('candidate'))
      {
         console.log ('icecandidate: jmsg: %s',JSON.stringify(jmsg));
        peer.addIceCandidate(new WebRTC.RTCIceCandidate(jmsg));
      
      }
    });


    

    peer.onnegotiationneeded = function() {
      console.log ('onnegotiationneeded');
      
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
    
  
  }

 
  server.listen(8281,  function() {
    console.log('Open in browser: http://localhost:8281/index.html');
  });
}
// */

