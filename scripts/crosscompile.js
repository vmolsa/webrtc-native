const fs = require('fs');
const path = require('path');
const spawn = require('child_process').spawn;
const rimraf = require('rimraf');
const root_path = path.resolve(__dirname, '../');

function runCross() {
  console.log('Cross compiling:', process.env['npm_config_arch']);
  const ng = spawn('node', ['node_modules/node-gyp/bin/node-gyp.js', 'rebuild', '-v', '--release', '--arch=' + process.env['npm_config_arch']]);

  ng.stdout.pipe(process.stdout);
  ng.stderr.pipe(process.stderr);

  ng.on('close', (code) => {
    console.log(`crosscompile exited with code ${code}`);
  });
}

if (process.env['npm_config_arch']) {
  if (fs.existsSync(path.resolve(root_path, 'dist'))) {
    rimraf(path.resolve(root_path, 'dist'), () => {
      runCross();
    });
  } else {
    runCross();
  }
} else {
  console.log('Run: npm run crosscompile --arch=[x86|x64|arm|arm64|mipsel]');
}