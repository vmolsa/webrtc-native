const path = require('path');
const os = require('os');
const fs = require('fs');

function reportMissingLinuxLibs() {
  console.log('Missing libraries: libxcb libxau libxdmcp libx11 libc libgcc libstdc++');

  if (fs.existsSync('/usr/bin/apt-get')) {
    throw new Error('Run: sudo apt-get install -qq libxcb1 libxau6 libxdmcp6 libx11-6 libc6 libgcc1 libstdc++6');
  }
  
  throw new Error('Unknown package management. Install missing packages: libxcb libxau libxdmcp libx11');
}

try {
  let module = require(path.resolve(__dirname, '..', 'dist', 'webrtc-native.node'));
} catch (error) {
  if (os.platform() == 'linux') {
    reportMissingLinuxLibs();
  } else {
    throw error;
  }
}

console.log('Module Loaded!');

