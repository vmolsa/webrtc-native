{
  'includes': [
    '../third_party/webrtc/src/talk/build/common.gypi',
    '../third_party/webrtc/src/webrtc/build/common.gypi',
    '../build/config.gypi',
    '../nodejs.gypi',
    'addon.gypi',
  ],
  'targets': [
    {
      'target_name': 'webrtc',
      'sources': [
        'Platform.cc',
        'Global.cc',
        'BackTrace.cc',
        'Configuration.cc',
        'EventEmitter.cc',
        'Observers.cc',
        'Module.cc',
        'PeerConnection.cc',
        'DataChannel.cc',
        'MediaStream.cc',
        'MediaStreamTrack.cc',
        'Stats.cc',
        'AudioSink.cc',
        'VideoSink.cc',
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
          'cflags': [
            '-Wno-deprecated-declarations',
            '-Wno-unused-variable',
            '-Wno-unknown-pragmas',
            '-Wno-unused-result',
          ],
          'cflags_cc': [
            '-Wno-non-virtual-dtor',
            '-Wno-delete-non-virtual-dtor',
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
          'msvs_disabled_warnings': [
            4267,
            4005,
            4201,
            4506,
          ],
        }],
        ['OS=="mac"', {
          'xcode_settings': {
            'OTHER_CFLAGS': [
              '-Wno-nonnull',
              '-Wno-deprecated-declarations',
              '-Wno-newline-eof',
              '-Wno-unknown-pragmas',
              '-Wno-unused-result',
            ],
          },
          'link_settings': {
            'xcode_settings': {
              'OTHER_LDFLAGS': [
                '-F.',
                '-framework WebRTC',
              ],
            },
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
