AM_DEFAULT_SOURCE_EXT = .cpp

pkgdatadir = $(datadir)/phosphor-ipmi-flash
dist_pkgdata_DATA =

noinst_LTLIBRARIES = libversionblob_common.la
libversionblob_common_la_SOURCES = \
	version_handlers_builder.cpp

libversionblob_common_la_CXXFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/bmc \
	$(SDBUSPLUS_CFLAGS) \
        $(PHOSPHOR_LOGGING_CFLAGS) \
        $(CODE_COVERAGE_CXXFLAGS) \
        -flto
libversionblob_common_la_LDFLAGS = \
        $(SDBUSPLUS_LIBS) \
        $(PHOSPHOR_LOGGING_LIBS) \
        $(CODE_COVERAGE_LIBS) \
        -lstdc++fs
libversionblob_common_la_LIBADD = $(top_builddir)/libfirmware_common.la
libversionblob_common_la_LIBADD += $(top_builddir)/bmc/libbmc_common.la
SUBDIRS = . test
