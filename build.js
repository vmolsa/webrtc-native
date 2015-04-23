var fs = require('fs');
var os = require('os');
var sh = require('child_process').execSync;
var path = require('path')

console.log(process.env);

var root = process.cwd();
var CONFIG = process.env['BUILDTYPE'] ? process.env['BUILDTYPE'] : 'Release';
var USE_LIBWEBRTC = (os.platform() === 'darwin');
var THIRD_PARTY = path.resolve(root, 'third_party');
var DEPOT_TOOLS_REPO = 'https://chromium.googlesource.com/chromium/tools/depot_tools.git';
var LIBWEBRTC_REPO = 'https://github.com/js-platform/libwebrtc';
//var WEBRTC_BRANCH = 'branch-heads/43';

if (os.platform() == 'win32') {
  THIRD_PARTY = path.resolve(root, '..', 'third_party'); 
}

var DEPOT_TOOLS = path.resolve(THIRD_PARTY, 'depot_tools');
var WEBRTC = USE_LIBWEBRTC ? path.resolve(THIRD_PARTY, 'libwebrtc') : path.resolve(THIRD_PARTY, 'webrtc');
var WEBRTC_SRC = USE_LIBWEBRTC ? WEBRTC : path.resolve(WEBRTC, 'src');
var WEBRTC_OUT = path.resolve(WEBRTC_SRC, 'out', CONFIG);

function buildWebrtc() {
  sh('ninja -C ' + path.resolve(WEBRTC_SRC, 'out', CONFIG), {
    cwd: WEBRTC_SRC,
    env: process.env,
    stdio: 'inherit',
  });
  
  fs.linkSync(WEBRTC_OUT + '/webrtc-native.so', root + '/build/' + CONFIG + '/webrtc-native.node');
}

function syncWebrtc() { 
  process.env['PATH'] = process.env['PATH'] + path.delimiter + DEPOT_TOOLS;
  
  if (!fs.existsSync(WEBRTC)) {    
    if (USE_LIBWEBRTC) {
      sh('git clone ' + LIBWEBRTC_REPO, {
        cwd: THIRD_PARTY,
        env: process.env,
        stdio: 'inherit',
      });
      
      if (os.platform() !== 'win32') {
        sh('sh update.sh', {
          cwd: path.resolve(WEBRTC_SRC, 'chromium', 'src', 'tools', 'clang', 'scripts'),
          env: process.env,
          stdio: 'inherit',
        });
      }
    } else {
      fs.mkdirSync(WEBRTC);
      
      sh('fetch webrtc', {
        cwd: WEBRTC,
        env: process.env,
        stdio: 'inherit',
      });
    }
    
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
      process.env['GYP_DEFINES'] = 'USE_LIBWEBRTC=1';
      
      break;
    case 'win32':
      process.env['DEPOT_TOOLS_WIN_TOOLCHAIN'] = 0;
      
      break;
    case 'linux':
      if (!process.env['JAVA_HOME']) {
        process.env['JAVA_HOME'] = '/usr/lib/jvm/java';
      }
      
      break;
    default:
      break;
  }

  if (!USE_LIBWEBRTC) {    
   if (typeof(WEBRTC_BRANCH) == 'string' && WEBRTC_BRANCH !== 'origin') {
      sh('gclient sync --with_branch_heads', {
        cwd: WEBRTC,
        env: process.env,
        stdio: 'inherit',
      });
      
      sh('git checkout ' + WEBRTC_BRANCH, {
        cwd: WEBRTC_SRC,
        env: process.env,
        stdio: 'inherit',
      });
    } else {
      sh('gclient sync', {
        cwd: WEBRTC,
        env: process.env,
        stdio: 'inherit',
      });    
    }
  }
 
  process.env['GYP_DEFINES'] += ' ROOT=' + root;
  
  sh('python ' + WEBRTC_SRC + path.sep + 'webrtc' + path.sep + 'build' + path.sep + 'gyp_webrtc webrtc.gyp', {
    cwd: root,
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
