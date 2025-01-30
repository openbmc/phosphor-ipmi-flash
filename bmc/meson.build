bmc_inc = include_directories('.')

common_pre = declare_dependency(
  dependencies: [nlohmann_json_dep],
  include_directories: [root_inc, bmc_inc])

common_lib = static_library(
  'common',
  'buildjson.cpp',
  'file_handler.cpp',
  'fs.cpp',
  'general_systemd.cpp',
  'skip_action.cpp',
  implicit_include_directories: false,
  dependencies: common_pre)

common_dep = declare_dependency(
  link_with: common_lib,
  dependencies: common_pre)

if get_option('tests').allowed()
  subdir('test')
endif

subdir('firmware-handler')
subdir('version-handler')
subdir('log-handler')
