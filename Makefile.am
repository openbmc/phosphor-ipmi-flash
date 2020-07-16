ACLOCAL_AMFLAGS = -I m4
AM_DEFAULT_SOURCE_EXT = .cpp

noinst_LTLIBRARIES = libfirmware_common.la
libfirmware_common_la_SOURCES = \
	util.cpp \
	internal/sys.cpp
libfirmware_common_la_CXXFLAGS = \
	-flto

SUBDIRS = .

if BUILD_BMC_HANDLER
SUBDIRS += bmc
endif

if BUILD_HOST_TOOL
SUBDIRS += tools
endif

if BUILD_CLEANUP_DELETE
SUBDIRS += cleanup
endif
