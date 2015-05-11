var WEBRTC = require('../');

function onSuccess(stream) {
  if (stream) {
    stream.onaddtrack = function(track) {
      console.log('Track Added!');
    };

    stream.onremovetrack = function (track) {
      console.log('Track Removed!');
    };

    stream.onended = function(track) {
      console.log('Stream Ended!');
    };

    console.log('MediaStream Ended:', stream.ended);
    console.log('MediaStream ID:', stream.id);

    var audio_list = stream.getAudioTracks();
    audio_list.forEach(function (track) {
      console.log('Audio Track');

      setTimeout(function () {
        stream.removeTrack(track);
      }, 100);
     
      setTimeout(function () {
        stream.addTrack(track);
      }, 200);
    });

    var video_list = stream.getVideoTracks();
    video_list.forEach(function (track) {
      console.log('Video Track');
    });
  }
}

function onError(error) {
  throw error;
}

WEBRTC.getUserMedia({
  audio: true,
  video: true,
}, onSuccess, onError);
