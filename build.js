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

process.env['NODEDIR'] = NODEJS;

if (fs.existsSync(ROOT + path.sep + 'nodejs.gypi')) {
  fs.unlinkSync(ROOT + path.sep + 'nodejs.gypi');
}

fs.linkSync(NODEJS + path.sep + 'common.gypi', ROOT + path.sep + 'nodejs.gypi');

var CONFIG = process.env['BUILDTYPE'] ? process.env['BUILDTYPE'] : 'Release';
var THIRD_PARTY = path.resolve(ROOT, 'third_party');
var DEPOT_TOOLS_REPO = 'https://chromium.googlesource.com/chromium/tools/depot_tools.git';

var DEPOT_TOOLS = path.resolve(THIRD_PARTY, 'depot_tools');
var WEBRTC = path.resolve(THIRD_PARTY, 'webrtc');
var WEBRTC_SRC = path.resolve(WEBRTC, 'src');
var WEBRTC_OUT = path.resolve(WEBRTC_SRC, 'out', CONFIG);

process.env['GYP_DEFINES'] = process.env['GYP_DEFINES'] ? process.env['GYP_DEFINES'] : '';

console.log('Using Configuration:', 'BUILDTYPE =', CONFIG);

if (process.env['GYP_DEFINES'] !== '') {
  console.log('Using Configuration:', 'GYP_DEFINES =', process.env['GYP_DEFINES']);
}

if (os.platform() == 'win32') {
  console.log('      Configuration: set BUILDTYPE=Debug');
  console.log('      Configuration: set BUILDTYPE=Release');
} else {
  console.log('      Configuration: export BUILDTYPE=Debug');
  console.log('      Configuration: export BUILDTYPE=Release');
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

process.env['GYP_DEFINES'] += ' target_arch=' + process.arch;
process.env['GYP_DEFINES'] += ' host_arch=' + process.arch;

// TODO(): src/build/landmines.py in third_party\webrtc\src\chromium is causing error on win32?

sh('gclient sync', {
  cwd: WEBRTC,
  env: process.env,
  stdio: 'inherit',
});

var configs = {
  variables: {
    target_arch: process.arch,
    host_arch: process.arch,
    nodedir: NODEJS,
  },
};

if (fs.existsSync(ROOT + path.sep + 'config.gypi')) {
  fs.unlinkSync(ROOT + path.sep + 'config.gypi');
}

fs.writeFileSync(ROOT + path.sep + 'config.gypi', JSON.stringify(configs));

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

fs.linkSync(WEBRTC_OUT + '/webrtc-native.node', ROOT + '/build/' + CONFIG + '/webrtc-native.node');
