AM_DEFAULT_SOURCE_EXT = .cpp

noinst_LTLIBRARIES = libfirmwareblob_common.la
libfirmwareblob_common_la_SOURCES = \
	$(top_srcdir)/util.cpp \
	firmware_handler.cpp \
	file_handler.cpp \
	$(top_srcdir)/internal/sys.cpp \
	prepare_systemd.cpp \
	verify_systemd.cpp \
	update_systemd.cpp

if ENABLE_LPC_BRIDGE
libfirmwareblob_common_la_SOURCES += lpc_handler.cpp
if ENABLE_ASPEED_LPC
libfirmwareblob_common_la_SOURCES += lpc_aspeed.cpp
endif
if ENABLE_NUVOTON_LPC
libfirmwareblob_common_la_SOURCES += lpc_nuvoton.cpp
endif
endif

if ENABLE_PCI_BRIDGE
if ENABLE_ASPEED_P2A
libfirmwareblob_common_la_SOURCES += pci_handler.cpp
endif
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
