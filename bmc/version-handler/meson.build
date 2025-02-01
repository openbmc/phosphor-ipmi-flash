version_inc = include_directories('.')

version_pre = declare_dependency(
    include_directories: [root_inc, version_inc],
    dependencies: [common_dep, firmware_dep],
)

version_lib = static_library(
    'versionblob',
    'version_handler.cpp',
    'version_handlers_builder.cpp',
    implicit_include_directories: false,
    dependencies: version_pre,
)


version_dep = declare_dependency(
    link_with: version_lib,
    dependencies: common_pre,
)

shared_module(
    'versionblob',
    'main.cpp',
    implicit_include_directories: false,
    dependencies: [version_dep, dependency('libipmid')],
    install: true,
    install_dir: get_option('libdir') / 'blob-ipmid',
)

if get_option('tests').allowed()
    subdir('test')
endif
