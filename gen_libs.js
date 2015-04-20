var fs = require('fs');
var os = require('os');

var LIB_TYPE = 'a';
var WEBRTC_OUT = 'third_party/webrtc/src/out/Release';

var config = {
  targets: [{
    target_name: 'webrtc-build',
    type: 'none',
    link_settings: {
      ldflags: [],
      libraries: []
    }
  }]
};

switch (os.platform()) {
  case 'darwin':
    LIB_TYPE = 'a';
    config.targets[0].link_settings.libraries.push('-framework AppKit');
    config.targets[0].link_settings.libraries.push('-framework QTKit');
    
    break;
  default: 
    break;
}

function getLibs(path) {
  var files = fs.readdirSync(path);
  
  files.forEach(function(file) {
    if (file !== '.' && file !== '..') {
      var entry = path + '/' + file;
      var stat = fs.statSync(entry);

      if (stat.isDirectory()) {
        return getLibs(entry);
      }

      if (stat.isFile()) {
        if (entry.substr(entry.lastIndexOf('.') + 1) == LIB_TYPE) {
          config.targets[0].link_settings.libraries.push('<(module_root_dir)/' + entry);
        }
      }
    }
  });
}

getLibs(WEBRTC_OUT);
fs.writeFileSync('./gen_libs_' + os.platform() + '.gyp', JSON.stringify(config));
