{
  'includes': [
    '../third_party/webrtc/src/talk/build/common.gypi',
    '../build/config.gypi',
    '../nodejs.gypi',
    'addon.gypi',
  ],
  'targets': [
    {
      'target_name': 'webrtc',
      'sources': [
        'Global.cc',
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
      'dependencies': [
        '<(webrtc_root)/webrtc.gyp:webrtc_all',
      ],
      'include_dirs': [
        '<(DEPTH)/third_party/jsoncpp/source/include',
        '<(DEPTH)/third_party/libsrtp/srtp',
        '<(DEPTH)/third_party/libyuv/include',
        "<!(node -e \"require('nan')\")",
      ],
      'conditions': [
        ['OS=="linux"', {
          'sources': [
            'Platform-linux.cc'
          ],
          'cflags': [
            '-Wno-deprecated-declarations',
            '-Wno-unused-variable',
            '-Wno-unknown-pragmas',
            '-Wno-unused-result',
          ],
          'cflags_cc': [
            '-std=gnu++11',
            '-Wno-overloaded-virtual',
          ],
          'ldflags': [
            '-Wl,--unresolved-symbols=ignore-in-object-files',
          ],
          'defines': [
            'USE_BACKTRACE',
          ],
        }],
        ['OS=="win"', {
          'sources': [
            'Platform-win32.cc'
          ],
          'msvs_disabled_warnings': [
            4267,
            4005,
            4201,
            4506,
          ],
        }],
        ['OS=="mac"', {
          'sources': [
            'Platform-osx.cc'
          ],
          'xcode_settings': {
            'OTHER_CFLAGS': [
              '-Wno-nonnull',
              '-Wno-deprecated-declarations',
              '-Wno-newline-eof',
              '-Wno-unknown-pragmas',
              '-Wno-unused-result',
            ],
          },
          'defines': [
            'USE_BACKTRACE',
          ],
        }],
      ],
    },
    {
      'target_name': 'All',
      'type': 'none',
      'dependencies': [
        'webrtc',
      ],
    },
  ],
}
