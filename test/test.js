var WebRTC = require('../');

//WebRTC.setDebug(true);

var capturer = new WebRTC.MediaSource("device/webcam", function() {
  
});

setTimeout(function() {
  console.log('Closing...');
}, 5000);