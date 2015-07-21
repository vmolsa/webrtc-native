var WebRTC = require('../');

//WebRTC.setDebug(true);

function onSuccess(stream) {
  console.log(stream.getAudioTracks());
  console.log(stream.getVideoTracks());
  
  var capturer = new WebRTC.MediaSource('mediastream', {
    stream: stream
  });
  
  capturer.ondata = function(frame) {
    console.log(frame);
    console.log(frame.data.byteLength);
  };
  
  setTimeout(function() {
    console.log('Closing...');
    capturer.end();
  }, 5000);
}

var constraints = {
  audio: true,
  video: true,
/*
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
  optional: [
    {
      DtlsSrtpKeyAgreement: true,
    },
  ],
  mandatory: {
    OfferToReceiveAudio: true,
    OfferToReceiveVideo: true,
  },
*/
};

WebRTC.getUserMedia(constraints, onSuccess);