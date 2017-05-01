const request = require('request');
const path = require('path');
const os = require('os');
const fs = require('fs');
const targz = require('tar.gz');
const root_path = path.resolve(__dirname, '../');
const pkg = require(path.resolve(root_path, 'package.json'));
const base_url = 'https://github.com/vmolsa/libcrtc/releases/download'
const target_cpu = os.arch();

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

function extractPackage() {
  console.log('Extracting to:', libcrtc_path);

  fs.createReadStream(path.resolve(root_path, 'dist', pkg_name)).pipe(targz().createWriteStream(libcrtc_path));
}

if (!fs.existsSync(libcrtc_path) || !fs.existsSync(path.resolve(libcrtc_path, 'lib')) || !fs.existsSync(path.resolve(libcrtc_path, 'include'))) {
  console.log('Downloading:', base_url + '/' + pkg.libcrtc + '/' + pkg_name); 
  
  if (!fs.existsSync(libcrtc_path)) {
    fs.mkdirSync(libcrtc_path);
  }

  if (!fs.existsSync(path.resolve(root_path, 'dist', pkg_name))) {
    request.get(base_url + '/' + pkg.libcrtc + '/' + pkg_name).pipe(fs.createWriteStream(path.resolve(root_path, 'dist', pkg_name))).on('finish', () => {
      extractPackage();
    });
  } else {
    extractPackage();
  }
}

