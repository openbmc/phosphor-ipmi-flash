firmware_inc = include_directories('.')

# phosphor-ipmi-flash config
config_data = []
if get_option('update-type') == 'static-layout'
    if get_option('reboot-update')
        config_data += 'config-static-bmc-reboot.json'
    else
        if get_option('update-status')
            config_data += 'config-static-bmc-with-update-status.json'
        else
            config_data += 'config-static-bmc.json'
        endif
    endif
endif

if get_option('host-bios')
    config_data += 'config-bios.json'
endif

foreach data : config_data
    configure_file(
        input: data + '.in',
        output: data,
        configuration: conf_data,
        install: true,
        install_dir: get_option('datadir') / 'phosphor-ipmi-flash',
    )
endforeach

# temp files
install_data(
    'phosphor-ipmi-flash.conf',
    install_dir: get_option('libdir') / 'tmpfiles.d',
)

# systemd configs
systemd_data = [
    'phosphor-ipmi-flash-bmc-prepare.target',
    'phosphor-ipmi-flash-bmc-verify.target',
    'phosphor-ipmi-flash-bmc-update.target',
]

if get_option('host-bios')
    systemd_data += [
        'phosphor-ipmi-flash-bios-prepare.target',
        'phosphor-ipmi-flash-bios-verify.target',
        'phosphor-ipmi-flash-bios-update.target',
    ]
endif

systemd = dependency('systemd')
if systemd.found()
    foreach data : systemd_data
        configure_file(
            input: data + '.in',
            output: data,
            configuration: conf_data,
            install: true,
            install_dir: systemd.get_variable('systemd_system_unit_dir'),
        )
    endforeach
endif

firmware_source = [
    'firmware_handlers_builder.cpp',
    'firmware_handler.cpp',
    'lpc_handler.cpp',
]

if (get_option('lpc-type') == 'aspeed-lpc' or
    get_option('tests').allowed())
    firmware_source += 'lpc_aspeed.cpp'
endif

if (get_option('lpc-type') == 'nuvoton-lpc' or
    get_option('tests').allowed())
    firmware_source += 'lpc_nuvoton.cpp'
endif

if (get_option('p2a-type') == 'aspeed-p2a' or
    get_option('tests').allowed())
    firmware_source += 'pci_handler.cpp'
endif

if get_option('p2a-type') == 'nuvoton-p2a-vga'
    firmware_source += 'pci_nuvoton_handler.cpp'
endif

if get_option('p2a-type') == 'nuvoton-p2a-mbox'
    firmware_source += 'pci_nuvoton_handler.cpp'
endif

if get_option('net-bridge')
    firmware_source += 'net_handler.cpp'
endif

firmware_pre = declare_dependency(
    include_directories: [root_inc, bmc_inc, firmware_inc],
    dependencies: [
        dependency('sdbusplus', fallback: ['sdbusplus', 'sdbusplus_dep']),
        common_dep,
        blobs_dep,
        sys_dep,
    ],
)

firmware_lib = static_library(
    'firmwareblob',
    firmware_source,
    conf_h,
    implicit_include_directories: false,
    dependencies: firmware_pre,
)

firmware_dep = declare_dependency(
    link_with: firmware_lib,
    dependencies: firmware_pre,
)

shared_module(
    'firmwareblob',
    'main.cpp',
    implicit_include_directories: false,
    dependencies: [firmware_dep, dependency('libipmid')],
    install: true,
    install_dir: get_option('libdir') / 'blob-ipmid',
)

if get_option('tests').allowed()
    subdir('test')
endif
