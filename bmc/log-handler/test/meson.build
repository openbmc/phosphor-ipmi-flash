log_tests = ['canhandle_enumerate', 'createhandler']

foreach t : log_tests
    test(
        t,
        executable(
            t.underscorify(),
            'log_' + t + '_unittest.cpp',
            build_by_default: false,
            implicit_include_directories: false,
            include_directories: [root_inc, bmc_test_inc, log_inc],
            dependencies: [log_dep, blobs_dep, gtest, gmock],
        ),
    )
endforeach
