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
      'target_name': 'webrtc',
      'dependencies': [
        'action_before_build',
      ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")",
        "libcrtc/include"
      ],
      'sources': [
        'src/module.cc',
        'src/rtcpeerconnection.cc',
        'src/rtcdatachannel.cc',
      ],
      'cflags': [
        '-std=c++11',
      ],

      'xcode_settings': {
        'OTHER_CFLAGS': [
          '-std=c++11',
        ],
        'OTHER_LDFLAGS': [
          '-Llibcrtc/lib/',
          '-lcrtc',
        ]
      },

      'link_settings': {
        'conditions': [
          ['OS=="linux"', {
            'libraries': [
              '-L../libcrtc/lib',
              '-lcrtc',
              '-Wl,-rpath,\'$$ORIGIN/../../libcrtc/lib\''
            ],
          }],
        ],
      },
    },
    {
      'target_name': 'action_after_build',
      'type': 'none',
      'dependencies': [ 'webrtc' ],
      'conditions': [
        ['OS=="mac"', {
          'actions': [
            {
              'action_name': 'run_install_script',
              'inputs': [],
              'outputs': [ 'webrtc.node' ],
              'action': [
                'install_name_tool',
                '-change',
                './libcrtc.dylib',
                '@loader_path/../../libcrtc/lib/libcrtc.dylib', 
                'build/Release/webrtc.node',
              ],
            },
          ],
        }],
      ],
    }
  ],
}
