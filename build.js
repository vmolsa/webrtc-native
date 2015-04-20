var fs = require('fs');
var os = require('os');
var sh = require('child_process').execSync;
var path = require('path')

var root = process.cwd();
var THIRD_PARTY = path.resolve(root, 'third_party'); 
var DEPOT_TOOLS_REPO = 'https://chromium.googlesource.com/chromium/tools/depot_tools.git';

if (os.platform() == 'win32') {
  THIRD_PARTY = path.resolve(root, '..', 'third_party'); 
}

var DEPOT_TOOLS = path.resolve(THIRD_PARTY, 'depot_tools');
var WEBRTC = path.resolve(THIRD_PARTY, 'webrtc');
var WEBRTC_SRC = path.resolve(WEBRTC, 'src');

function buildWebrtc() {
  sh('ninja -C out/Release', {
    cwd: WEBRTC_SRC,
    env: process.env,
    stdio: 'inherit',
  });
  
  sh('node gen_libs.js', {
    cwd: root,
    env: process.env,
    stdio: 'inherit',
  });
}

function syncWebrtc() { 
  process.env['PATH'] = process.env['PATH'] + path.delimiter + DEPOT_TOOLS;
  
  if (!fs.existsSync(WEBRTC)) {
    fs.mkdirSync(WEBRTC);
    
    sh('fetch webrtc', {
      cwd: WEBRTC,
      env: process.env,
      stdio: 'inherit',
    });
    
    if (os.platform() == 'linux') {
      sh('./build/install-build-deps.sh', {
        cwd: WEBRTC_SRC,
        env: process.env,
        stdio: 'inherit',
      });
    }
  }
  
  switch (os.platform()) {
    case 'darwin':
      process.env['GYP_DEFINES'] = 'clang=1';
      
      break;
    case 'win32':
      process.env['DEPOT_TOOLS_WIN_TOOLCHAIN'] = 0;
      
      break;
    case 'linux':
      process.env['JAVA_HOME'] = '/usr/lib/jvm/java';
      
      break;
    default:
      break;
  }
  
  sh('gclient sync', {
    cwd: WEBRTC,
    env: process.env,
    stdio: 'inherit',
  });
  
  buildWebrtc();
}

function checkDepotTools() {
  if (!fs.existsSync(DEPOT_TOOLS)) {
    sh('git clone ' + DEPOT_TOOLS_REPO, {
      cwd: THIRD_PARTY,
      env: process.env,
      stdio: 'inherit',
    });
  }
  
  syncWebrtc();
}

if (!fs.existsSync(THIRD_PARTY)) {
  fs.mkdirSync(THIRD_PARTY);
}

checkDepotTools();
