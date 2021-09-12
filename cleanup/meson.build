cleanup_pre = declare_dependency(
  include_directories: include_directories('.'),
  dependencies : [
    blobs_dep,
    phosphor_logging_dep])

cleanup_lib = shared_library(
  'firmwarecleanupblob',
  'cleanup.cpp',
  'fs.cpp',
  implicit_include_directories: false,
  dependencies: cleanup_pre,
  install: true,
  install_dir: get_option('libdir') / 'ipmid-providers')

cleanup_dep = declare_dependency(
  link_with: cleanup_lib,
  dependencies: cleanup_pre)

if not get_option('tests').disabled()
  subdir('test')
endif
