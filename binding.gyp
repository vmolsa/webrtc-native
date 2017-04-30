{
  'targets': [
    {
      'target_name': 'action_before_build',
      'dependencies': [],
      'type': 'none',
      'actions': [
        {
          'action_name': 'run_build_script',
          'inputs': [],
          'outputs': ['libcrtc'],
          'conditions': [
            ['OS=="win"', {
              'action': [
                'npm run install-libcrtc',
              ],
            }, {
              'action': [
                'npm', 'run', 'install-libcrtc',
              ],
            }],
          ],
        }
      ]
    }, {
      'target_name': 'webrtc-native',
      'dependencies': [
        'action_before_build',
      ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")",
        "dist/libcrtc/include"
      ],
      'sources': [
        'src/module.cc',
        'src/rtcpeerconnection.cc',
        'src/rtcdatachannel.cc',
        'src/mediastream.cc',
        'src/mediastreamtrack.cc',
        'src/v8-promise.cc',
      ],
      'cflags': [
        '-std=c++11',
      ],

      'xcode_settings': {
        'OTHER_CFLAGS': [
          '-std=c++11',
        ],
        'OTHER_LDFLAGS': [
          '-Ldist/libcrtc/lib/',
          '-lcrtc',
        ]
      },

      'link_settings': {
        'conditions': [
          ['OS=="linux"', {
            'libraries': [
              '-L../dist/libcrtc/lib',
              '-lcrtc',
              '-Wl,-rpath,\'$$ORIGIN/libcrtc/lib\''
            ],
          }],
        ],
      },
    },
    {
      'target_name': 'action_after_compile',
      'type': 'none',
      'dependencies': [ 'webrtc-native' ],
      'conditions': [
        ['OS=="mac"', {
          'actions': [
            {
              'action_name': 'run_install_script',
              'inputs': [],
              'outputs': [ 'webrtc-native.node' ],
              'action': [
                'install_name_tool',
                '-change',
                './libcrtc.dylib',
                '@loader_path/libcrtc/lib/libcrtc.dylib', 
                'build/Release/webrtc-native.node',
              ],
            },
          ],
        }],
      ],
    },
    {
      'target_name': 'action_after_build',
      'type': 'none',
      'dependencies': [ 'action_after_compile' ],
      'copies': [
        {
          'files': [ '<(PRODUCT_DIR)/webrtc-native.node' ],
          'destination': 'dist'
        }
      ]
    }
  ],
}
