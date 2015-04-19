{
  'targets': [
    {
      'target_name': 'action_before_build',
      'dependencies': [],
      'hard_dependency': 1,
      'type': 'none',
      'actions': [
        {
          'action_name': 'run_build_script',
          'inputs': [],
          'outputs': [''],
          'action': [
            'node', 'build.js'
          ],
        }
      ]
    },  
    {
      'target_name': 'webrtc-native',
      'include_dirs': [
        'third_party/webrtc/src/',
        'third_party/webrtc/src/webrtc',
        'third_party/webrtc/src/webrtc/system_wrappers/interface',
        'third_party/webrtc/src/third_party/jsoncpp/source/include',
        "<!(node -e \"require('node-arraybuffer')\")",
      ],
      'xcode_settings': {
        'OTHER_CFLAGS': [
          '-std=gnu++0x',
          '-Wno-c++0x-extensions',
          '-Wno-c++11-extensions',
        ]
      },
      'conditions': [
        ['OS=="win"', {
          'defines': [
            'WEBRTC_WIN=1',
          ],
        }, {
          'defines': [
            'WEBRTC_POSIX=1',
          ],
        }],
      ],
      'sources': [
        'src/Module.cc',
        'src/PeerConnection.cc',
        'src/DataChannel.cc',
        'src/Observers.cc'
      ],
      'dependencies': [
        'gen_libs.gyp:webrtc-build',
      ],
    },
  ],
}