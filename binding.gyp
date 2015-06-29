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
            'node', 'scripts/build.js', '<(node_root_dir)',
          ],
        }
      ]
    },
  ],
}
