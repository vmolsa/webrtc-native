{
  'variables': {
    'USE_LIBWEBRTC%': 0,
  },
  'targets': [
    {
      'target_name': 'webrtc-native',
      'type': 'loadable_module',
      'product_extension': 'node',
      'includes': [
        'build/config.gypi',
        'nodejs.gypi',
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
            '_LARGEFILE_SOURCE', 
            '_FILE_OFFSET_BITS=64',
          ],
          'include_dirs': [
            'third_party/libwebrtc/',
            'third_party/libwebrtc/webrtc',
            'third_party/libwebrtc/webrtc/system_wrappers/interface',
            'third_party/libwebrtc/third_party/jsoncpp/source/include',
          ],
        }, {
          'include_dirs': [
            'third_party/webrtc/src/',
            'third_party/webrtc/src/webrtc',
            'third_party/webrtc/src/webrtc/system_wrappers/interface',
            'third_party/webrtc/src/third_party/jsoncpp/source/include',
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
          'msvs_disabled_warnings': [ 
            4251,
          ],
          'msvs_settings': {
            'VCLinkerTool': {
              'SubSystem': '2',
              'AdditionalOptions': [ '/ignore:4199' ],
            },
          },
          'libraries': [
            '-lkernel32.lib',
            '-luser32.lib',
            '-lgdi32.lib',
            '-lwinspool.lib',
            '-lcomdlg32.lib',
            '-ladvapi32.lib',
            '-lshell32.lib',
            '-lole32.lib',
            '-loleaut32.lib',
            '-luuid.lib',
            '-lodbc32.lib',
            '-lDelayImp.lib',
            '-l"<(nodedir)\\<(host_arch)\\node"',
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
            '_LARGEFILE_SOURCE', 
            '_FILE_OFFSET_BITS=64',
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