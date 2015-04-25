try {
  module.exports = require('./build/Release/webrtc-native.node');
} catch (ignored) {
  module.exports = require('./build/Debug/webrtc-native.node');
}
