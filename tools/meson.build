tools_inc = include_directories('.')

updater_pre = [
  dependency('ipmiblob'),
  dependency('pciaccess', fallback: ['pciaccess', 'dep_pciaccess']),
  dependency('stdplus', fallback: ['stdplus', 'stdplus_dep']),
  blobs_dep,
  sys_dep]

updater_lib = static_library(
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
  conf_h,
  dependencies: updater_pre,
  include_directories: root_inc)

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

if get_option('tests').allowed()
  subdir('test')
endif
