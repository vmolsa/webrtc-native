var WEBRTC = require('../');

function onSuccess(stream) {
  if (stream) {
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
  }
}

function onError(error) {
  throw error;
}

WEBRTC.getUserMedia({
  audio: true,
  video: true,
}, onSuccess, onError);
