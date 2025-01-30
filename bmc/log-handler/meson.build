log_inc = include_directories('.')

log_pre = declare_dependency(
  include_directories: [root_inc, log_inc],
  dependencies : [
    common_dep,
    firmware_dep,
  ])

log_lib = static_library(
  'logblob',
  'log_handler.cpp',
  'log_handlers_builder.cpp',
  implicit_include_directories: false,
  dependencies: log_pre)


log_dep = declare_dependency(
  link_with: log_lib,
  dependencies: common_pre)

shared_module(
  'logblob',
  'main.cpp',
  implicit_include_directories: false,
  dependencies: [
    log_dep,
    dependency('libipmid'),
  ],
  install: true,
  install_dir: get_option('libdir') / 'blob-ipmid')

if get_option('tests').allowed()
  subdir('test')
endif