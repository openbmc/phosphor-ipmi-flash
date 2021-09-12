version_inc = include_directories('.')

version_pre = declare_dependency(
  include_directories: [root_inc, version_inc],
  dependencies : [
    common_dep,
    firmware_dep,
  ])

version_lib = shared_library(
  'versionblob',
  'main.cpp',
  'version_handler.cpp',
  'version_handlers_builder.cpp',
  conf_h,
  implicit_include_directories: false,
  dependencies: version_pre,
  install: true,
  install_dir: get_option('libdir') / 'ipmid-providers')


version_dep = declare_dependency(
  link_with: version_lib,
  dependencies: common_pre)

if not get_option('tests').disabled()
  subdir('test')
endif