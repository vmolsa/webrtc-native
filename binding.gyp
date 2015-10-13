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
            'node', 'scripts/build.js', '-Dtarget-arch=<(target_arch)', '<(node_root_dir)', '<(node_lib_file)', '<(node_gyp_dir)'
          ],
        }
      ]
    },
  ],
}
