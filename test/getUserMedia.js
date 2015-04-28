var WEBRTC = require('../');

function onSuccess(stream) {
  if (stream) {
    console.log('new MediaStream');
  }
}

function onError(error) {
  throw error;
}

WEBRTC.getUserMedia({
  audio: true,
  video: true,
}, onSuccess, onError);
