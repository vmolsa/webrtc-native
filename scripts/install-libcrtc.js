const request = require('request');
const path = require('path');
const os = require('os');
const fs = require('fs');
const targz = require('node-tar.gz');
const root_path = path.resolve(__dirname, '../');
const pkg = require(path.resolve(root_path, 'package.json'));
const base_url = 'https://github.com/vmolsa/libcrtc/releases/download'

let target_cpu = os.arch();

if (process.env['npm_config_arch']) {
  target_cpu = process.env['npm_config_arch']
}

if (!fs.existsSync(path.resolve(root_path, 'dist'))) {
  fs.mkdirSync(path.resolve(root_path, 'dist'));
}

if (!fs.existsSync(path.resolve(root_path, 'dist', 'LICENSE'))) {
  fs.createReadStream(path.resolve(root_path, 'LICENSE')).pipe(fs.createWriteStream(path.resolve(root_path, 'dist', 'LICENSE')));
}

const libcrtc_path = path.resolve(root_path, 'dist', 'libcrtc');

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


function removePackageExit(error) {
  fs.unlinkSync(path.resolve(root_path, 'dist', pkg_name));
  throw error;
}

function extractPackage() {
  console.log('Extracting to:', libcrtc_path);

  if (fs.existsSync(path.resolve(root_path, 'dist', pkg_name))) {
    fs.createReadStream(path.resolve(root_path, 'dist', pkg_name)).pipe(targz().createWriteStream(libcrtc_path)).on('error', (error) => {
      removePackageExit(new Error('Invalid Package.'));
    }).on('finish', () => {
      fs.unlinkSync(path.resolve(root_path, 'dist', pkg_name));
    });
  } else {
    removePackageExit(new Error('Package not found.'));
  }
}

if (!fs.existsSync(libcrtc_path) || !fs.existsSync(path.resolve(libcrtc_path, 'lib')) || !fs.existsSync(path.resolve(libcrtc_path, 'include'))) {  
  if (!fs.existsSync(libcrtc_path)) {
    fs.mkdirSync(libcrtc_path);
  }

  if (!fs.existsSync(path.resolve(root_path, 'dist', pkg_name))) {
    console.log('Downloading:', base_url + '/' + pkg.libcrtc + '/' + pkg_name); 

    request.get(base_url + '/' + pkg.libcrtc + '/' + pkg_name).on('response', (res) => {
      if (res.statusCode != 200 || res.headers['content-type'] == 'text/html') {
        removePackageExit(new Error('Package not found.'));
      }
    }).pipe(fs.createWriteStream(path.resolve(root_path, 'dist', pkg_name))).on('error', (error) => {
      removePackageExit(error);
    }).on('finish', () => {
      extractPackage();
    });
  } else {
    extractPackage();
  }
}
