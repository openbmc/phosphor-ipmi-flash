tools_inc = include_directories('.')

pciaccess_dep = dependency('pciaccess', fallback: ['pciaccess', 'dep_pciaccess'])

updater_pre = [
  blobs_dep,
  fmt_dep,
  pciaccess_dep,
  stdplus_dep,
  sys_dep]

updater_lib = shared_library(
  'updater_lib',
  'updater.cpp',
  'handler.cpp',
  'helper.cpp',
  'bt.cpp',
  'lpc.cpp',
  'io.cpp',
  'net.cpp',
  'pci.cpp',
  'pciaccess.cpp',
  'p2a.cpp',
  'progress.cpp',
  dependencies: updater_pre,
  include_directories: root_inc,
  install: true,
  install_dir: get_option('libdir') / 'ipmid-providers')

updater_dep = declare_dependency(
  dependencies: updater_pre,
  include_directories: root_inc,
  link_with: updater_lib)

executable(
  'burn_my_bmc',
  'main.cpp',
  implicit_include_directories: false,
  dependencies: updater_dep,
  include_directories: root_inc,
  install: true,
  install_dir: get_option('bindir'))

if not get_option('tests').disabled()
  subdir('test')
endif
