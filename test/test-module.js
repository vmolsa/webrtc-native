const webrtc_native = require('../build/Release/webrtc.node');

webrtc_native.runTest(() => {
  console.log('Done!');
});