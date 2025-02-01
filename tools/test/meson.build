tool_tests = [
    'tools_bt',
    'tools_lpc',
    'tools_pci',
    'tools_net',
    'tools_updater',
    'tools_helper',
    'io',
]

foreach t : tool_tests
    test(
        t,
        executable(
            t.underscorify(),
            t + '_unittest.cpp',
            build_by_default: false,
            implicit_include_directories: false,
            include_directories: [root_inc, tools_inc],
            dependencies: [updater_dep, gtest, gmock],
        ),
    )
endforeach
