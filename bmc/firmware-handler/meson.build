firmware_inc = include_directories('.')

# phosphor-ipmi-flash config
config_data = []
if not get_option('static-layout').disabled() or not get_option('tests').disabled()
  if not get_option('reboot-update').disabled()
    config_data += 'config-static-bmc-reboot.json'
  else
    if not get_option('update-status').disabled()
      config_data += 'config-static-bmc-with-update-status.json'
    else
      config_data += 'config-static-bmc.json'
    endif
  endif
endif

if not get_option('host-bios').disabled()
  config_data += 'config-bios.json'
endif

foreach data : config_data
  configure_file(
    input: data + '.in',
    output: data,
    configuration: conf_data,
    install: true,
    install_dir: get_option('datadir') / 'phosphor-ipmi-flash')
endforeach

# temp files
install_data(
  'phosphor-ipmi-flash.conf',
  install_dir: '/tmp/')

# systemd configs
systemd_data = [
  'phosphor-ipmi-flash-bmc-prepare.target',
  'phosphor-ipmi-flash-bmc-verify.target',
  'phosphor-ipmi-flash-bmc-update.target',
]
if not get_option('host-bios').disabled()
  systemd_data += [
    'phosphor-ipmi-flash-bios-prepare.target',
    'phosphor-ipmi-flash-bios-verify.target',
    'phosphor-ipmi-flash-bios-update.target']
endif

systemd = dependency('systemd')
if systemd.found()
  foreach data : systemd_data
    configure_file(
      input: data + '.in',
      output: data,
      configuration: conf_data,
      install: true,
      install_dir: systemd.get_variable(pkgconfig: 'systemdsystemunitdir'))
  endforeach
endif

firmware_source = [
  'firmware_handlers_builder.cpp',
  'firmware_handler.cpp',
  'lpc_handler.cpp']

if (not get_option('aspeed-lpc').disabled() or
    not get_option('tests').disabled())
  firmware_source += 'lpc_aspeed.cpp'
endif

if (not get_option('nuvoton-lpc').disabled() or
    not get_option('tests').disabled())
  firmware_source += 'lpc_aspeed.cpp'
endif

if (not get_option('aspeed-p2a').disabled() or
    not get_option('tests').disabled())
  firmware_source += 'pci_handler.cpp'
endif

if not get_option('nuvoton-p2a-vga').disabled()
firmware_source += 'pci_nuvoton_handler.cpp'
endif

if not get_option('nuvoton-p2a-mbox').disabled()
  firmware_source += 'pci_nuvoton_handler.cpp'
endif

if not get_option('net-bridge').disabled()
  firmware_source += 'net_handler.cpp'
endif

firmware_pre = declare_dependency(
  include_directories: [root_inc, bmc_inc, firmware_inc],
  dependencies : [
    common_dep,
    blobs_dep,
    sys_dep,
    sdbusplus_dep,
    phosphor_logging_dep,
  ])

firmware_lib = shared_library(
  'firmwareblob',
  'main.cpp',
  firmware_source,
  implicit_include_directories: false,
  dependencies: firmware_pre,
  install: true,
  install_dir: get_option('libdir') / 'ipmid-providers')

firmware_dep = declare_dependency(
  link_with: firmware_lib,
  dependencies: firmware_pre)

if not get_option('tests').disabled()
  subdir('test')
endif