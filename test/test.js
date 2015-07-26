var WebRTC = require('../');

WebRTC.setDebug(true);

var renderer = new WebRTC.MediaSource('window');
var capturer = new WebRTC.MediaSource('webcam');

capturer.onend = function() {
  renderer.end();
};

capturer.ondata = function(data) {
  renderer.write(data);
};

setTimeout(function() {
  console.log('Closing...');
  
  capturer.end();
}, 5000);