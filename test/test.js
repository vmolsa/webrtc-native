var WebRTC = require('../');

//WebRTC.setDebug(true);

var renderer = new WebRTC.MediaSource('window');
var capturer = new WebRTC.MediaSource('webcam');

capturer.connect(renderer);

setTimeout(function() {
  console.log('Closing...');
  
  capturer.end();
}, 10000);