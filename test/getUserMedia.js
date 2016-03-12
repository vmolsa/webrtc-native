var WebRTC = require('../');

//WebRTC.setDebug(true);

function onSuccess(stream) {
  if (stream) {
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
    });

    var video_list = stream.getVideoTracks();
    
    video_list.forEach(function (track) {
      console.log('Video Track');
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
