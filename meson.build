project('factory', 'c',
  version : '0.1',
  default_options : ['warning_level=3', 'c_std=c99'])

src = [
  'src/main.c',
  'src/factory.c',
  'src/utils.c',
  'src/blueprint.c'
]

inc = include_directories('include')

executable('factory',
  src,
  include_directories : inc,
  install : true)
