AM_DEFAULT_SOURCE_EXT = .cpp

pkgdatadir = $(datadir)/phosphor-ipmi-flash
dist_pkgdata_DATA =

noinst_LTLIBRARIES = liblogblob_common.la
liblogblob_common_la_SOURCES = \
	log_handler.cpp \
	log_handlers_builder.cpp

liblogblob_common_la_CXXFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/bmc \
	$(CODE_COVERAGE_CXXFLAGS) \
	-flto
liblogblob_common_la_LDFLAGS = \
	$(CODE_COVERAGE_LIBS) \
	-lstdc++fs
liblogblob_common_la_LIBADD = $(top_builddir)/libfirmware_common.la
liblogblob_common_la_LIBADD += $(top_builddir)/bmc/libbmc_common.la

liblogblobdir = ${libdir}/ipmid-providers
liblogblob_LTLIBRARIES = liblogblob.la
liblogblob_la_SOURCES = \
	main.cpp
liblogblob_la_LIBADD = liblogblob_common.la
liblogblob_la_LDFLAGS = \
	$(CODE_COVERAGE_LIBS) \
	-lstdc++fs \
	-log-info 0:0:0 -shared
liblogblob_la_CXXFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/bmc \
	$(CODE_COVERAGE_CXXFLAGS) \
	-flto