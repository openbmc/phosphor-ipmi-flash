bmc_test_inc = include_directories('.')

image_mock_pre = declare_dependency(include_directories: [root_inc, bmc_inc])

image_mock_lib = static_library(
    'image_mock',
    'image_mock.cpp',
    implicit_include_directories: false,
    dependencies: image_mock_pre,
)

image_mock_dep = declare_dependency(
    link_with: image_mock_lib,
    dependencies: image_mock_pre,
)


triggerable_mock_pre = declare_dependency(include_directories: [root_inc])

triggerable_mock_lib = static_library(
    'triggerable_mock',
    'triggerable_mock.cpp',
    implicit_include_directories: false,
    dependencies: triggerable_mock_pre,
)

triggerable_mock_dep = declare_dependency(
    link_with: triggerable_mock_lib,
    dependencies: triggerable_mock_pre,
)
