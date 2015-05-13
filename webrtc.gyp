{
  'includes': [
    'third_party/webrtc/src/webrtc/build/common.gypi',
    'third_party/webrtc/src/talk/build/common.gypi',
    'build/config.gypi',
    'nodejs.gypi',
  ],
  'variables': {
    'include_tests%': 0,
    'third_party%': 'third_party',
    'configuration%': 'Release',
    'build_tests%': 0,
  }, 
  'targets': [
    {
      'target_name': 'webrtc-native',
      'type': 'loadable_module',
      'product_extension': 'node',
      'sources': [
        'src/BackTrace.cc',
        'src/EventEmitter.cc',
        'src/Observers.cc',
        'src/ArrayBuffer.cc',
        'src/Module.cc',
        'src/PeerConnection.cc',
        'src/DataChannel.cc',
        'src/GetSources.cc',
        'src/GetUserMedia.cc',
        'src/MediaStream.cc',
        'src/MediaStreamTrack.cc',
        'src/MediaConstraints.cc',
        'src/Wrap.cc',
      ],      
      'defines': [
        'BUILDING_NODE_EXTENSION',
      ],
      'dependencies': [
        '<(DEPTH)/webrtc/common.gyp:webrtc_common',
        '<(DEPTH)/third_party/libsrtp/libsrtp.gyp:libsrtp',
        '<(DEPTH)/third_party/jsoncpp/jsoncpp.gyp:jsoncpp',
        '<(DEPTH)/third_party/libyuv/libyuv.gyp:libyuv',
        '<(DEPTH)/talk/libjingle.gyp:libjingle_peerconnection',
      ],
      'include_dirs': [
        '<(nodedir)/src',
        '<(nodedir)/deps/uv/include',
        '<(nodedir)/deps/v8/include',
        '<(DEPTH)/third_party/jsoncpp/source/include',
        '<(DEPTH)/third_party/libsrtp/srtp',
        '<(DEPTH)/third_party/libyuv/include',
      ],
      'conditions': [ 
        ['OS=="linux"', {
          'defines': [
            '_LARGEFILE_SOURCE', 
            '_FILE_OFFSET_BITS=64',
          ],
          'cflags': [
            '-fPIC',
            '-Wno-deprecated-declarations',
          ],
          'conditions': [
            ['clang==1', {
              'cflags': [
                '-Wall',
                '-Wextra',
                '-Wimplicit-fallthrough',
                '-Wmissing-braces',
                '-Wreorder',
                '-Wunused-variable',
                '-Wno-address-of-array-temporary',
                '-Wthread-safety',
              ],
              'cflags_cc': [
                '-Wunused-private-field',
              ],
            }],
          ],
        }],
        ['OS=="win"', {
          'msvs_disabled_warnings': [ 
            4251,
            4530,
            4702,
            4199,
            4201,
          ],
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
            '-l"<(nodedir)\\<(target_arch)\\node"',
          ],
        }],
        ['OS=="mac"', {
          'xcode_settings': {
            'OTHER_CFLAGS': [
              '-Wno-deprecated-declarations',
              '-Wno-newline-eof',
              '-Wno-unknown-pragmas',
            ],
            'DYLIB_INSTALL_NAME_BASE': '@rpath',
          },
          'libraries': [ 
            '-undefined dynamic_lookup',
            '-framework AppKit',
            '-framework QTKit',
          ],
          'defines': [
            'USE_BACKTRACE',
            '_LARGEFILE_SOURCE', 
            '_FILE_OFFSET_BITS=64',
            '_DARWIN_USE_64_BIT_INODE=1',
          ],
        }],
      ],      
    },
    {
      'target_name': 'webrtc-gtest',
      'conditions': [ 
        ['build_tests==1', {
          'type': 'executable',
          'sources': [
            '<(DEPTH)/talk/app/webrtc/peerconnectioninterface_unittest.cc',
            '<(DEPTH)/talk/app/webrtc/datachannel_unittest.cc',
            '<(DEPTH)/webrtc/base/unittest_main.cc',
            'src/GTest.cc',
            'src/PeerConnectionTest.cc',
            'src/BackTrace.cc',
            'src/EventEmitter.cc',
            'src/Observers.cc',
          ],
          'dependencies': [
            '<(DEPTH)/testing/gtest.gyp:gtest',
            '<(DEPTH)/webrtc/common.gyp:webrtc_common',
            '<(DEPTH)/third_party/libsrtp/libsrtp.gyp:libsrtp',
            '<(DEPTH)/third_party/jsoncpp/jsoncpp.gyp:jsoncpp',
            '<(DEPTH)/third_party/libyuv/libyuv.gyp:libyuv',
            '<(DEPTH)/talk/libjingle.gyp:libjingle_peerconnection',
          ],
          'export_dependent_settings': [
            '<(DEPTH)/testing/gtest.gyp:gtest',
          ],
          'xcode_settings': {
            'OTHER_CFLAGS': [
              '-Wno-newline-eof',
              '-Wno-unknown-pragmas',
            ],
          },
          'conditions': [
            ['OS=="win"', {
              'libraries': [
                '-l"<(nodedir)\\<(target_arch)\\node"',
              ],
            }, {
              'dependencies': [
                'uv.gyp:*',
              ],
              'include_dirs': [
                '<(third_party)/libuv/include',
              ],
              'libraries': [
                '<(third_party)/libuv/.libs/libuv.a',
                '-lpthread',
                '-ldl',
              ],
            }],
          ],
        }, {
          'type': 'none',
        }],
      ],
    },    
    {
      'target_name': 'All',
      'type': 'none',
      'dependencies': [
        'webrtc-native',
      ],
      'conditions': [ 
        ['build_tests==1', {
          'dependencies': [
            'webrtc-gtest',
          ],
        }],
      ],
    },
  ],
}
