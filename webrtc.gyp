{
  'variables': {
    'ROOT%': '.',
    'USE_LIBWEBRTC%': 0,
  },
  'targets': [
    {
      'target_name': 'webrtc-native',
      'type': 'loadable_module',
      'includes': [
        'build/config.gypi',
        'third_party/node/common.gypi',
      ],
      'include_dirs': [
        '<(nodedir)/src',
        '<(nodedir)/deps/uv/include',
        '<(nodedir)/deps/v8/include',      
      ],
      'defines': [
        'BUILDING_NODE_EXTENSION',
      ],
      'dependencies': [
        '<(DEPTH)/third_party/jsoncpp/jsoncpp.gyp:jsoncpp',
        '<(DEPTH)/talk/libjingle.gyp:libjingle_peerconnection',
      ],
      'sources': [
        'src/BackTrace.cc',
        'src/ArrayBuffer.cc',
        'src/EventEmitter.cc',
        'src/Module.cc',
        'src/PeerConnection.cc',
        'src/DataChannel.cc',
        'src/Observers.cc',
        'src/Wrap.cc'
      ],
      'conditions': [
        ['USE_LIBWEBRTC==1', {
          'defines': [
            'USE_LIBWEBRTC',
          ],
          'include_dirs': [
            '<(ROOT)/third_party/libwebrtc/',
            '<(ROOT)/third_party/libwebrtc/webrtc',
            '<(ROOT)/third_party/libwebrtc/webrtc/system_wrappers/interface',
            '<(ROOT)/third_party/libwebrtc/third_party/jsoncpp/source/include',
          ],
        }, {
          'include_dirs': [
            '<(ROOT)/third_party/webrtc/src/',
            '<(ROOT)/third_party/webrtc/src/webrtc',
            '<(ROOT)/third_party/webrtc/src/webrtc/system_wrappers/interface',
            '<(ROOT)/third_party/webrtc/src/third_party/jsoncpp/source/include',
          ],
        }],      
        ['OS=="linux"', {
          'defines': [
            'WEBRTC_POSIX=1',
          ],
          'cflags': [
            '-std=gnu++11',
            '-fPIC',
            '-Wno-deprecated-declarations',
            '-Wno-newline-eof',
          ],
        }],
        ['OS=="win"', {
          'defines': [
            'WEBRTC_WIN=1',
          ],
        }],
        ['OS=="mac"', {
          'xcode_settings': {
            'OTHER_CFLAGS': [
              '-std=gnu++0x',
              '-Wno-c++0x-extensions',
              '-Wno-c++11-extensions',
              '-Wno-deprecated-declarations',
              '-Wno-newline-eof',
            ],
            'DYLIB_INSTALL_NAME_BASE': '@rpath',
          },
          'libraries': [ 
            '-undefined dynamic_lookup'
          ],
          'defines': [
            '_DARWIN_USE_64_BIT_INODE=1',
            'WEBRTC_POSIX=1',
          ],
        }],
      ],      
    },
    {
      'target_name': 'All',
      'type': 'none',
      'dependencies': [
        'webrtc-native'
      ],
    },
  ],
}