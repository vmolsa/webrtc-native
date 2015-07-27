var fs = require('fs');
var os = require('os');
var spawn = require('child_process').spawn;
var path = require('path');
var request = require('request');

var ROOT = process.cwd();
var ARCH = os.arch();
var NODEVER = process.version.split('.');
var PKGNAME = require(ROOT + path.sep + 'package.json').name;
var URL = 'http://cide.cc:8080/webrtc/';

function build() {
  console.log('Building module...');
 
  var pangyp = path.resolve(ROOT, 'node_modules', 'pangyp', 'bin', 'node-gyp.js');
   
  var res = spawn('node', [ pangyp, 'rebuild' ], {
    cwd: ROOT,
    env: process.env,
    stdio: 'inherit',
  });
}

function test() {
  console.log('Loading module...');
  
  try {
    var WebRTC = require('build/Release/webrtc.node');
    
    console.log('Done! :)');
  } catch (ignored) {
    if (os.platform() == 'win32') {
      throw new Error('prebuilt module not working. "set BUILD_WEBRTC=true" and rerun "npm install" to begin to build from source.');
    } else {
      throw new Error('prebuilt module not working. "export BUILD_WEBRTC=true" and rerun "npm install" to begin to build from source.');
    }
    
    console.log('Install Failed! :(');
  }
}

function prebuilt(PLATFORM) {
  NODEVER[2] = 'x';
  NODEVER = NODEVER.join('.');
  
  URL += PLATFORM + '/';
  URL += ARCH + '/';
  URL += NODEVER + '/';
  URL += PKGNAME + '.node';
  
  if (process.env['BUILD_WEBRTC'] == 'true') {
    return build();
  }
  
  console.log('Downloading module...');
  
  if (!fs.existsSync(path.resolve(ROOT, 'build', 'Release'))) {
    if (!fs.existsSync(path.resolve(ROOT, 'build'))) {
      fs.mkdirSync(path.resolve(ROOT, 'build'));
    }
    
    fs.mkdirSync(path.resolve(ROOT, 'build', 'Release'));
  }
  
  request.get(URL, function (error, response, body) {
    if (!error && response.statusCode == 200) {
      setTimeout(test, 1000);
    } else {
      if (os.platform() == 'win32') {
        throw new Error('prebuilt module not found. "set BUILD_WEBRTC=true" and rerun "npm install" to begin to build from source.');
      } else {
        throw new Error('prebuilt module not found. "export BUILD_WEBRTC=true" and rerun "npm install" to begin to build from source.');
      }
      
      process.exit(1);
    }
  }).pipe(fs.createWriteStream(path.resolve(ROOT, 'build', 'Release', 'webrtc.node')));  
}

if (os.platform() == 'linux') {
  var lsb = require('lsb-release');
  
  lsb(function(error, data) {
    if (error) {
      throw error;
      process.exit(1);
    }
    
    prebuilt(data.distributorID + '-' + data.release);
  });
} else {
  prebuilt(os.platform());
}
  