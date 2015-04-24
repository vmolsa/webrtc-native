var fs = require('fs');
var os = require('os');
var sh = require('child_process').execSync;
var path = require('path');

var ROOT = process.cwd();
var NODEJS = '.';

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

fs.linkSync(NODEJS + path.sep + 'common.gypi', ROOT + path.sep + 'nodejs.gypi');

var CONFIG = process.env['BUILDTYPE'] ? process.env['BUILDTYPE'] : 'Release';
var BUILD_WEBRTC_SHARED = (os.platform() == 'win32');
var USE_LIBWEBRTC = (os.platform() == 'darwin');
var THIRD_PARTY = path.resolve(ROOT, 'third_party');
var DEPOT_TOOLS_REPO = 'https://chromium.googlesource.com/chromium/tools/depot_tools.git';
var LIBWEBRTC_REPO = 'https://github.com/js-platform/libwebrtc';

var DEPOT_TOOLS = path.resolve(THIRD_PARTY, 'depot_tools');
var WEBRTC = USE_LIBWEBRTC ? path.resolve(THIRD_PARTY, 'libwebrtc') : path.resolve(THIRD_PARTY, 'webrtc');
var WEBRTC_SRC = USE_LIBWEBRTC ? WEBRTC : path.resolve(WEBRTC, 'src');
var WEBRTC_OUT = path.resolve(WEBRTC_SRC, 'out', CONFIG);
var GYP_PATH = path.resolve(WEBRTC_SRC, 'tools', 'gyp', (os.platform() == 'win32' ? 'gyp.bat' : 'gyp'));

if (process.env['BUILD_WEBRTC_SHARED'] == 1 || process.env['BUILD_WEBRTC_SHARED'] == 0) {
  BUILD_WEBRTC_SHARED = process.env['BUILD_WEBRTC_SHARED'] == 1 ? true : false;
}

if (process.env['USE_LIBWEBRTC'] == 1 || process.env['USE_LIBWEBRTC'] == 0) {
  USE_LIBWEBRTC = process.env['USE_LIBWEBRTC'] == 1 ? true : false;
}

process.env['GYP_DEFINES'] = process.env['GYP_DEFINES'] ? process.env['GYP_DEFINES'] : '';

console.log('Using Configuration:', 'BUILDTYPE =', CONFIG);
console.log('Using Configuration:', 'BUILD_WEBRTC_SHARED =', BUILD_WEBRTC_SHARED);
console.log('Using Configuration:', 'USE_LIBWEBRTC =', USE_LIBWEBRTC);

if (process.env['GYP_DEFINES'] !== '') {
  console.log('Using Configuration:', 'GYP_DEFINES =', process.env['GYP_DEFINES']);
}

if (os.platform() == 'win32') {
  console.log('      Configuration: set BUILDTYPE=Debug');
  console.log('      Configuration: set BUILDTYPE=Release');
  console.log('      Configuration: set BUILD_WEBRTC_SHARED=1');
  console.log('      Configuration: set BUILD_WEBRTC_SHARED=0');
  console.log('      Configuration: set USE_LIBWEBRTC=1');
  console.log('      Configuration: set USE_LIBWEBRTC=0');
} else {
  console.log('      Configuration: export BUILDTYPE=Debug');
  console.log('      Configuration: export BUILDTYPE=Release');
  console.log('      Configuration: export BUILD_WEBRTC_SHARED=1');
  console.log('      Configuration: export BUILD_WEBRTC_SHARED=0');
  console.log('      Configuration: export USE_LIBWEBRTC=1');
  console.log('      Configuration: export USE_LIBWEBRTC=0');
}

function linkNodeModule() {
  fs.linkSync(WEBRTC_OUT + '/webrtc-native.node', ROOT + '/build/' + CONFIG + '/webrtc-native.node');
}

function buildNodeModule() {
  console.log('Building WebRTC Node Module');

  sh('python ' + WEBRTC_SRC + path.sep + 'webrtc' + path.sep + 'build' + path.sep + 'gyp_webrtc webrtc.gyp', {
    cwd: ROOT,
    env: process.env,
    stdio: 'inherit',
  });

  sh('ninja -C ' + path.resolve(WEBRTC_SRC, 'out', CONFIG), {
    cwd: WEBRTC_SRC,
    env: process.env,
    stdio: 'inherit',
  });

  linkNodeModule();
}

function buildWebrtcShared() {
  process.env['GYP_DEFINES'] += ' component=shared_library';
  console.log('Building WebRTC Shared Library');

  sh('python ' + WEBRTC_SRC + path.sep + 'webrtc' + path.sep + 'build' + path.sep + 'gyp_webrtc webrtc-shared.gyp', {
    cwd: ROOT,
    env: process.env,
    stdio: 'inherit',
  });

  sh('ninja -C ' + path.resolve(WEBRTC_SRC, 'out', CONFIG), {
    cwd: WEBRTC_SRC,
    env: process.env,
    stdio: 'inherit',
  });

  console.log(process.env['GYP_DEFINES']);

  sh(GYP_PATH + ' --depth=. webrtc.gyp -f ninja -Goutput_dir=out', {
    cwd: ROOT,
    env: process.env,
    stdio: 'inherit',
  });

  sh('ninja -C ' + path.resolve(ROOT, 'out', 'Default'), {
    cwd: ROOT,
    env: process.env,
    stdio: 'inherit',
  });

  linkNodeModule();
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

      if (os.platform() == 'win32') {
        sh('python update.py', {
          cwd: path.resolve(WEBRTC_SRC, 'chromium', 'src', 'tools', 'clang', 'scripts'),
          env: process.env,
          stdio: 'inherit',
        });
      } else {
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
      process.env['GYP_DEFINES'] += ' clang=1';

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

  if (os.platform() !== 'win32') {
    process.env['GYP_DEFINES'] += ' target_arch=' + process.arch;
    process.env['GYP_DEFINES'] += ' host_arch=' + process.arch;
  }


  process.env['GYP_DEFINES'] += USE_LIBWEBRTC ? ' USE_LIBWEBRTC=1' : ' USE_LIBWEBRTC=0';
  process.env['GYP_DEFINES'] += BUILD_WEBRTC_SHARED ? ' BUILD_WEBRTC_SHARED=1' : ' BUILD_WEBRTC_SHARED=0';
  process.env['GYP_DEFINES'] += ' CONFIGURATION=' + CONFIG;

  if (!USE_LIBWEBRTC) {
    sh('gclient sync', {
      cwd: WEBRTC,
      env: process.env,
      stdio: 'inherit',
    });
  }

  if (BUILD_WEBRTC_SHARED) {
    buildWebrtcShared();
  } else {
    buildNodeModule();
  }
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
