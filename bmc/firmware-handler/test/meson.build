firmware_tests = [
    'handler',
    'stat',
    'canhandle',
    'write',
    'writemeta',
    'open',
    'close',
    'sessionstat',
    'commit',
    'state_notyetstarted',
    'state_uploadinprogress',
    'state_verificationpending',
    'state_verificationstarted',
    'state_verificationcompleted',
    'state_updatepending',
    'state_updatestarted',
    'state_updatecompleted',
    'state_notyetstarted_tarball',
    'multiplebundle',
    'json',
    'skip',
]

foreach t : firmware_tests
    test(
        t,
        executable(
            t.underscorify(),
            'firmware_' + t + '_unittest.cpp',
            build_by_default: false,
            implicit_include_directories: false,
            include_directories: [root_inc, bmc_test_inc, firmware_inc],
            dependencies: [
                firmware_dep,
                image_mock_dep,
                triggerable_mock_dep,
                gtest,
                gmock,
            ],
        ),
    )
endforeach

file_handler_test = executable(
    'file_handler',
    'file_handler_unittest.cpp',
    build_by_default: false,
    implicit_include_directories: false,
    include_directories: [root_inc, bmc_inc, bmc_test_inc, firmware_inc],
    dependencies: [common_dep, blobs_dep, gtest, gmock],
)
test('file_handler', file_handler_test)
