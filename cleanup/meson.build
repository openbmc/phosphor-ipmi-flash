cleanup_pre = declare_dependency(
    include_directories: [root_inc, include_directories('.')],
    dependencies: blobs_dep,
)

cleanup_lib = static_library(
    'firmwarecleanupblob',
    'cleanup.cpp',
    'fs.cpp',
    conf_h,
    implicit_include_directories: false,
    dependencies: cleanup_pre,
)

cleanup_dep = declare_dependency(
    link_with: cleanup_lib,
    dependencies: cleanup_pre,
)

shared_module(
    'firmwarecleanupblob',
    'main.cpp',
    implicit_include_directories: false,
    dependencies: [cleanup_dep, dependency('libipmid')],
    install: true,
    install_dir: get_option('libdir') / 'blob-ipmid',
)

if get_option('tests').allowed()
    subdir('test')
endif
