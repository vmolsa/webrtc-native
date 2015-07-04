var os = require('os');
var spawn = require('child_process').spawn;
var versions = require('../node_versions.json');

var ROOT = process.cwd();
var MULTIBUILD = (os.platform() == 'win32') ? 'scripts\\multibuild.cmd' : 'scripts/multibuild.sh';

process.env['BUILD_WEBRTC'] = 'true';

function build(version, callback) {
  var res = spawn(MULTIBUILD, [ version ], {
    cwd: ROOT,
    env: process.env,
    stdio: 'inherit',
  });

  res.on('close', function (code) {
    callback(code == 0);
  });
}

function buildNext(index) {
  if (index < versions.length) {
    build(versions[index], function(result) {
      if (result) {
        buildNext(index + 1);
      }
    });
  }
}

buildNext(0);
