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
          '-Wl,-rpath,./libcrtc/lib',
          '-lcrtc',
        ]
      },

      'link_settings': {
        'conditions': [
          ['OS=="linux"', {
            'libraries': [
              '-L../libcrtc/lib',
              '-lcrtc',
              '-Wl,-rpath,./libcrtc/lib'
            ],
          }],
        ],
      },
    }
  ],
}
