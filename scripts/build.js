var fs = require('fs');
var os = require('os');
var sh = require('child_process').execSync;
var exec = require('child_process').exec;
var spawn = require('child_process').spawn;
var path = require('path');

if (!sh) {
  sh = require('sync-exec');
}

var ROOT = process.cwd();
var NODEJS = '.';
var SYNC = false;

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

if (os.platform() == 'linux') {
  if (fs.existsSync('/usr/include/node/common.gypi')) {
    fs.createReadStream('/usr/include/node/common.gypi').pipe(fs.createWriteStream(ROOT + path.sep + 'nodejs.gypi'));
  }
} else {
  fs.linkSync(NODEJS + path.sep + 'common.gypi', ROOT + path.sep + 'nodejs.gypi');
}

var CONFIG = 'Release';

if (fs.existsSync(ROOT + path.sep + 'build' + path.sep + 'Debug')) {
  CONFIG = 'Debug';
}

var THIRD_PARTY = path.resolve(ROOT, 'third_party');
var DEPOT_TOOLS_REPO = 'https://chromium.googlesource.com/chromium/tools/depot_tools.git';
var DEPOT_TOOLS = path.resolve(THIRD_PARTY, 'depot_tools');
var WEBRTC = path.resolve(THIRD_PARTY, 'webrtc');
var WEBRTC_SRC = path.resolve(WEBRTC, 'src');
var WEBRTC_OUT = path.resolve(WEBRTC_SRC, 'out', CONFIG);
var FETCH = path.resolve(DEPOT_TOOLS, (os.platform() == 'win32') ? 'fetch.bat' : 'fetch');
var GCLIENT = path.resolve(DEPOT_TOOLS, (os.platform() == 'win32') ? 'gclient.bat' : 'gclient');

if (os.platform() == 'win32' && process.arch == 'x64') {
  WEBRTC_OUT = path.resolve(WEBRTC_SRC, 'out', CONFIG + '_x64');
}

function build() {
  if (fs.existsSync(WEBRTC_SRC)) {
    sh('python ' + WEBRTC_SRC + path.sep + 'webrtc' + path.sep + 'build' + path.sep + 'gyp_webrtc src' + path.sep + 'webrtc.gyp', {
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
    console.log('Done! :)');
  }
}

function sync() {
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

  if (!SYNC) {
    return build();
  }

  var res = spawn(GCLIENT, ['sync'], {
    cwd: WEBRTC,
    env: process.env,
    stdio: 'inherit',
  });

  res.on('close', function (code) {
    if (!code) {
      fs.closeSync(fs.openSync(THIRD_PARTY + path.sep + 'webrtc_sync', 'w'));
      return build();
    }

    process.exit(1);
  });
}

function configure() {
  process.env['GYP_DEFINES'] += ' third_party=' + THIRD_PARTY;
  process.env['GYP_DEFINES'] += ' target_arch=' + process.arch;
  process.env['GYP_DEFINES'] += ' host_arch=' + process.arch;
  process.env['GYP_DEFINES'] += ' nodedir=' + NODEJS;
  process.env['GYP_DEFINES'] += ' configuration=' + CONFIG;
  process.env['GYP_DEFINES'] += ' build_with_chromium=0';
  process.env['GYP_DEFINES'] += ' include_tests=0';
  process.env['GYP_DEFINES'] += ' uv_library=static_library';

  switch (os.platform()) {
    case 'darwin':
      process.env['GYP_DEFINES'] += ' clang=1';

      break;
    case 'win32':
      process.env['DEPOT_TOOLS_WIN_TOOLCHAIN'] = 0;
      
      break;
    case 'linux':
      process.env['GYP_DEFINES'] += ' clang=0';

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

  sync();
}

function fetch(rerun) {
  if (!fs.existsSync(WEBRTC) || !fs.existsSync(WEBRTC_SRC)) {
    if (!fs.existsSync(WEBRTC)) {
      fs.mkdirSync(WEBRTC);
    }

    var res = spawn(FETCH, ['--nohooks', 'webrtc'], {
      cwd: WEBRTC,
      env: process.env,
      stdio: 'inherit',
    });

    res.on('close', function (code) {
      if (!code) {
        return configure();
      }

      if (os.platform() == 'win32' && !rerun) {
        return fetch(true);
      }

      process.exit(1);
    });
  } else {
    configure();
  }
}

function prep() {
  process.env['GYP_DEFINES'] = process.env['GYP_DEFINES'] ? process.env['GYP_DEFINES'] : '';

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
  fetch(false);
}

// TODO(): apt-get install

prep();
