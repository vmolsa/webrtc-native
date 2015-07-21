var WebRTC = require('../');

//WebRTC.setDebug(true);

var capturer = new WebRTC.MediaSource('webcam');
var renderer = new WebRTC.MediaSource('window');

setTimeout(function() {
  console.log('Closing...');
  
  capturer.end();
  renderer.end();
}, 5000);