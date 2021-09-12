project(
  'phosphor-ipmi-flash',
  'cpp',
  version: '0.1',
  meson_version: '>=0.57.0',
  default_options: [
    'cpp_std=c++20',
    'warning_level=3',
    'werror=true',
  ],
)

root_inc = include_directories('.')

# Setting up config data
conf_data = configuration_data()
conf_data.set_quoted('STATIC_HANDLER_STAGED_NAME', get_option('static-handler-staged-name'))
conf_data.set_quoted('PREPARATION_DBUS_SERVICE', get_option('preparation-dbus-service'))
conf_data.set_quoted('VERIFY_DBUS_SERVICE', get_option('verify-dbus-service'))
conf_data.set_quoted('UPDATE_DBUS_SERVICE', get_option('update-dbus-service'))
conf_data.set_quoted('BIOS_STAGED_NAME', get_option('bios-staged-name'))
conf_data.set_quoted('PREPARATION_BIOS_TARGET', get_option('preparation-bios-target'))
conf_data.set_quoted('VERIFY_BIOS_TARGET', get_option('verify-bios-target'))
conf_data.set_quoted('UPDATE_BIOS_TARGET', get_option('update-bios-target'))

conf_data.set_quoted('TARBALL_STAGED_NAME', get_option('tarball-staged-name'))
conf_data.set_quoted('HASH_FILENAME', get_option('hash-filename'))
conf_data.set_quoted('VERIFY_STATUS_FILENAME', get_option('verify-status-filename'))
conf_data.set_quoted('UPDATE_STATUS_FILENAME', get_option('update-status-filename'))
conf_data.set_quoted('BIOS_VERIFY_STATUS_FILENAME', get_option('bios-verify-status-filename'))
conf_data.set('MAPPED_ADDRESS', get_option('mapped-address'))


conf_h = configure_file(
  output: 'config.h',
  configuration: conf_data)

# Setup for the test config
if not get_option('tests').disabled()
  add_project_arguments('-DENABLE_STATIC_LAYOUT', language: 'cpp')
  add_project_arguments('-DENABLE_TARBALL_UBI', language: 'cpp')
  add_project_arguments('-DASPEED_P2A', language: 'cpp')
  add_project_arguments('-DENABLE_PCI_BRIDGE', language: 'cpp')
  add_project_arguments('-DASPEED_LPC', language: 'cpp')
  add_project_arguments('-DNUVOTON_LPC', language: 'cpp')
  add_project_arguments('-DENABLE_LPC_BRIDGE', language: 'cpp')
  add_project_arguments('-DENABLE_HOST_BIOS', language: 'cpp')
endif

# Enable both static and ubi tarball bmc image types for tests only.
assert(
  not get_option('tests').disabled() \
    or get_option('static-layout').disabled() \
    or get_option('tarball-ubi').disabled(),
  'Invalid configuration enabling both static and ubi tarball bmc image types.')

if not get_option('tarball-ubi').disabled()
  add_project_arguments('-DENABLE_TARBALL_UBI', language : 'cpp')
endif

# Enable both Aspeed and Nuvoton LPC for tests only.
assert(
  not get_option('tests').disabled() \
    or get_option('aspeed-lpc').disabled() \
    or get_option('nuvoton-lpc').disabled(),
  'Invalid configuration enabling both ASPEED and Nuvoton for LPC.')

if (not get_option('aspeed-lpc').disabled() or not get_option('nuvoton-lpc').disabled())
  add_project_arguments('-DENABLE_LPC_BRIDGE', language: 'cpp')
endif

# Enable only Aspeed P2A and Nuvoton LPC for tests only.
assert(
  not get_option('tests').disabled() \
    or get_option('aspeed-lpc').disabled() \
    or get_option('nuvoton-lpc').disabled(),
  'Invalid configuration enabling both ASPEED and Nuvoton for P2A.')


# Enable both Aspeed and Nuvoton P2A for tests only.
assert(
    not get_option('tests').disabled() \
      or get_option('aspeed-p2a').disabled() \
      or (get_option('nuvoton-p2a-vga').disabled() \
      and get_option('nuvoton-p2a-mbox').disabled()),
  'Invalid configuration enabling both ASPEED and Nuvoton for P2A.')

