const fs = require('fs');
const spawn = require('child_process').spawn;

if (process.env['npm_config_arch']) {
  console.log('Cross compiling:', process.env['npm_config_arch']);
  
  const ng = spawn('node', ['node_modules/node-gyp/bin/node-gyp.js', 'rebuild', '--release', '--arch=' + process.env['npm_config_arch']]);

  ng.stdout.pipe(process.stdout);
  ng.stderr.pipe(process.stderr);

  ng.on('close', (code) => {
    console.log(`crosscompile exited with code ${code}`);
  });
} else {
  console.log('Run: npm run crosscompile --arch=[x86|x64|arm|arm64|mipsel]');
}