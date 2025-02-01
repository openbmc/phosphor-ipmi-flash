version_tests = [
    'json',
    'canhandle_enumerate',
    'createhandler',
    'open',
    'close',
    'read',
    'stat',
]

foreach t : version_tests
    test(
        t,
        executable(
            t.underscorify(),
            'version_' + t + '_unittest.cpp',
            build_by_default: false,
            implicit_include_directories: false,
            include_directories: [root_inc, bmc_test_inc, version_inc],
            dependencies: [version_dep, blobs_dep, gtest, gmock],
        ),
    )
endforeach
