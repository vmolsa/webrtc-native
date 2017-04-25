const request = require('request');
const targz = require('tar.gz');
const path = require('path');
const os = require('os');
const fs = require('fs');
const root_path = path.resolve(__dirname, '../');
const pkg = require(path.resolve(root_path, 'package.json'));
const base_url = 'https://github.com/vmolsa/libcrtc/releases/download'
const target_cpu = os.arch();
const libcrtc_path = path.resolve(root_path, 'libcrtc');

switch (target_cpu) {
  case 'ia32':
  case 'x32':
    target_cpu = 'x86';
    break;
  case 'mips':
    target_cpu = 'mipsel';
    break;
  default:
    break;
}

const pkg_name = 'libcrtc-' + pkg.libcrtc + '-' + os.platform()  + '-' + target_cpu + '.tar.gz'

if (!fs.existsSync(libcrtc_path) || !fs.existsSync(path.resolve(libcrtc_path, 'lib')) || !fs.existsSync(path.resolve(libcrtc_path, 'include'))) {
  console.log('Downloading:', base_url + '/' + pkg.libcrtc + '/' + pkg_name);
  console.log('Extracting package to', libcrtc_path);

  let read = request.get(base_url + '/' + pkg.libcrtc + '/' + pkg_name);
  let write = targz().createWriteStream(libcrtc_path);

  read.on('error', (error) => {
    throw error;
  });

  read.pipe(write);
}

