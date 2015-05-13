var fs = require('fs');
var os = require('os');
var sh = require('child_process').execSync;
var path = require('path');

var ROOT = process.cwd();
var NODEJS = '.';
var SYNC = false;

if (process.env['SKIP_SYNC'] == 1 || process.env['SKIP_SYNC'] == 0) {
  SYNC = (process.env['SKIP_SYNC'] == 1) ? false : true;
}

if (os.platform() == 'win32') {
  process.chdir(path.resolve(ROOT, '..'));
  ROOT = process.cwd();
  NODEJS = path.resolve(process.env['USERPROFILE'], '.node-gyp', process.version.substring(1));
} else {
  NODEJS = path.resolve(process.env['HOME'], '.node-gyp', process.version.substring(1));
}

if (fs.existsSync(ROOT + path.sep + 'nodejs.gypi')) {
  fs.unlinkSync(ROOT + path.sep + 'nodejs.gypi');
}

if (!fs.existsSync(ROOT + path.sep + 'build' + path.sep + 'config.gypi')) {
  throw new Error('Run node-gyp rebuild instead of node build.js');
}

fs.linkSync(NODEJS + path.sep + 'common.gypi', ROOT + path.sep + 'nodejs.gypi');

var CONFIG = 'Release';

if (fs.existsSync(ROOT + path.sep + 'build' + path.sep + 'Debug')) {
  CONFIG = 'Debug';
}

var TESTS = (CONFIG == 'Debug') ? true : false;

if (process.env['ENABLE_TESTS'] == 1 || process.env['ENABLE_TESTS'] == 0) {
  TESTS = (process.env['ENABLE_TESTS'] == 1) ? true : false;
}

var THIRD_PARTY = path.resolve(ROOT, 'third_party');
var DEPOT_TOOLS_REPO = 'https://chromium.googlesource.com/chromium/tools/depot_tools.git';
var DEPOT_TOOLS = path.resolve(THIRD_PARTY, 'depot_tools');
var WEBRTC = path.resolve(THIRD_PARTY, 'webrtc');
var WEBRTC_SRC = path.resolve(WEBRTC, 'src');
var WEBRTC_OUT = path.resolve(WEBRTC_SRC, 'out', CONFIG);

if (os.platform() == 'win32' && process.arch == 'x64') {
  WEBRTC_OUT = path.resolve(WEBRTC_SRC, 'out', CONFIG + '_x64');
}

if (!SYNC) {
  if (fs.existsSync(THIRD_PARTY + path.sep + 'webrtc_sync')) {
    var stat = fs.statSync(THIRD_PARTY + path.sep + 'webrtc_sync');

    if (((new Date().getTime()) - stat.mtime.getTime()) > 86400000) {
      SYNC = true;
    }
  } else {
    SYNC = true;
  }
}

process.env['GYP_DEFINES'] = process.env['GYP_DEFINES'] ? process.env['GYP_DEFINES'] : '';

if (process.env['GYP_DEFINES'] !== '') {
  console.log('GYP_DEFINES =', process.env['GYP_DEFINES']);
}

if (!fs.existsSync(THIRD_PARTY)) {
  fs.mkdirSync(THIRD_PARTY);
}

if (!fs.existsSync(DEPOT_TOOLS)) {
  sh('git clone ' + DEPOT_TOOLS_REPO, {
    cwd: THIRD_PARTY,
    env: process.env,
    stdio: 'inherit',
  });
}

process.env['PATH'] = process.env['PATH'] + path.delimiter + DEPOT_TOOLS;

if (!fs.existsSync(WEBRTC) || !fs.existsSync(WEBRTC_SRC)) {
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

process.env['GYP_DEFINES'] += ' third_party=' + THIRD_PARTY;
process.env['GYP_DEFINES'] += ' target_arch=' + process.arch;
process.env['GYP_DEFINES'] += ' host_arch=' + process.arch;
process.env['GYP_DEFINES'] += ' nodedir=' + NODEJS;
process.env['GYP_DEFINES'] += ' configuration=' + CONFIG;
process.env['GYP_DEFINES'] += TESTS ? ' build_tests=1' : ' build_tests=0';

switch (os.platform()) {
  case 'darwin':
    process.env['GYP_DEFINES'] += ' clang=1';

    break;
  case 'win32':
    process.env['DEPOT_TOOLS_WIN_TOOLCHAIN'] = 0;

    break;
  case 'linux':
    if (!process.env['JAVA_HOME']) {
      if (fs.existsSync('/usr/lib/jvm/java')) {
        process.env['JAVA_HOME'] = '/usr/lib/jvm/java';
      } else {
        process.env['JAVA_HOME'] = '/usr/lib/jvm/default-java';
      }
    }

    break;
  default:
    break;
}

console.log('target_arch =', process.arch);
console.log('host_arch =', process.arch);
console.log('configuration =', CONFIG);
console.log('SKIP_SYNC =', SYNC ? false : true);
console.log('ENABLE_TESTS =', TESTS ? true : false);
console.log('Enable / Disable Options: ');

if (os.platform() == 'win32') {
  console.log('set SKIP_SYNC=1');
  console.log('set SKIP_SYNC=0');
  console.log('set ENABLE_TESTS=1');
  console.log('set ENABLE_TESTS=0');
} else {
  console.log('export SKIP_SYNC=1');
  console.log('export SKIP_SYNC=0');
  console.log('export ENABLE_TESTS=1');
  console.log('export ENABLE_TESTS=0');
}

if (SYNC) {
  try {
    sh('gclient sync', {
      cwd: WEBRTC,
      env: process.env,
      stdio: 'inherit',
    });
  } catch (error) {
    // TODO(): src/build/landmines.py in third_party\webrtc\src\chromium is causing error on win32?
    
    if (os.platform() == 'win32') {
      sh('gclient sync', {
        cwd: WEBRTC,
        env: process.env,
        stdio: 'inherit',
      });
    } else {
      throw error;
    }
  }
  
  fs.closeSync(fs.openSync(THIRD_PARTY + path.sep + 'webrtc_sync', 'w'));
}

if (fs.existsSync(WEBRTC_SRC)) {
  console.log('Building WebRTC Node Module');

  sh('python ' + WEBRTC_SRC + path.sep + 'webrtc' + path.sep + 'build' + path.sep + 'gyp_webrtc webrtc.gyp', {
    cwd: ROOT,
    env: process.env,
    stdio: 'inherit',
  });

  sh('ninja -C ' + WEBRTC_OUT, {
    cwd: WEBRTC_SRC,
    env: process.env,
    stdio: 'inherit',
  });

  fs.linkSync(WEBRTC_OUT + path.sep + 'webrtc-native.node', ROOT + path.sep + 'build' + path.sep + CONFIG + path.sep + 'webrtc-native.node');
  
  if (TESTS) {
    var peerconnection = 'webrtc-gtest';
    
    if (os.platform() == 'win32') {
      peerconnection += '.exe';
    }
  
    sh(WEBRTC_OUT + path.sep + peerconnection, {
      cwd: WEBRTC_SRC,
      env: process.env,
      stdio: 'inherit',
    });
  }
  
  console.log('Done! :)');
}
