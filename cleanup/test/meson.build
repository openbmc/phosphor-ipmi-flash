cleanup_test = executable(
    'cleanup',
    'cleanup_handler_unittest.cpp',
    build_by_default: false,
    implicit_include_directories: false,
    include_directories: root_inc,
    dependencies: [cleanup_dep, gtest, gmock],
)
test('cleanup', cleanup_test)
