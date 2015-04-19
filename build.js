var fs = require('fs');
var os = require('os');
var sh = require('child_process').execSync;
var root = process.cwd();

var DEPOT_TOOLS = 'https://chromium.googlesource.com/chromium/tools/depot_tools.git';

function buildWebrtc() {
  sh('ninja -C out/Release', {
    cwd: root + '/third_party/webrtc/src/',
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
  process.env['PATH'] = root + '/third_party/depot_tools:' + process.env['PATH'];
  
  if (!fs.existsSync(root + '/third_party/webrtc')) {
    fs.mkdirSync(root + './third_party/webrtc');
    
    sh('fetch webrtc', {
      cwd: root + '/third_party/webrtc/',
      env: process.env,
      stdio: 'inherit',
    });
    
    if (os.platform() == 'linux') {
      sh('./build/install-build-deps.sh', {
        cwd: root + '/third_party/webrtc/src/',
        env: process.env,
        stdio: 'inherit',
      });
    }
  }
  
  if (os.platform() == 'darwin') {
    process.env['GYP_DEFINES'] = 'clang=1';
  }
  
  sh('gclient sync', {
    cwd: root + '/third_party/webrtc/',
    env: process.env,
    stdio: 'inherit',
  });
  
  buildWebrtc();
}

function checkDepotTools() {
  if (!fs.existsSync(root + '/third_party/depot_tools')) {
    sh('git clone ' + DEPOT_TOOLS, {
      cwd: root + '/third_party/',
      env: process.env,
      stdio: 'inherit',
    });
  }
  
  if (os.platform() == 'win32') {
    process.env['DEPOT_TOOLS_WIN_TOOLCHAIN'] = 0;
  }
  
  syncWebrtc();
}

if (!fs.existsSync(root + '/third_party')) {
  fs.mkdirSync(root + './third_party');
}

checkDepotTools();
