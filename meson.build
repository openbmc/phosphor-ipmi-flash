project(
    'phosphor-ipmi-flash',
    'cpp',
    version: '0.1',
    meson_version: '>=1.1.1',
    default_options: ['cpp_std=c++23', 'warning_level=3', 'werror=true'],
)

root_inc = include_directories('.')

# Setting up config data
conf_data = configuration_data()
conf_data.set_quoted(
    'STATIC_HANDLER_STAGED_NAME',
    get_option('static-handler-staged-name'),
)
conf_data.set_quoted(
    'PREPARATION_DBUS_SERVICE',
    get_option('preparation-dbus-service'),
)
conf_data.set_quoted('VERIFY_DBUS_SERVICE', get_option('verify-dbus-service'))
conf_data.set_quoted('UPDATE_DBUS_SERVICE', get_option('update-dbus-service'))
conf_data.set_quoted('BIOS_STAGED_NAME', get_option('bios-staged-name'))
conf_data.set_quoted(
    'PREPARATION_BIOS_TARGET',
    get_option('preparation-bios-target'),
)
conf_data.set_quoted('VERIFY_BIOS_TARGET', get_option('verify-bios-target'))
conf_data.set_quoted('UPDATE_BIOS_TARGET', get_option('update-bios-target'))

conf_data.set_quoted('TARBALL_STAGED_NAME', get_option('tarball-staged-name'))
conf_data.set_quoted('HASH_FILENAME', get_option('hash-filename'))
conf_data.set_quoted(
    'VERIFY_STATUS_FILENAME',
    get_option('verify-status-filename'),
)
conf_data.set_quoted(
    'UPDATE_STATUS_FILENAME',
    get_option('update-status-filename'),
)
conf_data.set_quoted(
    'BIOS_VERIFY_STATUS_FILENAME',
    get_option('bios-verify-status-filename'),
)
conf_data.set('MAPPED_ADDRESS', get_option('mapped-address'))
conf_data.set('NUVOTON_PCI_DID', get_option('nuvoton-pci-did'))


conf_h = configure_file(output: 'config.h', configuration: conf_data)

# Setup for the test config
if get_option('tests').allowed()
    add_project_arguments('-DENABLE_STATIC_LAYOUT', language: 'cpp')
    add_project_arguments('-DENABLE_TARBALL_UBI', language: 'cpp')
    add_project_arguments('-DASPEED_P2A', language: 'cpp')
    add_project_arguments('-DENABLE_PCI_BRIDGE', language: 'cpp')
    add_project_arguments('-DASPEED_LPC', language: 'cpp')
    add_project_arguments('-DNUVOTON_LPC', language: 'cpp')
    add_project_arguments('-DENABLE_LPC_BRIDGE', language: 'cpp')
    add_project_arguments('-DENABLE_HOST_BIOS', language: 'cpp')
endif

if get_option('lpc-type') != 'none'
    add_project_arguments('-DENABLE_LPC_BRIDGE', language: 'cpp')
endif

# Enable LPC and PCI for tests only.
assert(
    get_option('tests').allowed() \
    or get_option('lpc-type') == 'none' \
    or get_option('p2a-type') == 'none',
    'Invalid configuration enabling both PCI and LPC.',
)

if get_option('p2a-type') != 'none'
    add_project_arguments('-DENABLE_PCI_BRIDGE', language: 'cpp')
endif

feature_map = {
    'host-bios'        : '-DENABLE_HOST_BIOS',
    'ppc'              : '-DENABLE_PPC',
    'reboot-update'    : '-DENABLE_REBOOT_UPDATE',
    'update-status'    : '-DENABLE_UPDATE_STATUS',
    'net-bridge'       : '-DENABLE_NET_BRIDGE',
}

# Get the options status and build a project summary to show which flags are
# being enabled during the configuration time.

foreach option_key, option_value : feature_map
    if get_option(option_key)
        add_project_arguments(option_value, language: 'cpp')
        summary(option_key, option_value, section: 'Enabled Features')
    endif
endforeach


update_type_combo_map = {
    'static-layout'    : '-DENABLE_STATIC_LAYOUT',
    'tarball-ubi'      : '-DENABLE_TARBALL_UBI',
}

foreach option_key, option_value : update_type_combo_map
    if get_option('update-type') == option_key
        add_project_arguments(option_value, language: 'cpp')
        summary(
            option_key,
            option_value,
            section: 'Enabled Firmware Update Features',
        )
    endif
endforeach

lpc_type_combo_map = {
    'aspeed-lpc'       : '-DASPEED_LPC',
    'nuvoton-lpc'      : '-DNUVOTON_LPC',
}

foreach option_key, option_value : lpc_type_combo_map
    if get_option('lpc-type') == option_key
        add_project_arguments(option_value, language: 'cpp')
        summary(option_key, option_value, section: 'Enabled LPC Features')
    endif
endforeach

pci_type_combo_map = {
    'aspeed-p2a'       : '-DASPEED_P2A',
    'nuvoton-p2a-vga'  : '-DNUVOTON_P2A_VGA',
    'nuvoton-p2a-mbox' : '-DNUVOTON_P2A_MBOX',
}

foreach option_key, option_value : pci_type_combo_map
    if get_option('p2a-type') == option_key
        add_project_arguments(option_value, language: 'cpp')
        summary(option_key, option_value, section: 'Enabled PCI Features')
    endif
endforeach


sys_lib = static_library(
    'sys',
    'internal/sys.cpp',
    implicit_include_directories: false,
)

sys_dep = declare_dependency(link_with: sys_lib)

blobs_dep = dependency('phosphor-ipmi-blobs')
nlohmann_json_dep = dependency('nlohmann_json', include_type: 'system')

if get_option('tests').allowed()
    gtest = dependency('gtest', main: true, disabler: true, required: false)
    gmock = dependency('gmock', disabler: true, required: false)
    if not gtest.found() or not gmock.found()
        gtest_opt = import('cmake').subproject_options()
        gtest_opt.append_compile_args('c++', ['-DCMAKE_CXX_FLAGS=-Wno-pedantic'])
        gtest_proj = cmake.subproject(
            'googletest',
            options: gtest_opt,
            required: false,
        )

        if gtest_proj.found()
            gtest = declare_dependency(
                dependencies: [
                    dependency('threads'),
                    gtest_proj.dependency('gtest'),
                    gtest_proj.dependency('gtest_main'),
                ],
            )
            gmock = gtest_proj.dependency('gmock')
        endif
    endif
endif


if get_option('bmc-blob-handler').allowed()
    subdir('bmc')
endif

if get_option('host-tool').allowed()
    subdir('tools')
endif

if get_option('cleanup-delete').allowed()
    subdir('cleanup')
endif
