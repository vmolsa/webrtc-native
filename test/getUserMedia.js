var WebRTC = require('../');

//WebRTC.setDebug(true);

function dumpTrack(track) {
  console.log('MediaStreamTrack Enabled:', track.enabled);
  console.log('MediaStreamTrack Id:', track.id);
  console.log('MediaStreamTrack Kind:', track.kind);
  console.log('MediaStreamTrack Label:', track.label);
  console.log('MediaStreamTrack Muted:', track.muted);
  console.log('MediaStreamTrack ReadyState:', track.readyState);
  console.log('MediaStreamTrack Remote:', track.remote);
}

function onSuccess(stream) {
  if (stream && stream.active) {
    stream.onaddtrack = function(track) {
      console.log('Track Added!');
    };

    stream.onremovetrack = function (track) {
      console.log('Track Removed!');
    };

    console.log('MediaStream Active:', stream.active);
    console.log('MediaStream Ended:', stream.ended);
    console.log('MediaStream ID:', stream.id);

    var audio_list = stream.getAudioTracks();
    
    audio_list.forEach(function (track) {
      console.log('Audio Track');
      dumpTrack(track);
    });

    var video_list = stream.getVideoTracks();
    
    video_list.forEach(function (track) {
      console.log('Video Track');
      dumpTrack(track);
    });
    
    setTimeout(function() {
      console.log('Closing...');
    }, 5000);
  }
}

function onError(error) {
  throw error;
}

WebRTC.getUserMedia({
  audio: true,
  video: true,
}, onSuccess, onError);