# Enable both Nuvoton LPC and PCI for tests only.
assert(
    not get_option('tests').disabled() \
      or get_option('nuvoton-lpc').disabled() \
      or (get_option('nuvoton-p2a-vga').disabled() \
    and get_option('nuvoton-p2a-mbox').disabled()),
  'Invalid configuration enabling both PCI and LPC of Nuvoton.')

# Enable both Nuvoton PCI method for tests only.
assert(
    not get_option('tests').disabled() \
      or get_option('nuvoton-p2a-vga').disabled() \
      or get_option('nuvoton-p2a-mbox').disabled(),
  'Invalid configuration enabling both PCI-VGA and PCI-MBOX of Nuvoton.')

if (not get_option('aspeed-p2a').disabled() \
    or get_option('nuvoton-p2a-vga').disabled() \
    or get_option('nuvoton-p2a-mbox').disabled())
  add_project_arguments('-DENABLE_PCI_BRIDGE', language: 'cpp')
endif

feature_map = {
  'host-bios'        : '-DENABLE_HOST_BIOS',
  'ppc'              : '-DENABLE_PPC',
  'reboot-update'    : '-DENABLE_REBOOT_UPDATE',
  'update-status'    : '-DENABLE_UPDATE_STATUS',
  'static-layout'    : '-DENABLE_STATIC_LAYOUT',
  'tarball-ubi'      : '-DENABLE_TARBALL_UBI',
  'aspeed-lpc'       : '-DASPEED_LPC',
  'nuvoton-lpc'      : '-DASPEED_LPC',
  'aspeed-p2a'       : '-DASPEED_P2A',
  'nuvoton-p2a-vga'  : '-DNUVOTON_P2A_VGA',
  'nuvoton-p2a-mbox' : '-DNUVOTON_P2A_MBOX',
  'net-bridge'       : '-DENABLE_NET_BRIDGE',
}

# Get the options status and build a project summary to show which flags are
# being enabled during the configuration time.

foreach option_key, option_value : feature_map
  if not get_option(option_key).disabled()
    add_project_arguments(option_value, language: 'cpp')
    summary(option_key, option_value, section : 'Enabled Features')
  endif
endforeach

sys_lib = static_library(
  'sys',
  'internal/sys.cpp',
  implicit_include_directories: false)

sys_dep = declare_dependency(
  link_with: sys_lib)

stdplus_dep = dependency('stdplus', fallback: ['stdplus', 'stdplus_dep'])
sdbusplus_dep = dependency('sdbusplus', fallback: ['sdbusplus', 'sdbusplus_dep'])
phosphor_logging_dep = dependency(
  'phosphor-logging',
  fallback : [
    'phosphor-logging',
    'phosphor_logging_dep'])

fmt_dep = dependency('fmt', required: false)
if not fmt_dep.found()
  fmt_proj = import('cmake').subproject(
    'fmt',
    options: [
      '-DCMAKE_POSITION_INDEPENDENT_CODE=ON',
      '-DMASTER_PROJECT=OFF'
    ],
    required: false)
  assert(fmt_proj.found(), 'fmtlib is required')
  fmt_dep = fmt_proj.dependency('fmt')
endif

blobs_dep = dependency('phosphor-ipmi-blobs')

ipmiblob_dep = dependency('ipmiblob')

if not get_option('tests').disabled()
  gtest = dependency('gtest', main: true, disabler: true, required: false)
  gmock = dependency('gmock', disabler: true, required: false)
  if not gtest.found() or not gmock.found()
    gtest_proj = import('cmake').subproject(
      'googletest',
      options: [
        '-DCMAKE_CXX_FLAGS=-Wno-pedantic',
      ],
      required: false)
    if gtest_proj.found()
      gtest = declare_dependency(
        dependencies: [
          dependency('threads'),
          gtest_proj.dependency('gtest'),
          gtest_proj.dependency('gtest_main'),
        ])
      gmock = gtest_proj.dependency('gmock')
    endif
  endif
endif


if not get_option('bmc-blob-handler').disabled()
  subdir('bmc')
  subdir('bmc/firmware-handler')
  subdir('bmc/version-handler')
endif

if not get_option('host-tool').disabled()
  subdir('tools')
endif

if (not get_option('cleanup-delete').disabled() or
    not get_option('tests').disabled())
  subdir('cleanup')
endif
