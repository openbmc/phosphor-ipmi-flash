# Ignore system headers
CODE_COVERAGE_IGNORE_PATTERN = '/include/*' '/usr/include/*' '$(includedir)/*'
# Ignore the real implementation sources for sys
CODE_COVERAGE_IGNORE_PATTERN += '$(abs_builddir)/internal/sys.*'
export CODE_COVERAGE_IGNORE_PATTERN

CODE_COVERAGE_LCOV_SHOPTS = $(CODE_COVERAGE_LCOV_SHOPTS_DEFAULT)
# Use our configuration file for lcov
CODE_COVERAGE_LCOV_SHOPTS += --config-file $(abs_srcdir)/.lcovrc
export CODE_COVERAGE_LCOV_SHOPTS

CODE_COVERAGE_LCOV_OPTIONS = $(CODE_COVERAGE_LCOV_OPTIONS_DEFAULT)
# Use our configuration file for lcov
CODE_COVERAGE_LCOV_OPTIONS += --config-file $(abs_srcdir)/.lcovrc
export CODE_COVERAGE_LCOV_OPTIONS

CODE_COVERAGE_LCOV_RMOPTS = $(CODE_COVERAGE_LCOV_RMOPTS_DEFAULT)
# Use our configuration file for lcov
CODE_COVERAGE_LCOV_RMOPTS += --config-file $(abs_srcdir)/.lcovrc
export CODE_COVERAGE_LCOV_RMOPTS

CODE_COVERAGE_GENHTML_OPTIONS = $(CODE_COVERAGE_GENHTML_OPTIONS_DEFAULT)
# Use our configuration file for genhtml
CODE_COVERAGE_GENHTML_OPTIONS += --config-file $(abs_srcdir)/.lcovrc
# Don't generate the absolute path for each file in the HTML output
CODE_COVERAGE_GENHTML_OPTIONS += --prefix $(abs_srcdir) --prefix $(abs_builddir)
export CODE_COVERAGE_GENHTML_OPTIONS

if AUTOCONF_CODE_COVERAGE_2019_01_06
include $(top_srcdir)/aminclude_static.am
clean-local: code-coverage-clean
distclean-local: code-coverage-dist-clean
else
@CODE_COVERAGE_RULES@
endif

ACLOCAL_AMFLAGS = -I m4
AM_DEFAULT_SOURCE_EXT = .cpp

noinst_LTLIBRARIES = libfirmwareblob_common.la
libfirmwareblob_common_la_SOURCES = \
	util.cpp \
	firmware_handler.cpp \
	file_handler.cpp \
	internal/sys.cpp \
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
	$(SDBUSPLUS_CFLAGS) \
	$(PHOSPHOR_LOGGING_CFLAGS) \
	$(CODE_COVERAGE_CXXFLAGS) \
	-flto

SUBDIRS = . test

if BUILD_HOST_TOOL
SUBDIRS += tools
endif
