{
  'includes': [
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
