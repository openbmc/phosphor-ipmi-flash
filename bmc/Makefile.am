AM_DEFAULT_SOURCE_EXT = .cpp

pkgdatadir = $(datadir)/phosphor-ipmi-flash
dist_pkgdata_DATA =

# reboot update is only effective for static layout
# static layout is also possible with a tarball+bmc-code-mgmt
#
# TODO: see if bmc-code-mgmt will reboot for us or if we need to support this
# variation
if ENABLE_STATIC_LAYOUT
if ENABLE_REBOOT_UPDATE
dist_pkgdata_DATA += config-static-bmc-reboot.json
else
dist_pkgdata_DATA += config-static-bmc.json
endif
endif

if ENABLE_HOST_BIOS
dist_pkgdata_DATA += config-bios.json
endif

if HAVE_SYSTEMD
systemdsystemunit_DATA = \
	phosphor-ipmi-flash-bmc-prepare.target \
	phosphor-ipmi-flash-bmc-verify.target \
	phosphor-ipmi-flash-bmc-update.target
if ENABLE_HOST_BIOS
systemdsystemunit_DATA += \
	phosphor-ipmi-flash-bios-prepare.target \
	phosphor-ipmi-flash-bios-verify.target \
	phosphor-ipmi-flash-bios-update.target
endif
endif

noinst_LTLIBRARIES = libfirmwareblob_common.la
libfirmwareblob_common_la_SOURCES = \
	buildjson.cpp \
	firmware_handler.cpp \
	file_handler.cpp \
	prepare_systemd.cpp \
	verify_systemd.cpp \
	update_systemd.cpp \
	lpc_handler.cpp

if ENABLE_ASPEED_LPC
libfirmwareblob_common_la_SOURCES += lpc_aspeed.cpp
endif

if ENABLE_NUVOTON_LPC
libfirmwareblob_common_la_SOURCES += lpc_nuvoton.cpp
endif

if ENABLE_ASPEED_P2A
libfirmwareblob_common_la_SOURCES += pci_handler.cpp
endif

libfirmwareblob_common_la_CXXFLAGS = \
	-I$(top_srcdir) \
	$(SDBUSPLUS_CFLAGS) \
        $(PHOSPHOR_LOGGING_CFLAGS) \
        $(CODE_COVERAGE_CXXFLAGS) \
        -flto
libfirmwareblob_common_la_LDFLAGS = \
        $(SDBUSPLUS_LIBS) \
        $(PHOSPHOR_LOGGING_LIBS) \
        $(CODE_COVERAGE_LIBS) \
        -lstdc++fs
libfirmwareblob_common_la_LIBADD = $(top_builddir)/libfirmware_common.la

libfirmwareblobdir = ${libdir}/ipmid-providers
libfirmwareblob_LTLIBRARIES = libfirmwareblob.la
libfirmwareblob_la_SOURCES = main.cpp
libfirmwareblob_la_LIBADD = libfirmwareblob_common.la
libfirmwareblob_la_LDFLAGS = \
	$(SDBUSPLUS_LIBS) \
	$(PHOSPHOR_LOGGING_LIBS) \
	$(CODE_COVERAGE_LIBS) \
	-lstdc++fs \
	-version-info 0:0:0 -shared
libfirmwareblob_la_CXXFLAGS = \
	-I$(top_srcdir) \
	$(SDBUSPLUS_CFLAGS) \
	$(PHOSPHOR_LOGGING_CFLAGS) \
	$(CODE_COVERAGE_CXXFLAGS) \
	-flto

SUBDIRS = . test
