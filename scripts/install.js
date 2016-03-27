var fs = require('fs');
var os = require('os');
var spawn = require('child_process').spawn;
var path = require('path');
var request = require('request');

var PLATFORM = os.platform();
var ROOT = path.resolve(__dirname, '..');
var ARCH = os.arch();
var URL = 'http://cide.cc:8080/webrtc/';
var NODEVER = process.version.split('.');
var PACKAGE = require(path.resolve(ROOT, 'package.json'));

NODEVER[2] = 'x';
NODEVER = NODEVER.join('.');

URL += 'webrtc-' + PACKAGE.version + '-' + PLATFORM + '-' + ARCH + '-' + NODEVER + '.node';

function build() {
  console.log('Building module...');
  
  var nodegyp = path.resolve(ROOT, 'node_modules', 'node-gyp', 'bin', 'node-gyp.js');
  
  if (!fs.existsSync(nodegyp)) {
    nodegyp = path.resolve(ROOT, '..', 'node-gyp', 'bin', 'node-gyp.js');
    
    if (!fs.existsSync(nodegyp)) {      
      throw new Error('Build Failed. Please follow instructions from https://github.com/vmolsa/webrtc-native#build-from-source');
    }
  }
  
  var res = spawn('node', [ nodegyp, 'rebuild' ], {
    cwd: ROOT,
    env: process.env,
    stdio: 'inherit',
  });
  
  res.on('error', function(error) {
    var res = spawn('iojs', [ nodegyp, 'rebuild' ], {
      cwd: ROOT,
      env: process.env,
      stdio: 'inherit',
    });
  });
}

function test() {
  console.log('Loading module...');
  
  try {
    var WebRTC = require(path.resolve(ROOT, 'build', 'Release', 'webrtc.node'));
    
    setTimeout(function() {
      console.log('Done! :)');
    }, 200);
  } catch (ignored) {
    if (PLATFORM == 'win32') {
      throw new Error('prebuilt module not working. See the instructions from https://github.com/vmolsa/webrtc-native#build-from-source for building module from source.');
    } else {
      throw new Error('prebuilt module not working. See the instructions from https://github.com/vmolsa/webrtc-native#build-from-source for building module from source.');
    }
  }
}

if (process.env['BUILD_WEBRTC'] == 'true') {
  build();
} else {
  console.log('Downloading module...');

  if (!fs.existsSync(path.resolve(ROOT, 'build', 'Release'))) {
    if (!fs.existsSync(path.resolve(ROOT, 'build'))) {
      fs.mkdirSync(path.resolve(ROOT, 'build'));
    }
    
    fs.mkdirSync(path.resolve(ROOT, 'build', 'Release'));
  }

  request.get(URL, function (error, response, body) {
    if (!error && response.statusCode == 200) {
      setTimeout(test, 200);
    } else {
      if (os.platform() == 'win32') {
        throw new Error('prebuilt module not found. See the instructions from https://github.com/vmolsa/webrtc-native#build-from-source for building module from source.');
      } else {
        throw new Error('prebuilt module not found. See the instructions from https://github.com/vmolsa/webrtc-native#build-from-source for building module from source.');
      }
    }
  }).pipe(fs.createWriteStream(path.resolve(ROOT, 'build', 'Release', 'webrtc.node')));
}
