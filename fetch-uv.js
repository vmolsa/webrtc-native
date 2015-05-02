var fs = require('fs');
var os = require('os');
var sh = require('child_process').execSync;
var path = require('path');

var ROOT = process.cwd();
var THIRD_PARTY = path.resolve(ROOT, 'third_party');
var LIBUV = path.resolve(THIRD_PARTY, 'libuv');
var LIBUV_REPO = 'https://github.com/libuv/libuv.git';

if (!fs.existsSync(LIBUV)) {
  sh('git clone ' + LIBUV_REPO, {
    cwd: THIRD_PARTY,
    env: process.env,
    stdio: 'inherit',
  });
  
  sh('./autogen.sh', {
    cwd: LIBUV,
    env: process.env,
    stdio: 'inherit',
  });

  sh('./configure', {
    cwd: LIBUV,
    env: process.env,
    stdio: 'inherit',
  });

  sh('make all', {
    cwd: LIBUV,
    env: process.env,
    stdio: 'inherit',
  });
}