{
  'includes': [
    '../third_party/webrtc/src/webrtc/supplement.gypi',
    '../third_party/webrtc/src/webrtc/build/common.gypi',
    '../third_party/webrtc/src/talk/build/common.gypi',
    '../build/config.gypi',
    '../nodejs.gypi',
  ],
  'variables': {
    'include_tests%': 0,
    'third_party%': 'third_party',
    'configuration%': 'Release',
    'build_with_chromium%': 0,
    'uv_library%': 'static_library',
    'nodelib%': 'node',
  }, 
  'targets': [
    {
      'target_name': 'webrtc-native',
      'type': 'loadable_module',
      'product_extension': 'node',
      'sources': [
        'Core.cc',
        'BackTrace.cc',
        'EventEmitter.cc',
        'Observers.cc',
        'Module.cc',
        'PeerConnection.cc',
        'DataChannel.cc',
        'GetSources.cc',
        'GetUserMedia.cc',
        'MediaStream.cc',
        'MediaStreamTrack.cc',
        'MediaConstraints.cc',
        'Stats.cc',
      ],      
      'defines': [
        'BUILDING_NODE_EXTENSION',
      ],
      'dependencies': [
        '<(DEPTH)/talk/libjingle.gyp:libjingle_peerconnection', 
        '<(DEPTH)/webrtc/modules/modules.gyp:video_capture_module',
        '<(DEPTH)/webrtc/modules/modules.gyp:video_capture_module_internal_impl',
        '<(DEPTH)/webrtc/modules/modules.gyp:video_render_module',
        '<(DEPTH)/webrtc/modules/modules.gyp:video_render_module_internal_impl',
      ],
      'include_dirs': [
        '<(nodedir)/src',
        '<(nodedir)/deps/uv/include',
        '<(nodedir)/deps/v8/include',
        '<(DEPTH)/third_party/jsoncpp/source/include',
        '<(DEPTH)/third_party/libsrtp/srtp',
        '<(DEPTH)/third_party/libyuv/include',
        "<!(node -e \"require('nan')\")",
      ],
      'conditions': [ 
        ['OS=="linux"', {
          'include_dirs': [
            '/usr/include/node/',
            '/usr/include/nodejs/'
          ],
          'defines': [
            '_LARGEFILE_SOURCE', 
            '_FILE_OFFSET_BITS=64',
          ],
          'cflags': [
            '-fPIC',
            '-Wno-deprecated-declarations',
            '-Wno-unused-variable',
            '-Wno-unknown-pragmas',
            '-Wno-unused-result',
          ],
        }],
        ['OS=="win"', {
          'msvs_disabled_warnings': [ 
            4251,
            4530,
            4702,
            4199,
            4201,
            4267,
            4005,
            4506,
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
            '-l"<(nodedir)\\<(target_arch)\\<(nodelib)"',
          ],
        }],
        ['OS=="mac"', {
          'xcode_settings': {
            'OTHER_CFLAGS': [
              '-Wno-deprecated-declarations',
              '-Wno-newline-eof',
              '-Wno-unknown-pragmas',
              '-Wno-unused-result',
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
      'target_name': 'All',
      'type': 'none',
      'dependencies': [
        'webrtc-native',
      ],
    },
  ],
}
