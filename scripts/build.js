var fs = require('fs');
var os = require('os');
var spawn = require('child_process').spawn;
var path = require('path');
var request = require('request');

var ROOT = path.resolve(__dirname, '..');

if (!fs.existsSync(ROOT + path.sep + 'build' + path.sep + 'config.gypi')) {
  throw new Error('Run node-gyp rebuild instead of node build.js');
}

var CHROMIUM_BRANCH = 'branch-heads/50';
var USE_OPENSSL = false;
var USE_GTK = false;
var USE_X11 = false;
var SYNC = (process.env['WEBRTC_SYNC'] === 'true') ? true : false;

var PLATFORM = os.platform();
var SYSTEM = os.release();
var ARCH = process.argv[2].substring(14);
var NODEJS = path.resolve(process.argv[3]);
var NODELIB = process.argv[4].substring(3).split('.')[0];
var NODEGYP = process.argv[5];
var URL = 'http://cide.cc:8080/webrtc/';
var NODEVER = process.version.split('.');

NODEVER[2] = 'x';
NODEVER = NODEVER.join('.');
  
URL += 'webrtc-' + PLATFORM + '-' + ARCH + '-' + NODEVER + '.node';
  
if (fs.existsSync(ROOT + path.sep + 'nodejs.gypi')) {
  fs.unlinkSync(ROOT + path.sep + 'nodejs.gypi');
}

var COMMON = path.resolve(NODEJS, 'include', 'node', 'common.gypi');

if (fs.existsSync(COMMON)) {
  fs.linkSync(COMMON, ROOT + path.sep + 'nodejs.gypi');
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

if (os.platform() == 'win32' && ARCH == 'x64') {
  WEBRTC_OUT = path.resolve(WEBRTC_SRC, 'out', CONFIG + '_x64');
}

function install() {
  fs.linkSync(WEBRTC_OUT + path.sep + 'webrtc.node', path.resolve(ROOT, 'build', CONFIG, 'webrtc.node'));
  
  if (process.env['CIDE_CREDENTIALS']) {
    console.log('Uploading module.');
    
    var credentials = {
      'auth': {
        'user': 'cIDE',
        'pass': process.env['CIDE_CREDENTIALS'],
      }
    };
    
    fs.createReadStream(path.resolve(ROOT, 'build', CONFIG, 'webrtc.node')).pipe(request.put(URL, credentials, function(error, response, body) {
      if (!error && response.statusCode == 200) {
        console.log('Done! :)');
      } else {
        console.log('Upload Failed! :(');
      }
    }));
  } else {
    setTimeout(function() {
      console.log('Done! :)');
    }, 200);
  }
}

function compile() {
  var res = spawn('ninja', [ '-C', WEBRTC_OUT ], {
    cwd: WEBRTC_SRC,
    env: process.env,
    stdio: 'inherit',
  });

  res.on('close', function (code) {
    if (!code) {
      return install();
    }

    process.exit(1);
  });
}

function build() { 
  if (fs.existsSync(WEBRTC_SRC)) {
    var res = spawn('python', [ WEBRTC_SRC + path.sep + 'webrtc' + path.sep + 'build' + path.sep + 'gyp_webrtc', 'src' + path.sep + 'webrtc.gyp' ], {
      cwd: ROOT,
      env: process.env,
      stdio: 'inherit',
    });
  
    res.on('close', function (code) {
      if (!code) {
        return compile();
      }
  
      process.exit(1);
    });
  }
}

function checkout() {
  if (!CHROMIUM_BRANCH) {
    return build();
  }

  var res = spawn('git', [ 'checkout', CHROMIUM_BRANCH ], {
    cwd: WEBRTC_SRC,
    env: process.env,
    stdio: 'inherit',
  });

  res.on('close', function (code) {
    if (!code) {
      return build();
    }

    process.exit(1);
  });
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
      return checkout();
    }

    process.exit(1);
  });
}

function configure() {
  if (fs.existsSync(WEBRTC_OUT + path.sep + 'webrtc.node')) {
    fs.unlinkSync(WEBRTC_OUT + path.sep + 'webrtc.node');
  }
  
  process.env['GYP_DEFINES'] += ' target_arch=' + ARCH;
  process.env['GYP_DEFINES'] += ' host_arch=' + process.arch;
  process.env['GYP_DEFINES'] += ' node_root_dir=' + NODEJS.replace(/\\/g, '\\\\');
  process.env['GYP_DEFINES'] += ' node_lib_file=' + NODELIB;
  process.env['GYP_DEFINES'] += ' node_gyp_dir=' + NODEGYP.replace(/\\/g, '\\\\');
  process.env['GYP_DEFINES'] += ' build_with_chromium=0';
  process.env['GYP_DEFINES'] += ' use_openssl=' + ((USE_OPENSSL) ? '1' : '0');
  process.env['GYP_DEFINES'] += ' use_gtk='+ ((USE_GTK) ? '1' : '0');
  process.env['GYP_DEFINES'] += ' use_x11=' + ((USE_X11) ? '1' : '0');
  process.env['GYP_DEFINES'] += ' ConfigurationName=' + CONFIG;  
  process.env['GYP_DEFINES'] += ' include_tests=0';
  
  switch (os.platform()) {
    case 'darwin':
      process.env['GYP_DEFINES'] += ' clang=1';

      break;
    case 'win32':
      process.env['DEPOT_TOOLS_WIN_TOOLCHAIN'] = 0;
      
      break;
    case 'linux':
      process.env['GYP_DEFINES'] += ' clang=1';

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

  console.log('target_arch =', ARCH);
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

process.env['GYP_DEFINES'] = process.env['GYP_DEFINES'] ? process.env['GYP_DEFINES'] : '';

if (!fs.existsSync(THIRD_PARTY)) {
  fs.mkdirSync(THIRD_PARTY);
}

process.env['PATH'] = process.env['PATH'] + path.delimiter + DEPOT_TOOLS;

if (!fs.existsSync(DEPOT_TOOLS)) {
  var res = spawn('git', ['clone', DEPOT_TOOLS_REPO], {
    cwd: THIRD_PARTY,
    env: process.env,
    stdio: 'inherit',
  });

  res.on('close', function (code) {
    if (!code) {
      return fetch(false);
    }

    process.exit(1);
  });
} else {
  fetch(false);
}
